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
    for ( auto rm: recPacks )
        {
        //size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t[{}] =", rm->getId( )).size;
        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t{{").size;
        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t\tCMD=0,").size;
        auto activeRecipe = rm->getActiveRecipeRef( );
        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t\tNMR={},", rm->getActiveRecipe()).size;
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

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "t.REСLOADER = \n\t{{" ).size;
    for ( auto rm: recPacks )
        {
        //size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t[{}] =", rm->getId( )).size;
        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t{{").size;
        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t\tCMD=0,").size;

        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t\t}},").size;
        }

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t}}\n" ).size;
    buff[ size ] = '\0';

    return size;
    }

const char *ParamsRecipeManager::get_name_in_Lua( ) const
    {
    return "ParamsRecipeManager";
    }

ParamsRecipeStorage *ParamsRecipeManager::createRecipes( int size, int quantity )
    {
    auto recStorage = new ParamsRecipeStorage(sId++, size, quantity);
    recPacks.push_back(recStorage);
    return recStorage;
    }

ParamsRecipeManager::ParamsRecipeManager( )
    {
    minSaveTimeout = 10000L;
    lastSaveTimer = get_millisec();
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
    std::regex rmCommand(R"lit(__RECMAN\[(\d+)\]\[(\d+)\]:set_cmd\( "(\w+)", (\d+), ([\w\."]+) \))lit");
    std::smatch rmMatch;
    if (std::regex_match(cmd, rmMatch, rmCommand))
        {
        printf("Is match\n");
        std::cout << rmMatch[1].str() << ' ' << rmMatch[2].str() << ' ' << rmMatch[3].str() << ' ' << rmMatch[4].str() << ' ' << rmMatch[5].str() << '\n';
        auto recMgrIdx = std::stoi(rmMatch[1].str());
        auto recIdx = std::stoi(rmMatch[2].str());
        auto varName = rmMatch[3].str();
        auto idx = std::stoi( rmMatch[4].str());
        auto val = rmMatch[5].str();

        return 1;
        }
    return 0;
    }

ParamsRecipeAdapter *ParamsRecipeManager::createAdapter( ParamsRecipeStorage* recStorage )
    {
        auto adapter = new ParamsRecipeAdapter( recStorage );
        recAdapters.push_back(adapter);
        return adapter;
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
    if (recNo < mRecipeCount && parNo < mRecipeParamsCount)
        {
        if (recipes[recNo].params[parNo] != newValue)
            {
            isChanged = true;
            recipes[recNo].params[parNo] = newValue;
            }
        }
    }

float ParamsRecipeStorage::getRecPar( uInt recNo, uInt parNo )
    {
    if (recNo < mRecipeCount && parNo < mRecipeParamsCount)
        {
        return recipes[recNo].params[parNo];
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
            G_LOG->debug("File values: %d, %d.\n", mRecipeCount, mRecipeParamsCount);
            G_LOG->debug("Proceeding with deserialization anyway.\n");
            }

        int min_rc = std::min<int>(rc, mRecipeCount); // find the minimum of the two recipe_count values
        int min_pc = std::min<int>(pc, mRecipeParamsCount); // find the minimum of the two param_count values

        for (int i = 0; i < min_rc; i++) { // loop through the recipes in the file
            std::string name; // variable to store the name of the recipe
            std::vector<float> params; // vector to store the params of the recipe
            infile.ignore(); // ignore the newline character after reading the recipe_count and param_count
            std::getline(infile, name); // read the name of the recipe from the file
            for (int j = 0; j < min_pc; j++) { // loop through the params in the file
                float param; // variable to store each param
                infile >> param; // read each param from the file
                params.push_back(param); // add the param to the params vector
                }
            ParamsRecipe recipe(name, params); // create a recipe object with the name and params
            recipes[i] = recipe; // replace the recipe at the index i with the deserialized recipe
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
        return mActiveRecipe;
    }

int ParamsRecipeStorage::setActiveRecipe( int recipe )
    {
     if (recipe < mRecipeCount)
        {
        if (mActiveRecipe!= recipe)
            {
            mActiveRecipe = recipe;
            return recipe;
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

void ParamsRecipeAdapter::addMap( uInt startRecPar, uInt startObjPar, uInt quantity )
    {
    paramsMap.emplace_back(startRecPar, startObjPar, quantity);
    }

void ParamsRecipeAdapter::loadParams( ParamsRecipeStorage *recStorage, tech_object *techObject, uInt recNo )
    {
    if (recStorage == nullptr || techObject == nullptr) return;
    if (recNo >= recStorage->getCount()) return;
    auto recSize = recStorage->getParamsCount();
    auto parSize = techObject->par_float.get_count();
    for (auto map : paramsMap)
        {
        if (map.startRecPar + map.quantity > recSize || map.startObjPar + map.quantity > parSize) return;
        for (int i = 0; i < map.quantity; i++)
            {
            techObject->par_float[map.startObjPar + i] = recStorage->getRecPar(recNo, map.startRecPar + i);
            }
        }
    }

ParamsRecipeAdapter::ParamsRecipeAdapter( ParamsRecipeStorage *recStorage )
    {
    this->recStorage = recStorage;
    }

ParamsRecipeMapRecord::ParamsRecipeMapRecord( uInt startRecPar, uInt startObjPar, uInt quantity )
    : startRecPar( startRecPar ),
      startObjPar( startObjPar ),
      quantity( quantity )
    {

    }
