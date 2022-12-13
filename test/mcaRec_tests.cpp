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


TEST( TRecipeManager, LoadFromFile )
    {
    auto lineNo = 1;
    TRecipeManager mngr( lineNo );

    auto recipeNo = 0;
    mngr.ResetRecipeToDefaults( recipeNo );
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

TEST(ParentRecipeManager, CopyRecipe)
    {
    auto lineNo = 0;
    ParentRecipeManager mngr(lineNo);
    mngr.CopyRecipe();
    }