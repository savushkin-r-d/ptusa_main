#include "params_recipe_manager_tests.h"

using namespace ::testing;

class ParamsRecipeStorageTest : public testing::Test {
    protected:
        // define a pointer to the RecipeManager object
        ParamsRecipeStorage* rm{};

        // define a method to set up the test fixture
        void SetUp() override {
            // create a RecipeManager object with 3 recipes and 4 params per recipe
            rm = new ParamsRecipeStorage(1, 3, 5);
            }

        // define a method to tear down the test fixture
        void TearDown() override {
            // delete the RecipeManager object
            delete rm;
            }
    };

// define a test case for serializing the recipes to a file
TEST_F(ParamsRecipeStorageTest, Serialize) {
    // create a vector of params for the recipe
    std::vector<float> params = {1.0, 2.0, 3.0, 4.0, 5.0};
    rm->recipes[0].name = "Test1";
    rm->recipes[0].params = params;
    // serialize the recipes to a file named "test.txt"
    rm->serialize("test.txt");
    // create an input file stream with the file name
    std::ifstream infile("test.txt");
    // check if the file is opened successfully
    ASSERT_TRUE(infile.is_open());
    // create variables to store the values from the file
    int rc, pc;
    std::string name;
    float p1, p2, p3, p4;
    // read the values from the file
    infile >> rc >> pc;
    infile.ignore();
    std::getline(infile, name);
    infile >> p1 >> p2 >> p3 >> p4;
    // close the file stream
    infile.close();
    // check if the values from the file match with the expected values
    EXPECT_EQ(rc, 3);
    EXPECT_EQ(pc, 5);
    EXPECT_EQ(name, "Test1");
    EXPECT_EQ(p1, 1.0);
    EXPECT_EQ(p2, 2.0);
    EXPECT_EQ(p3, 3.0);
    EXPECT_EQ(p4, 4.0);
    }

// define a test case for deserializing the recipes from a file
TEST_F(ParamsRecipeStorageTest, Deserialize) {
    // deserialize the recipes from a file named "test.txt"
    rm->deserialize("test.txt");
    // check if the name of the recipe at index 0 is equal to "Test1"
    EXPECT_EQ(rm->recipes[0].name, "Test1");
    // check if the params of the recipe at index 0 are equal to {1.0, 2.0, 3.0, 4.0, 5.0}
    EXPECT_EQ(rm->recipes[0].params[0], 1.0);
    EXPECT_EQ(rm->recipes[0].params[1], 2.0);
    EXPECT_EQ(rm->recipes[0].params[2], 3.0);
    EXPECT_EQ(rm->recipes[0].params[3], 4.0);
    EXPECT_EQ(rm->recipes[0].params[4], 5.0);
    }
