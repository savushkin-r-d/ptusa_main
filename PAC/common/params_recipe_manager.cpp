#include "params_recipe_manager.h"

#include <utility>

#include "PAC_err.h"

auto_smart_ptr<ParamsRecipeManager> ParamsRecipeManager::sInstance;

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
    int size = fmt::format_to_n( buff, MAX_COPY_SIZE, "t.RECMAN = \n\t{\n\t}" ).size;
    buff[ size ] = '\0';

    return size;
    }

const char *ParamsRecipeManager::get_name_in_Lua( ) const
    {
    return "ParamsRecipeManager";
    }

ParamsRecipeStorage *ParamsRecipeManager::createRecipes( int size, int quantity )
    {
    return nullptr;
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

int ParamsRecipeStorage::getId( )
    {
    return mId;
    }

int ParamsRecipeStorage::getCount( )
    {
    return this->mRecipeCount;
    }

int ParamsRecipeStorage::getParamsCount( )
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
