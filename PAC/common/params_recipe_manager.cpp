#include "params_recipe_manager.h"

#include <utility>

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
    int size = fmt::format_to_n( buff, MAX_COPY_SIZE, "t.RECMAN = \n\t{{\n" ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\t}}\n" ).size;
    buff[ size ] = '\0';

    return size;
    }

const char *ParamsRecipeManager::get_name_in_Lua( ) const
    {
    return "ParamsRecipeManager";
    }

ParamsRecipeStorage *ParamsRecipeManager::createRecipes( int size, int quantity )
    {
    recPacks.emplace_back(sId, size, quantity);
    return &recPacks[sId++];
    }

ParamsRecipeManager::ParamsRecipeManager( )
    {
    minSaveTimeout = 10000L;
    lastSaveTimer = get_millisec();
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
    for (int i = 0; i < recipeCount; i++ )
        {
        std::vector<float> params( recipeParamsCount, 0);
        ParamsRecipe  recipe("none", params);
        recipes.push_back( recipe);
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

float ParamsRecipeStorage::getRecPar( int recNo, int parNo )
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
