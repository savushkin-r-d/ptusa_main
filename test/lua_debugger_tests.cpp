#include "includes.h"

#include <array>
#include <cstring>
#include <string>

#include "lua_debugger.h"
#include "lua_manager.h"
#include "log.h"
#include "g_errors.h"
#include "tech_def.h"

namespace
    {
    std::size_t occurrences( const std::string& text,
        const std::string& fragment )
        {
        std::size_t count = 0;
        std::size_t position = 0;
        while ( ( position = text.find( fragment, position ) ) !=
            std::string::npos )
            {
            count++;
            position += fragment.size();
            }
        return count;
        }

    std::string extract_session_id( const std::string& response )
        {
        const std::string marker = R"("session_id":")";
        const auto start = response.find( marker );
        if ( start == std::string::npos ) return {};
        const auto value_start = start + marker.size();
        const auto end = response.find( '"', value_start );
        return response.substr( value_start, end - value_start );
        }

    class debugger_error_owner final : public i_simple_error
        {
        public:
            void set_error_params( saved_params_u_int_4* ) override {}
            const char* get_name() const override { return "TEST_DEVICE"; }
            const char* get_error_description() override
                { return "test device error"; }
            int get_error_id() override { return -1; }
            int get_state() const override { return -1; }
            u_int_4 get_serial_n() const override { return 1; }
            int get_error_type() const override { return 1; }
        };

    class lua_debugger_test : public ::testing::Test
        {
        protected:
            void SetUp() override
                {
                state = lua_open();
                luaL_openlibs( state );
                G_LUA_MANAGER->set_Lua( state );
                G_LUA_DEBUGGER->reset();
                session_id = extract_session_id(
                    raw_request( lua_debugger::CMD_CREATE_SESSION ) );
                ASSERT_FALSE( session_id.empty() );
                }

            void TearDown() override
                {
                G_LUA_DEBUGGER->reset( state );
                G_LUA_MANAGER->free_Lua();
                }

            std::string raw_request( lua_debugger::COMMAND command,
                const std::string& text = {} )
                {
                std::array<unsigned char, 4096> input{};
                std::array<unsigned char, 65536> output{};
                input[ 0 ] = command;
                std::memcpy( input.data() + 1, text.data(), text.size() );
                const auto size = lua_debugger::process_service(
                    static_cast<long>( text.size() + 1 ), input.data(),
                    output.data() );
                EXPECT_GT( size, 0 );
                return reinterpret_cast<const char*>( output.data() );
                }

            std::string request( lua_debugger::COMMAND command,
                const std::string& text = {} )
                {
                return raw_request( command, session_id + "\n" + text );
                }

            lua_State* state = nullptr;
            std::string session_id;
        };
    }

TEST_F( lua_debugger_test, creates_and_closes_session )
    {
    EXPECT_EQ( 1u, G_LUA_DEBUGGER->sessions_count() );
    EXPECT_EQ( R"({"ok":true})", request( lua_debugger::CMD_KEEP_ALIVE ) );
    EXPECT_EQ( R"({"ok":true})", request( lua_debugger::CMD_CLOSE_SESSION ) );
    EXPECT_EQ( 0u, G_LUA_DEBUGGER->sessions_count() );
    EXPECT_NE( std::string::npos,
        request( lua_debugger::CMD_KEEP_ALIVE ).find( "expired session" ) );
    }

TEST_F( lua_debugger_test, session_contains_controller_time_anchor )
    {
    const auto response = raw_request( lua_debugger::CMD_CREATE_SESSION );
    EXPECT_NE( std::string::npos,
        response.find( R"("controller_time_unix_ms":)" ) );
    EXPECT_NE( std::string::npos,
        response.find( R"("controller_time_millisec":)" ) );
    }

TEST_F( lua_debugger_test, expires_inactive_session )
    {
    G_LUA_DEBUGGER->expire_sessions_for_test(
        lua_debugger::SESSION_TIMEOUT_MS + 1 );
    EXPECT_EQ( 0u, G_LUA_DEBUGGER->sessions_count() );
    EXPECT_NE( std::string::npos,
        request( lua_debugger::CMD_KEEP_ALIVE ).find( "expired session" ) );
    }

TEST_F( lua_debugger_test, evaluates_lua_expression )
    {
    EXPECT_EQ( R"({"ok":true,"type":"number","value":3})",
        request( lua_debugger::CMD_EVALUATE, "1 + 2" ) );
    EXPECT_EQ( R"({"ok":true,"type":"string","value":"a\"b"})",
        request( lua_debugger::CMD_EVALUATE, R"("a\"b")" ) );

    const auto error = request( lua_debugger::CMD_EVALUATE, "1 +" );
    EXPECT_NE( std::string::npos, error.find( R"("ok":false)" ) );
    EXPECT_NE( std::string::npos, error.find( R"("type":"error")" ) );
    }

TEST_F( lua_debugger_test, caches_only_chart_value_changes )
    {
    ASSERT_EQ( 0, luaL_dostring( state, "debug_x = 10" ) );
    EXPECT_EQ( R"({"ok":true,"count":2})",
        request( lua_debugger::CMD_SET_CHART_EXPRESSIONS,
            "debug_x\ndebug_x * 2" ) );
    EXPECT_EQ( 2u, G_LUA_DEBUGGER->expressions_count( session_id ) );

    G_LUA_DEBUGGER->evaluate();
    G_LUA_DEBUGGER->evaluate();
    ASSERT_EQ( 0, luaL_dostring( state, "debug_x = 11" ) );
    G_LUA_DEBUGGER->evaluate();

    const auto data = request( lua_debugger::CMD_GET_CHART_DATA );
    EXPECT_NE( std::string::npos,
        data.find( R"("expression":"debug_x")" ) );
    EXPECT_NE( std::string::npos, data.find( R"("value":10)" ) );
    EXPECT_NE( std::string::npos, data.find( R"("value":11)" ) );
    EXPECT_NE( std::string::npos, data.find( R"("value":20)" ) );
    EXPECT_NE( std::string::npos, data.find( R"("value":22)" ) );
    EXPECT_EQ( 4u, occurrences( data, R"("time_ms":)" ) );

    EXPECT_EQ( R"({"ok":true})",
        request( lua_debugger::CMD_CLEAR_CHART_DATA ) );
    const auto cleared = request( lua_debugger::CMD_GET_CHART_DATA );
    EXPECT_EQ( std::string::npos, cleared.find( R"("time_ms":)" ) );
    }

TEST_F( lua_debugger_test, sessions_have_independent_expression_lists )
    {
    ASSERT_EQ( R"({"ok":true,"count":1})",
        request( lua_debugger::CMD_SET_CHART_EXPRESSIONS, "41 + 1" ) );
    const auto second_id = extract_session_id(
        raw_request( lua_debugger::CMD_CREATE_SESSION ) );
    ASSERT_FALSE( second_id.empty() );
    EXPECT_EQ( 2u, G_LUA_DEBUGGER->sessions_count() );
    EXPECT_EQ( 1u, G_LUA_DEBUGGER->expressions_count( session_id ) );
    EXPECT_EQ( 0u, G_LUA_DEBUGGER->expressions_count( second_id ) );
    }

TEST_F( lua_debugger_test, delivers_messages_to_each_current_session )
    {
    G_LUA_DEBUGGER->publish_message( "test", 4, "first message" );

    const auto second_id = extract_session_id(
        raw_request( lua_debugger::CMD_CREATE_SESSION ) );
    ASSERT_FALSE( second_id.empty() );

    const auto first_messages = request( lua_debugger::CMD_GET_MESSAGES );
    EXPECT_NE( std::string::npos, first_messages.find( "first message" ) );
    EXPECT_NE( std::string::npos,
        first_messages.find( R"("source":"test")" ) );

    const auto second_old_messages = raw_request(
        lua_debugger::CMD_GET_MESSAGES, second_id + "\n" );
    EXPECT_EQ( std::string::npos,
        second_old_messages.find( "first message" ) );

    G_LUA_DEBUGGER->publish_message( "test", 3, "shared message" );
    EXPECT_NE( std::string::npos,
        request( lua_debugger::CMD_GET_MESSAGES ).find( "shared message" ) );
    EXPECT_NE( std::string::npos, raw_request(
        lua_debugger::CMD_GET_MESSAGES, second_id + "\n" ).find(
            "shared message" ) );
    }

TEST_F( lua_debugger_test, mirrors_log_messages )
    {
    G_LOG->write_log( i_log::P_INFO, "lua debugger log test" );
    const auto messages = request( lua_debugger::CMD_GET_MESSAGES );
    EXPECT_NE( std::string::npos,
        messages.find( "lua debugger log test" ) );
    EXPECT_NE( std::string::npos, messages.find( R"("source":"log")" ) );
    EXPECT_NE( std::string::npos, messages.find( R"("priority":6)" ) );
    }

TEST_F( lua_debugger_test, receives_error_manager_errors )
    {
    debugger_error_owner owner;
    G_ERRORS_MANAGER->clear();
    G_ERRORS_MANAGER->add_error( new simple_error( &owner ) );

    G_ERRORS_MANAGER->evaluate();

    const auto messages = request( lua_debugger::CMD_GET_MESSAGES );
    EXPECT_NE( std::string::npos, messages.find( "test device error" ) );
    EXPECT_NE( std::string::npos,
        messages.find( R"("source":"error_manager")" ) );
    G_ERRORS_MANAGER->clear();
    }

TEST_F( lua_debugger_test, receives_set_err_msg_messages )
    {
    tech_object object( "TEST_OBJECT", 1, 1, "TEST_OBJECT1",
        0, 0, 1, 1, 1, 1 );

    object.set_err_msg( "test object alarm", 0, 0,
        tech_object::ERR_ALARM );

    const auto messages = request( lua_debugger::CMD_GET_MESSAGES );
    EXPECT_NE( std::string::npos, messages.find( "test object alarm" ) );
    EXPECT_NE( std::string::npos,
        messages.find( R"("source":"set_err_msg")" ) );
    }

TEST_F( lua_debugger_test, rejects_invalid_chart_expression_atomically )
    {
    EXPECT_EQ( R"({"ok":true,"count":1})",
        request( lua_debugger::CMD_SET_CHART_EXPRESSIONS, "41 + 1" ) );

    const auto error = request( lua_debugger::CMD_SET_CHART_EXPRESSIONS,
        "valid_name\n1 +" );
    EXPECT_NE( std::string::npos, error.find( R"("ok":false)" ) );
    EXPECT_EQ( 1u, G_LUA_DEBUGGER->expressions_count( session_id ) );
    }

TEST_F( lua_debugger_test, reports_protocol_errors )
    {
    std::array<unsigned char, 128> input{};
    std::array<unsigned char, 1024> output{};
    lua_debugger::process_service( 0, input.data(), output.data() );
    EXPECT_STREQ( R"({"ok":false,"error":"Missing command"})",
        reinterpret_cast<const char*>( output.data() ) );

    input[ 0 ] = lua_debugger::CMD_KEEP_ALIVE;
    const std::string invalid = "unknown\n";
    std::memcpy( input.data() + 1, invalid.data(), invalid.size() );
    lua_debugger::process_service( static_cast<long>( invalid.size() + 1 ),
        input.data(), output.data() );
    EXPECT_STREQ(
        R"({"ok":false,"error":"Invalid or expired session"})",
        reinterpret_cast<const char*>( output.data() ) );

    input[ 0 ] = 255;
    const std::string valid = session_id + "\n";
    std::memcpy( input.data() + 1, valid.data(), valid.size() );
    lua_debugger::process_service( static_cast<long>( valid.size() + 1 ),
        input.data(), output.data() );
    EXPECT_STREQ( R"({"ok":false,"error":"Unknown command"})",
        reinterpret_cast<const char*>( output.data() ) );
    }
