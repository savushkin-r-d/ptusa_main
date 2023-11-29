#include "params_recipe_manager.h"

#include <utility>
#include <regex>
#include <iostream>

#include "PAC_err.h"

auto_smart_ptr<ParamsRecipeManager> ParamsRecipeManager::sInstance = nullptr;

ParamsRecipeManager *G_PARAMS_RECIPE_MANAGER( )
    {
    return ParamsRecipeManager::getInstance();
    }

ParamsRecipeManager *ParamsRecipeManager::getInstance( )
    {
    if ( sInstance.is_null() )
        {
        sInstance = new ParamsRecipeManager();
        }
    return sInstance;
    }

int ParamsRecipeManager::save_device( char *buff )
    {
    int size = fmt::format_to_n( buff, MAX_COPY_SIZE, "t.RECMAN = \n\t{{" ).size;
    for ( auto rm: recAdapters )
        {
        //size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t[{}] =", rm->getId( )).size;
        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t{{").size;
        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t\tCMD=0,").size;
        auto activeRecipe = rm->getRecStorage( )->getActiveRecipeRef();
        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t\tACT={},", rm->getActiveState()).size;
        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t\tNMR={},", rm->getRecStorage()->getActiveRecipe()).size;
        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t\tNAME='{}',", activeRecipe.name).size;

        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t\tPAR=\n\t\t\t{{\n\t\t\t").size;
        for (auto par : activeRecipe.params )
            {
            size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "{},", par).size;
            }

        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t\t}},").size;
        /*for (auto rec: rm->recipes)
            {
            size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t\t{{").size;
            size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t\tNAME='{}',", rec.name).size;

            size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t\tPAR=\n\t\t\t{{\n\t\t\t").size;
            for (auto par : rec.params)
                {
                size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "{},", par).size;
                }

            size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t\t}}\n\t\t\t}},").size;
            }*/
        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t}},").size;
        }

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t}}\n" ).size;

//    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "t.RECLOADER =\n\t{{" ).size;
//    for ( auto rm: recAdapters )
//        {
//        //size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t[{}] =", rm->getId( )).size;
//        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t{{").size;
//        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t\tCMD=0,").size;
//
//        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t}},").size;
//        }
//
//    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t}}\n" ).size;
    buff[ size ] = '\0';

    return size;
    }

const char *ParamsRecipeManager::get_name_in_Lua( ) const
    {
    return "ParamsRecipeManager";
    }

ParamsRecipeStorage *ParamsRecipeManager::createRecipes( int size, int quantity )
    {
    auto recStorage = new ParamsRecipeStorage( mId++, size, quantity);
    recPacks.push_back(recStorage);
    return recStorage;
    }

ParamsRecipeManager::ParamsRecipeManager( )
    {
    mMinSaveTimeout = 10000L;
    mLastSaveTimer = get_millisec();
    }

ParamsRecipeManager::~ParamsRecipeManager( )
    {
    for (auto rs : recPacks)
        {
        delete rs;
        }
        recPacks.clear();
    for (auto rs : recAdapters)
        {
        delete rs;
        }
        recAdapters.clear();
    }

int ParamsRecipeManager::parseDriverCmd( char *buff )
    {

    std::string cmd = buff;
    std::cout << cmd << "\n";
    std::regex rmCommand(R"lit(__RECMAN\[(\d+)\]:set_cmd\( "(\w+)", (\d+), ((?:"(.*)")|([\d\.]+)) \))lit");
    std::smatch rmMatch;
    if (std::regex_match(cmd, rmMatch, rmCommand))
        {
        printf( "Is match\n" );

        auto recMgrIdx = std::stoi( rmMatch[ 1 ].str( ));
        auto varName = rmMatch[ 2 ].str( );
        auto idx = std::stoi( rmMatch[ 3 ].str( ));
        auto strval = rmMatch[ 5 ].str( );
        auto floatval = rmMatch[ 6 ].str( ).length( ) > 0 ? std::stof( rmMatch[ 6 ].str( )) : 0;
        std::cout << recMgrIdx << ' ' << varName << ' ' << idx << " str=" << strval << " float=" << floatval
                  << std::endl;

        if ( recMgrIdx - 1 < (int)recAdapters.size( ))
            {
            recAdapters[ recMgrIdx - 1 ]->set_cmd( varName, idx, floatval, strval );
            }
        return 1;
        }
    return 0;
    }

ParamsRecipeAdapter *ParamsRecipeManager::createAdapter( ParamsRecipeStorage* recStorage )
    {
        auto adapter = new ParamsRecipeAdapter( mAdaptersId++, recStorage );
        recAdapters.push_back(adapter);
        return adapter;
    }

void ParamsRecipeManager::evaluate( )
    {
    for ( auto adapter: recAdapters )
        {
        if ( adapter->isChanged )
            {
            adapter->isChanged = false;
            adapter->serialize( );
            }
        }
    for ( auto recipeStorage: recPacks )
        {
        if ( recipeStorage->isChanged )
            {
            recipeStorage->isChanged = false;
            recipeStorage->serialize( );
            }
        }
    }

ParamsRecipe::ParamsRecipe( std::string name, std::vector<float> params )
    {
    this->name = std::move( name);
    this->params = std::move( params);
    }

ParamsRecipe::ParamsRecipe( )
    {
    this->name = "";
    this->params = {};
    }

ParamsRecipeStorage::ParamsRecipeStorage( int id, int recipeCount, int recipeParamsCount )
    {
    this->mId = id;
    this->mRecipeCount = recipeCount;
    this->mRecipeParamsCount = recipeParamsCount;
    this->isChanged = false;
    this->mActiveRecipe = 0;
    for (int i = 0; i < recipeCount; i++ )
        {
        std::vector<float> params( recipeParamsCount, 0);
        recipes.emplace_back( "none", params );
        }
    deserialize( );
    }

int ParamsRecipeStorage::getId( ) const
    {
    return mId;
    }

int ParamsRecipeStorage::getCount( ) const
    {
    return this->mRecipeCount;
    }

int ParamsRecipeStorage::getParamsCount( ) const
    {
    return this->mRecipeParamsCount;
    }

void ParamsRecipeStorage::setRecPar( int recNo, int parNo, float newValue )
    {
    if (recNo <= mRecipeCount && parNo <= mRecipeParamsCount && recNo > 0 && parNo > 0)
        {
        if (recipes[recNo - 1].params[parNo - 1] != newValue)
            {
            isChanged = true;
            recipes[recNo - 1].params[parNo - 1] = newValue;
            }
        }
    }

float ParamsRecipeStorage::getRecPar( uInt recNo, uInt parNo )
    {
    if ((int)recNo <= mRecipeCount && (int)parNo <= mRecipeParamsCount && (int)recNo > 0 && (int)parNo > 0)
        {
        return recipes[recNo - 1].params[parNo - 1];
        }
    else
        {
        return 0.0f;
        }
    }

void ParamsRecipeStorage::serialize( const std::string &filename )
    {
    std::ofstream outfile( filename ); // create an output file stream with the given filename
    if ( outfile.is_open( ))
        { // check if the file is opened successfully
        outfile << mRecipeCount << " " << mRecipeParamsCount
                << "\n"; // write the recipe_count and param_count to the file
        for ( int i = 0; i < mRecipeCount; i++ )
            { // loop through the recipes vector
            outfile << recipes[ i ].name << "\n"; // write the name of the recipe to the file
            for ( int j = 0; j < mRecipeParamsCount; j++ )
                { // loop through the params of the recipe
                outfile << recipes[ i ].params[ j ] << " "; // write each param to the file separated by space
                }
            outfile << "\n"; // write a new line after each recipe
            }
        outfile.close( ); // close the file stream
        G_LOG->debug("Successfully serialized the recipes to %s\n", filename.c_str());
        }
    else
        {
        G_LOG->debug("Failed to open %s for serialization.\n", filename.c_str());
        }
    }

void ParamsRecipeStorage::deserialize( const std::string &filename )
    {
    std::ifstream infile(filename); // create an input file stream with the given filename
    if (infile.is_open()) { // check if the file is opened successfully
        int rc, pc; // variables to store the recipe_count and param_count from the file
        infile >> rc >> pc; // read the recipe_count and param_count from the file

        if (rc != mRecipeCount || pc != mRecipeParamsCount) { // check if they match with the current values
            G_LOG->debug("The file has different values for recipe_count and param_count.\n");
            G_LOG->debug("Current values: %d, %d.\n", mRecipeCount, mRecipeParamsCount);
            G_LOG->debug("File values: %d, %d.\n", rc, pc);
            G_LOG->debug("Proceeding with deserialization anyway.\n");
            }

        int min_rc = std::min<int>(rc, mRecipeCount); // find the minimum of the two recipe_count values
        int min_pc = std::min<int>(pc, mRecipeParamsCount); // find the minimum of the two param_count values

        for (int i = 0; i < rc; i++) { // loop through the recipes in the file
            std::string name; // variable to store the name of the recipe
            std::vector<float> params(mRecipeParamsCount, 0); // vector to store the params of the recipe
            infile.ignore(); // ignore the newline character
            infile >> name; // read the name of the recipe from the file
            infile.ignore(); // ignore the newline character
            for (int j = 0; j < pc; j++) { // loop through the params in the file
                float param; // variable to store each param
                infile >> param; // read each param from the file
                if (j < min_pc)
                    {
                    params[j] = param; // add the param to the params vector
                    }
                }
            if (i < min_rc)
                {
                recipes[ i ].name = name; // replace the recipe at the index i with the deserialized recipe
                recipes[ i ].params = std::move(params); // replace the recipe at the index i with the deserialized recipe
                }
            }

        infile.close(); // close the file stream
        G_LOG->debug("Successfully deserialized the recipes from %s.\n", filename.c_str());
        }
    else {
        G_LOG->debug("Failed to open %s for deserialization.\n", filename.c_str());
        }
    }

int ParamsRecipeStorage::getActiveRecipe( ) const
    {
        return mActiveRecipe + 1;
    }

int ParamsRecipeStorage::setActiveRecipe( int recipe )
    {
     if (recipe - 1 < mRecipeCount && recipe > 0)
        {
        if (mActiveRecipe!= recipe - 1)
            {
            mActiveRecipe = recipe - 1;
            return recipe - 1;
            }
        }
        return -1;
    }

ParamsRecipe &ParamsRecipeStorage::getActiveRecipeRef( )
    {
     if (mActiveRecipe < mRecipeCount)
        {
        return recipes[mActiveRecipe];
        }
        return recipes[0];
    }

void ParamsRecipeStorage::serialize( )
    {
    std::string filename = std::string("recipes_") + std::to_string(mId) + ".serialized";
    serialize( filename );
    }

void ParamsRecipeStorage::deserialize( )
    {
    std::string filename = std::string("recipes_") + std::to_string(mId) + ".serialized";
    deserialize( filename );
    }

void ParamsRecipeAdapter::addMap( unsigned int startRecPar, unsigned int startObjPar, unsigned int quantity )
    {
    mParamsMap.emplace_back( startRecPar, startObjPar, quantity);
    }

void ParamsRecipeAdapter::loadParams( ParamsRecipeStorage *recStorage, tech_object *techObject, unsigned int recNo )
    {
    if (recStorage == nullptr || techObject == nullptr) return;
    if ((int)recNo > recStorage->getCount()) return;
    auto recSize = recStorage->getParamsCount();
    auto parSize = techObject->par_float.get_count();
    for (auto map : mParamsMap)
        {
        if ((int)map.startRecPar + (int)map.quantity > recSize || map.startObjPar + map.quantity > (int)parSize) return;
        for (int i = 0; i < (int)map.quantity; i++)
            {
            techObject->par_float[map.startObjPar + i] = recStorage->getRecPar(recNo, map.startRecPar + i);
            }
        }
        LastLoadedRecipeIndex = (int)recNo;
        LastLoadedRecipeName = recStorage->recipes[recNo-1].name;
    }

ParamsRecipeAdapter::ParamsRecipeAdapter( int id, ParamsRecipeStorage *recStorage )
    {
    this->mId = id;
    this->mRecStorage = recStorage;
    mActiveRecipes.assign( recStorage->getCount(), 1 );
    deserialize( );
    }

void ParamsRecipeAdapter::serialize( const std::string &filename )
    {
    std::ofstream outfile( filename ); // create an output file stream with the given filename
    if ( outfile.is_open( ))
        {
        outfile << mActiveRecipes.size() << "\n"; // write the recipe_count
        for ( int mActiveRecipe: mActiveRecipes )
            {
            outfile << mActiveRecipe << " "; // write the active state of the recipe to the file separated by space
            }
        }
    else
        {
        G_LOG->debug("Failed to open %s for serialization.\n", filename.c_str());
        }
    }

void ParamsRecipeAdapter::serialize( )
    {
    serialize(std::string("paramsadapter") + std::to_string(mId) + ".serialized");
    }

void ParamsRecipeAdapter::deserialize( const std::string &filename )
    {
    std::ifstream infile( filename ); // create an input file stream with the given filename
    if ( infile.is_open( ))
        {
        int recipeCount;
        int actualRecipeCount = mActiveRecipes.size();
        infile >> recipeCount;
        if ( recipeCount > (int)mActiveRecipes.size( ))
            { // check if they match with the current values
            G_LOG->debug( "The file has different values for recipe_count.\n" );
            }
        for ( int i = 0; i < recipeCount; i++ )
            {
            int active;
            infile >> active;
            if (i < actualRecipeCount)
                {
                mActiveRecipes[ i ] = active;
                }
            }
        }
    else
        {
        G_LOG->debug( "Failed to open %s for deserialization.\n", filename.c_str( ));
        }
    }

void ParamsRecipeAdapter::deserialize( )
    {
     deserialize(std::string("paramsadapter") + std::to_string(mId) + ".serialized");
    }

int ParamsRecipeAdapter::getActiveState( ) const
    {
    return mActiveRecipes[ mRecStorage->getActiveRecipe() - 1 ];
    }

void ParamsRecipeAdapter::setActiveState( int state )
    {
    if ( state )
        {
        mActiveRecipes[ mRecStorage->getActiveRecipe( ) - 1 ] = 1;
        }
    else
        {
        mActiveRecipes[ mRecStorage->getActiveRecipe( ) - 1 ] = 0;
        }
        isChanged = true;
    }

ParamsRecipeStorage *ParamsRecipeAdapter::getRecStorage( ) const
    {
    return mRecStorage;
    }

int ParamsRecipeAdapter::set_cmd( const std::string& varName, int index, float value, const std::string& strValue )
    {
    if ( varName == "ACT" )
        {
        setActiveState( (int)value );
        }
    if ( varName == "NAME" )
        {
        mRecStorage->getActiveRecipeRef().name = strValue;
        mRecStorage->isChanged = true;
        return 1;
        }
    if ( varName == "NMR" )
        {
        mRecStorage->setActiveRecipe(( int ) value );
        }
    if ( varName == "PAR" )
        {
        if ( index <= mRecStorage->getParamsCount( ) && index > 0 )
            {
            if ( mRecStorage->getActiveRecipeRef( ).params[ index - 1 ]!= value )
                {
                mRecStorage->getActiveRecipeRef( ).params[ index - 1 ] = value;
                mRecStorage->isChanged = true;
                }
            }
        }
    return  0;
    }


ParamsRecipeMapRecord::ParamsRecipeMapRecord( uInt startRecPar, uInt startObjPar, uInt quantity )
    : startRecPar( startRecPar ),
      startObjPar( startObjPar ),
      quantity( quantity )
    {

    }
