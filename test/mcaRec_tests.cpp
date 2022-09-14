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
