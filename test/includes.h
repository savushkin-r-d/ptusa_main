#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <subhook.h>
#include <vector>
#include <limits.h>

unsigned long get_delta_millisec_as_1001( unsigned long time1 );

unsigned long get_delta_millisec_as_10001( unsigned long time1 );

unsigned long get_delta_millisec( unsigned long time1 );

const auto G_RM_DELTA_MILLISEC_HOOK_10001 = subhook_new(
    reinterpret_cast<void*>(get_delta_millisec),
    reinterpret_cast<void*>(get_delta_millisec_as_10001),
    SUBHOOK_64BIT_OFFSET);

const auto G_GET_DELTA_MILLISEC_HOOK_1001 = subhook_new(
    reinterpret_cast<void*>(get_delta_millisec),
    reinterpret_cast<void*>(get_delta_millisec_as_1001),
    SUBHOOK_64BIT_OFFSET);
