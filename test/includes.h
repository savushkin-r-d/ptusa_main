#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <subhook.h>
#include <vector>
#include <limits.h>

uint64_t subhook_for_get_delta_millisec( uint64_t );

uint64_t get_delta_millisec( uint64_t time1 );

extern const subhook_t  GTESTS_DELTA_MILLISEC_SUBHOOK;

class DeltaMilliSecSubHooker
    {
    inline static unsigned long millitime;

    public:

        static unsigned long get_millisec()
            {
            return millitime;
            }

        static void set_millisec( unsigned long millisec )
            {
            millitime = millisec;
            subhook_install( GTESTS_DELTA_MILLISEC_SUBHOOK );
            }

        static void set_default_time()
            {
            millitime = 1UL;
            subhook_remove( GTESTS_DELTA_MILLISEC_SUBHOOK );
            }

        explicit DeltaMilliSecSubHooker( unsigned long millisec )
            {
            DeltaMilliSecSubHooker::set_millisec( millisec );
            }

        ~DeltaMilliSecSubHooker()
            {
            DeltaMilliSecSubHooker::set_default_time();
            }

        DeltaMilliSecSubHooker& operator=( DeltaMilliSecSubHooker&& ) = delete;
    };

#ifdef LINUX_OS
    # define ANSI_COLOR_RED     "\x1b[31m"
    # define ANSI_COLOR_GREEN   "\x1b[32m"
    # define ANSI_COLOR_YELLOW  "\x1b[33m"
    # define ANSI_COLOR_BLUE    "\x1b[34m"
    # define ANSI_COLOR_MAGENTA "\x1b[35m"
    # define ANSI_COLOR_CYAN    "\x1b[36m"
    # define ANSI_COLOR_RESET   "\x1b[0m"
#else
    # define ANSI_COLOR_RED     ""
    # define ANSI_COLOR_GREEN   ""
    # define ANSI_COLOR_YELLOW  ""
    # define ANSI_COLOR_BLUE    ""
    # define ANSI_COLOR_MAGENTA ""
    # define ANSI_COLOR_CYAN    ""
    # define ANSI_COLOR_RESET   ""
#endif
