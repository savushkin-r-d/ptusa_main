#include "imitation_TE_tests.h"

using namespace ::testing;

TEST(imitation_TE, get_TE) 
	{
	imitation_TE obj;
	float test_variable = obj.get_TE();
	EXPECT_GE(test_variable, 20);
	EXPECT_LE(test_variable, 35);
	}

TEST(imitation_TE,imitation_TE) 
	{
	imitation_TE obj1(0.34f, 26.f);
	float st_d = obj1.get_st_deviation();
	float ex_st_d = sqrt(0.34f);
	EXPECT_EQ(st_d, ex_st_d);
	}

TEST(imitation_TE, set_max)
	{
	imitation_TE obj2;
	obj2.set_max(35);
	float ex_max = obj2.get_max();
	EXPECT_EQ(ex_max, 35);
	}

TEST(imitation_TE, set_min) 
	{
	imitation_TE obj3;
	obj3.set_min(0);
	float ex_min = obj3.get_min();
	EXPECT_EQ(ex_min, 0);
	}
	


