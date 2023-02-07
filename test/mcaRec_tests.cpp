#include "mcaRec_tests.h"

using namespace ::testing;

TEST( TMediumRecipeManager, LoadFromFile )
    {
    TMediumRecipeManager mngr( TMediumRecipeManager::MT_CAUSTIC );

    auto recipeNo = 0;
    mngr.ResetRecipeToDefaults( recipeNo );
    auto fileName = "tmp1.txt";
    EXPECT_EQ( 0, mngr.SaveToFile( fileName ) );
    EXPECT_EQ( 0, mngr.LoadFromFile( fileName ) );
    }

TEST( TMediumRecipeManager, CopyRecipe )
    {
    TMediumRecipeManager mngr( TMediumRecipeManager::MT_CAUSTIC );
    mngr.CopyRecipe();
    }

TEST( TMediumRecipeManager, PasteRecipe )
    {
    TMediumRecipeManager mngr( TMediumRecipeManager::MT_CAUSTIC );
    mngr.PasteRecipe();
    }

TEST( TMediumRecipeManager, NullifyRecipe )
    {
    TMediumRecipeManager mngr( TMediumRecipeManager::MT_CAUSTIC );
    mngr.NullifyRecipe();
    }

TEST( TMediumRecipeManager, GetParamsCount )
{
    TMediumRecipeManager mngr(TMediumRecipeManager::MT_CAUSTIC);
    mngr.GetParamsCount();
}

TEST( TMediumRecipeManager, getValue )
{
    TMediumRecipeManager mngr(TMediumRecipeManager::MT_CAUSTIC);
    auto valueNo = 0;
    mngr.getValue(valueNo);
}

TEST( TMediumRecipeManager, setValue)
{
    TMediumRecipeManager mngr(TMediumRecipeManager::MT_CAUSTIC);
    auto valueNo = 0;
    auto newValue = 0;
    mngr.setValue(valueNo, newValue);
}

TEST( TMediumRecipeManager, getCurrentRecipe )
{
    TMediumRecipeManager mngr(TMediumRecipeManager::MT_CAUSTIC);
    mngr.getCurrentRecipe();
}

TEST( TMediumRecipeManager, setCurrentRecipe )
{
    TMediumRecipeManager mngr(TMediumRecipeManager::MT_CAUSTIC);
    auto recipeNo = 0;
    mngr.setCurrentRecipe(recipeNo);
    recipeNo = 25;
    mngr.setCurrentRecipe(recipeNo);
}

TEST( TRecipeManager, LoadFromFile )
    {
    auto lineNo = 1;
    TRecipeManager mngr( lineNo );

    auto recipeNo = 0;
    mngr.ResetRecipeToDefaults( recipeNo );
    recipeNo = -1;
    mngr.ResetRecipeToDefaults(recipeNo);
    auto fileName = "tmp2.txt";
    EXPECT_EQ( 0, mngr.SaveToFile( fileName ) );
    EXPECT_EQ( 0, mngr.LoadFromFile( fileName ) );
    }

TEST( TRecipeManager, CopyRecipe )
    {
    auto lineNo = 1;
    TRecipeManager mngr( lineNo );
    mngr.CopyRecipe();
    }

TEST( TRecipeManager, PasteRecipe )
    {
    auto lineNo = 1;
    TRecipeManager mngr( lineNo );
    mngr.PasteRecipe();
    }

TEST( TRecipeManager, NullifyRecipe )
    {
    auto lineNo = 1;
    TRecipeManager mngr( lineNo );
    mngr.NullifyRecipe();
    }

TEST( TRecipeManager, EvalRecipe )
    {
    auto lineNo = 1;
    TRecipeManager mngr(lineNo);
    mngr.EvalRecipe();
    }


TEST( TRecipeManager, OffRecipeDevices )
    {
    auto lineNo = 1;
    TRecipeManager mngr(lineNo);
    auto recipeNo = 0;
    auto msaline = 1;
    mngr.OffRecipeDevices(recipeNo, msaline);
    EXPECT_EQ(0, mngr.OffRecipeDevices(recipeNo, msaline));
    }

TEST( TRecipeManager, NextRecipe )
    {
    auto lineNo = 1;
    TRecipeManager mngr(lineNo);
    auto currentRecipe = 0;
    mngr.NextRecipe();
    currentRecipe = 24;
    mngr.NextRecipe();
    }

TEST( TRecipeManager, PrevRecipe )
    {
    auto lineNo = 1;
    TRecipeManager mngr(lineNo);
    auto currentRecipe = 1;
    mngr.PrevRecipe();
    currentRecipe = 0;
    mngr.PrevRecipe();
    }

TEST( TRecipeManager, setCurrentRecipe )
    {
    auto lineNo = 1;
    TRecipeManager mngr(lineNo);
    auto recipeNo = 0;
    mngr.setCurrentRecipe(recipeNo);
    recipeNo = -1;
    mngr.setCurrentRecipe(recipeNo);
    }

TEST( TRecipeManager, OnRecipeDevices ) 
    {
    auto lineNo = 1;
    TRecipeManager mngr(lineNo);
    auto recipeNo = 0;
    auto msaline = 1;
    mngr.OnRecipeDevices( recipeNo, msaline );
    }

TEST( TRecipeManager, ToRecipe )
{
    auto lineNo = 1;
    TRecipeManager mngr(lineNo);
    auto recNo = 1;
    EXPECT_EQ(1, mngr.ToRecipe(recNo));
    recNo = 25;
    EXPECT_EQ(0, mngr.ToRecipe(recNo));
}

TEST( ParentRecipeManager, CopyRecipe )
    {
    auto lineNo = 1;
    ParentRecipeManager mngr(lineNo);
    mngr.CopyRecipe();
    }

TEST( ParentRecipeManager, PasteRecipe ) 
    {
    auto lineNo = 1;
    ParentRecipeManager mngr(lineNo);
    mngr.PasteRecipe();
    }

TEST( ParentRecipeManager, NullifyRecipe )
    {
    auto lineNo = 1;
    ParentRecipeManager mngr(lineNo);
    mngr.NullifyRecipe();
    }

TEST( ParentRecipeManager, SaveRecipeName )
    {
    auto lineNo = 1;
    ParentRecipeManager mngr(lineNo);
    mngr.SaveRecipeName();
    }

TEST( ParentRecipeManager, SaveToFile )
    {
    auto lineNo = 1;
    ParentRecipeManager mngr(lineNo);
    auto fileName = "tmp3.txt";
    EXPECT_EQ(0, mngr.SaveToFile(fileName));
    }

TEST(ParentRecipeManager, getRecipeValue)
    {
    auto lineNo = 1;
    ParentRecipeManager mngr(lineNo);
    auto recNo = 0;
    auto valueNo = 0;
    auto newValue = 0;
    mngr.getRecipeValue(recNo, valueNo);
    mngr.setRecipeValue(recNo, valueNo, newValue);
    mngr.setValue(valueNo, newValue);
    }

