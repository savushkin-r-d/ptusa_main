#ifndef PTUSA_MAIN_PARAMRECIPESMANAGER_H
#define PTUSA_MAIN_PARAMRECIPESMANAGER_H

#include "smart_ptr.h"
#include "g_device.h"
#include "tech_def.h"

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
        int mActiveRecipe;
    public:
        ParamsRecipeStorage( int id, int recipeCount, int recipeParamsCount);
        int getId() const;
        int getCount() const;
        int getParamsCount() const;
        bool isChanged;
        void setRecPar(int recNo, int parNo, float newValue);
        float getRecPar( uInt recNo, uInt parNo);
        void serialize(const std::string& filename);
        void deserialize(const std::string& filename);
        int getActiveRecipe() const;
        int setActiveRecipe(int recipe);
        ParamsRecipe& getActiveRecipeRef();
        std::vector<ParamsRecipe> recipes;

    };

struct ParamsRecipeMapRecord
    {
        uInt startRecPar;
        uInt startObjPar;
        uInt quantity;
        ParamsRecipeMapRecord(uInt startRecPar, uInt startObjPar, uInt quantity);
    };

class ParamsRecipeAdapter
    {
    private:
        std::vector<ParamsRecipeMapRecord> paramsMap;
        ParamsRecipeStorage* recStorage;
    public:
        explicit ParamsRecipeAdapter( ParamsRecipeStorage *recStorage );
        void addMap(uInt startRecPar, uInt startObjPar, uInt quantity);
        void loadParams(ParamsRecipeStorage* recStorage, tech_object* techObject, uInt recNo);
    };

class ParamsRecipeManager: public i_Lua_save_device
    {
    public:
        void evaluate();
        ParamsRecipeStorage* createRecipes(int size, int quantity);
        ParamsRecipeAdapter* createAdapter( ParamsRecipeStorage* recStorage );
        ~ParamsRecipeManager();
        static ParamsRecipeManager* getInstance();

        int save_device( char *buff ) override;
        int parseDriverCmd( char *buff);

        const char *get_name_in_Lua( ) const override;

        std::vector<ParamsRecipeStorage*> recPacks;
        std::vector<ParamsRecipeAdapter*> recAdapters;

        ParamsRecipeManager(ParamsRecipeManager &outer) = delete;
        void operator=(const ParamsRecipeManager &) = delete;

    protected:
        ParamsRecipeManager();

    private:
        int sId = 0;
        unsigned long lastSaveTimer;
        unsigned long minSaveTimeout;
        static auto_smart_ptr <ParamsRecipeManager> sInstance;

    };

ParamsRecipeManager* G_PARAMS_RECIPE_MANAGER();

#endif //PTUSA_MAIN_PARAMRECIPESMANAGER_H
