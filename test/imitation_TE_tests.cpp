#include "imitation_TE_tests.h"

using namespace ::testing;

TEST(imitation_TE, get_TE) 
	{
	imitation_TE obj;
	float test_variable = obj.get_TE();
	auto max_variable =obj.get_max();
	EXPECT_LE(max_variable,30);
	EXPECT_GE(test_variable, 20);
	}
