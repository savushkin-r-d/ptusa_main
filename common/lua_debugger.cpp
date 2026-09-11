#include "lua_debugger.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <limits>
#include <locale>
#include <random>
#include <sstream>

#include "dtime.h"
#include "lua_manager.h"
#include "tcp_cmctr.h"

namespace
    {
    constexpr std::size_t MAX_EXPRESSION_LENGTH = 1024;
    constexpr std::size_t MAX_CHART_STRING_LENGTH = 128;
    constexpr std::size_t MAX_RESPONSE_LENGTH =
        tcp_communicator::BUFSIZE - 6;

    std::string request_text( long len, const unsigned char* data )
        {
        if ( len <= 1 ) return {};

        std::string result( reinterpret_cast<const char*>( data + 1 ),
            static_cast<std::size_t>( len - 1 ) );
        while ( !result.empty() && result.back() == '\0' ) result.pop_back();
        return result;
        }

    bool split_session_request( const std::string& request,
        std::string& session_id, std::string& body )
        {
        const auto separator = request.find( '\n' );
        if ( separator == std::string::npos ) return false;
        session_id = request.substr( 0, separator );
        if ( !session_id.empty() && session_id.back() == '\r' )
            session_id.pop_back();
        body = request.substr( separator + 1 );
        return !session_id.empty();
        }
    }

lua_debugger* lua_debugger::get_instance()
    {
    static lua_debugger instance;
    return &instance;
    }

bool lua_debugger::value::operator==( const value& rhs ) const
    {
    return ok == rhs.ok && type == rhs.type && json == rhs.json;
    }

std::string lua_debugger::json_quote( const std::string& source )
    {
    std::ostringstream out;
    out << '"';
    for ( unsigned char ch : source )
        {
        switch ( ch )
            {
            case '"': out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\b': out << "\\b"; break;
            case '\f': out << "\\f"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default:
                if ( ch < 0x20 )
                    {
                    out << "\\u" << std::hex << std::setw( 4 )
                        << std::setfill( '0' ) << static_cast<int>( ch )
                        << std::dec;
                    }
                else
                    {
                    out << static_cast<char>( ch );
                    }
            }
        }
    out << '"';
    return out.str();
    }

lua_debugger::value lua_debugger::value_from_stack(
    lua_State* state, int index, std::size_t max_string_length ) const
    {
    value result;
    result.ok = true;
    result.type = lua_typename( state, lua_type( state, index ) );

    switch ( lua_type( state, index ) )
        {
        case LUA_TNIL:
            result.json = "null";
            break;
        case LUA_TBOOLEAN:
            result.json = lua_toboolean( state, index ) ? "true" : "false";
            break;
        case LUA_TNUMBER:
            {
            const auto number = lua_tonumber( state, index );
            if ( !std::isfinite( number ) )
                {
                result.json = json_quote( std::isnan( number ) ? "nan" :
                    number > 0 ? "infinity" : "-infinity" );
                break;
                }
            std::ostringstream out;
            out.imbue( std::locale::classic() );
            out << std::setprecision(
                std::numeric_limits<lua_Number>::max_digits10 ) << number;
            result.json = out.str();
            break;
            }
        case LUA_TSTRING:
            {
            std::size_t length = 0;
            const char* text = lua_tolstring( state, index, &length );
            std::string string_value( text,
                ( std::min )( length, max_string_length ) );
            if ( length > max_string_length ) string_value += "...";
            result.json = json_quote( string_value );
            break;
            }
        default:
            result.json = json_quote( "<" + result.type + ">" );
            break;
        }
    return result;
    }

lua_debugger::value lua_debugger::evaluate_source(
    const std::string& source ) const
    {
    auto* state = G_LUA_MANAGER->get_Lua();
    if ( !state ) return { false, "error", json_quote( "Lua is not initialized" ) };

    const int stack_top = lua_gettop( state );
    const std::string chunk = "return (" + source + ")";
    if ( luaL_loadbuffer( state, chunk.data(), chunk.size(), "lua debugger" ) ||
        lua_pcall( state, 0, 1, 0 ) )
        {
        const char* error = lua_tostring( state, -1 );
        value result{ false, "error", json_quote(
            error ? error : "Unknown Lua error" ) };
        lua_settop( state, stack_top );
        return result;
        }

    value result = value_from_stack( state, -1, MAX_RESPONSE_LENGTH / 2 );
    lua_settop( state, stack_top );
    return result;
    }

lua_debugger::value lua_debugger::evaluate_ref( int lua_ref ) const
    {
    auto* state = G_LUA_MANAGER->get_Lua();
    const int stack_top = lua_gettop( state );
    lua_rawgeti( state, LUA_REGISTRYINDEX, lua_ref );
    if ( lua_pcall( state, 0, 1, 0 ) )
        {
        const char* error = lua_tostring( state, -1 );
        value result{ false, "error", json_quote(
            error ? error : "Unknown Lua error" ) };
        lua_settop( state, stack_top );
        return result;
        }

    value result = value_from_stack( state, -1, MAX_CHART_STRING_LENGTH );
    lua_settop( state, stack_top );
    return result;
    }

std::string lua_debugger::create_session()
    {
    if ( sessions_.size() >= MAX_SESSIONS )
        return R"({"ok":false,"error":"Too many debugger sessions"})";

    if ( !state_ ) state_ = G_LUA_MANAGER->get_Lua();
    if ( !state_ ) return R"({"ok":false,"error":"Lua is not initialized"})";

    std::string session_id;
    do
        {
        std::random_device random;
        const auto id_value =
            ( static_cast<std::uint64_t>( random() ) << 32 ) ^
            static_cast<std::uint64_t>( random() ) ^ next_session_id_++;
        std::ostringstream id_stream;
        id_stream << std::hex << std::setw( 16 ) << std::setfill( '0' )
            << id_value;
        session_id = id_stream.str();
        }
    while ( sessions_.find( session_id ) != sessions_.end() );

    sessions_[ session_id ].last_access_ms = get_millisec();
    return R"({"ok":true,"session_id":)" + json_quote( session_id ) +
        R"(,"timeout_ms":)" + std::to_string( SESSION_TIMEOUT_MS ) + "}";
    }

std::string lua_debugger::set_expressions( session& target,
    const std::string& request )
    {
    auto* state = G_LUA_MANAGER->get_Lua();
    if ( !state ) return R"({"ok":false,"error":"Lua is not initialized"})";

    std::vector<expression> replacement;
    std::istringstream input( request );
    std::string source;
    while ( std::getline( input, source ) )
        {
        if ( !source.empty() && source.back() == '\r' ) source.pop_back();
        if ( source.empty() ) continue;
        if ( source.size() > MAX_EXPRESSION_LENGTH )
            {
            for ( auto& item : replacement )
                luaL_unref( state, LUA_REGISTRYINDEX, item.lua_ref );
            return R"({"ok":false,"error":"Expression is too long"})";
            }
        if ( replacement.size() >= MAX_EXPRESSIONS )
            {
            for ( auto& item : replacement )
                luaL_unref( state, LUA_REGISTRYINDEX, item.lua_ref );
            return R"({"ok":false,"error":"Too many expressions"})";
            }

        const std::string chunk = "return (" + source + ")";
        if ( luaL_loadbuffer( state, chunk.data(), chunk.size(), "lua chart" ) )
            {
            const char* error = lua_tostring( state, -1 );
            const std::string response = R"({"ok":false,"expression":)" +
                json_quote( source ) + R"(,"error":)" +
                json_quote( error ? error : "Unknown Lua error" ) + "}";
            lua_pop( state, 1 );
            for ( auto& item : replacement )
                luaL_unref( state, LUA_REGISTRYINDEX, item.lua_ref );
            return response;
            }

        expression item;
        item.source = source;
        item.lua_ref = luaL_ref( state, LUA_REGISTRYINDEX );
        replacement.push_back( std::move( item ) );
        }

    release_expressions( target );
    target.expressions = std::move( replacement );
    return R"({"ok":true,"count":)" +
        std::to_string( target.expressions.size() ) + "}";
    }

void lua_debugger::evaluate()
    {
    auto* current_state = G_LUA_MANAGER->get_Lua();
    if ( !current_state || ( state_ && current_state != state_ ) ) return;

    const auto time_ms = get_millisec();
    expire_sessions();
    for ( auto& session_item : sessions_ )
        {
        for ( auto& item : session_item.second.expressions )
            {
            value current = evaluate_ref( item.lua_ref );
            if ( item.samples.empty() || !( item.samples.back().data == current ) )
                {
                item.samples.push_back( { time_ms, std::move( current ) } );
                if ( item.samples.size() > MAX_SAMPLES_PER_EXPRESSION )
                    item.samples.pop_front();
                }
            }
        }
    }

std::string lua_debugger::chart_data( const session& target ) const
    {
    std::string response = R"({"ok":true,"server_time_ms":)" +
        std::to_string( get_millisec() ) + R"(,"series":[)";
    bool first_expression = true;
    for ( const auto& item : target.expressions )
        {
        if ( !first_expression ) response += ',';
        first_expression = false;
        response += R"({"expression":)" + json_quote( item.source ) +
            R"(,"samples":[)";
        bool first_sample = true;
        for ( const auto& point : item.samples )
            {
            if ( !first_sample ) response += ',';
            first_sample = false;
            response += R"({"time_ms":)" + std::to_string( point.time_ms ) +
                R"(,"ok":)" + ( point.data.ok ? "true" : "false" ) +
                R"(,"type":)" + json_quote( point.data.type ) +
                R"(,"value":)" + point.data.json + "}";
            }
        response += "]}";
        }
    response += "]}";
    return response;
    }

void lua_debugger::clear_samples( session& target )
    {
    for ( auto& item : target.expressions ) item.samples.clear();
    }

void lua_debugger::release_expressions( session& target )
    {
    auto* current_state = G_LUA_MANAGER->get_Lua();
    if ( state_ && state_ == current_state )
        {
        for ( auto& item : target.expressions )
            luaL_unref( state_, LUA_REGISTRYINDEX, item.lua_ref );
        }
    target.expressions.clear();
    }

void lua_debugger::expire_sessions()
    {
    for ( auto item = sessions_.begin(); item != sessions_.end(); )
        {
        if ( get_delta_millisec( item->second.last_access_ms ) >
            SESSION_TIMEOUT_MS )
            {
            release_expressions( item->second );
            item = sessions_.erase( item );
            }
        else
            {
            ++item;
            }
        }
    if ( sessions_.empty() ) state_ = nullptr;
    }

void lua_debugger::reset( lua_State* state )
    {
    if ( state && state != state_ ) return;
    for ( auto& item : sessions_ ) release_expressions( item.second );
    sessions_.clear();
    state_ = nullptr;
    }

long lua_debugger::write_response( const std::string& response,
    unsigned char* outdata )
    {
    if ( response.size() > MAX_RESPONSE_LENGTH )
        {
        static constexpr char RESPONSE_TOO_LARGE[] =
            R"({"ok":false,"error":"Debugger response is too large"})";
        std::memcpy( outdata, RESPONSE_TOO_LARGE,
            sizeof( RESPONSE_TOO_LARGE ) );
        return sizeof( RESPONSE_TOO_LARGE );
        }

    std::memcpy( outdata, response.c_str(), response.size() + 1 );
    return static_cast<long>( response.size() + 1 );
    }

long lua_debugger::process_service( long len, unsigned char* data,
    unsigned char* outdata )
    {
    if ( len < 1 )
        return write_response( R"({"ok":false,"error":"Missing command"})",
            outdata );

    auto* debugger = get_instance();
    const auto command = static_cast<COMMAND>( data[ 0 ] );
    const std::string text = request_text( len, data );
    if ( command == CMD_CREATE_SESSION )
        return write_response( debugger->create_session(), outdata );

    std::string session_id;
    std::string body;
    if ( !split_session_request( text, session_id, body ) )
        return write_response(
            R"({"ok":false,"error":"Missing session id"})", outdata );

    const auto found = debugger->sessions_.find( session_id );
    if ( found == debugger->sessions_.end() )
        return write_response(
            R"({"ok":false,"error":"Invalid or expired session"})", outdata );

    auto& target = found->second;
    target.last_access_ms = get_millisec();
    switch ( command )
        {
        case CMD_EVALUATE:
            {
            if ( body.empty() )
                return write_response(
                    R"({"ok":false,"error":"Missing expression"})", outdata );
            if ( body.size() > MAX_EXPRESSION_LENGTH )
                return write_response(
                    R"({"ok":false,"error":"Expression is too long"})",
                    outdata );
            const value result = debugger->evaluate_source( body );
            const std::string response = R"({"ok":)" +
                std::string( result.ok ? "true" : "false" ) +
                R"(,"type":)" + json_quote( result.type ) +
                ( result.ok ? R"(,"value":)" : R"(,"error":)" ) +
                result.json + "}";
            return write_response( response, outdata );
            }
        case CMD_SET_CHART_EXPRESSIONS:
            return write_response(
                debugger->set_expressions( target, body ), outdata );
        case CMD_GET_CHART_DATA:
            return write_response( debugger->chart_data( target ), outdata );
        case CMD_CLEAR_CHART_DATA:
            debugger->clear_samples( target );
            return write_response( R"({"ok":true})", outdata );
        case CMD_CLOSE_SESSION:
            debugger->release_expressions( target );
            debugger->sessions_.erase( found );
            if ( debugger->sessions_.empty() ) debugger->state_ = nullptr;
            return write_response( R"({"ok":true})", outdata );
        case CMD_KEEP_ALIVE:
            return write_response( R"({"ok":true})", outdata );
        default:
            return write_response(
                R"({"ok":false,"error":"Unknown command"})", outdata );
        }
    }

#ifdef PTUSA_TEST
std::size_t lua_debugger::sessions_count() const
    {
    return sessions_.size();
    }

std::size_t lua_debugger::expressions_count(
    const std::string& session_id ) const
    {
    const auto found = sessions_.find( session_id );
    return found == sessions_.end() ? 0 : found->second.expressions.size();
    }

void lua_debugger::expire_sessions_for_test( std::uint32_t elapsed_ms )
    {
    const auto now = get_millisec();
    for ( auto& item : sessions_ )
        item.second.last_access_ms = now - elapsed_ms;
    expire_sessions();
    }
#endif
