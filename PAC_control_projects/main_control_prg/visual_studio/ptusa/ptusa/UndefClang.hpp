#pragma once
//this file is only used to undefine __clang__ macro for intellisense
//__clang__ is a predefined macro in visual studio for intellisense and currently found no other way to disable the macro
#if defined(__INTELLISENSE__) && defined(__clang__)
	#undef __clang__
#endif