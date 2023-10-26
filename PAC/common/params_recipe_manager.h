#ifndef PTUSA_MAIN_PARAMRECIPESMANAGER_H
#define PTUSA_MAIN_PARAMRECIPESMANAGER_H

#include "smart_ptr.h"
#include "g_device.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class ParamsRecipe
    {
    public:
        std::string name; // the name of the recipe
        std::vector<float> params; // the array of float parameters

        // constructor with name and params
        ParamsRecipe( std::string name, std::vector<float> params );

        // default constructor with empty name and params
        ParamsRecipe( );
    };

class ParamsRecipeStorage
    {
    private:
        int mRecipeCount;
        int mRecipeParamsCount;
        int mId;
    public:
        ParamsRecipeStorage( int id, int recipeCount, int recipeParamsCount);
        int getId();
        int getCount();
        int getParamsCount();
        bool isChanged;
        void setRecPar(int recNo, int parNo, float newValue);
        float getRecPar(int recNo, int parNo);
        std::vector<ParamsRecipe> recipes;

    };

class ParamsRecipeManager: public i_Lua_save_device
    {
    public:
        void evaluate();
        ParamsRecipeStorage* createRecipes(int size, int quantity);
        static ParamsRecipeManager* getInstance();

        int save_device( char *buff ) override;

        const char *get_name_in_Lua( ) const override;

    private:
        int sId = 0;
        static auto_smart_ptr <ParamsRecipeManager> sInstance;

    };

ParamsRecipeManager* G_PARAMS_RECIPE_MANAGER();

#endif //PTUSA_MAIN_PARAMRECIPESMANAGER_H
