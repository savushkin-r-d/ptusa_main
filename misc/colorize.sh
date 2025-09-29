#!/bin/sh
awk -v IGNORECASE=1 '
    /( failed)|( not found)|( warning)|( could not find)/   {print "\033[33m" $0 "\033[39m"; next} # Yellow
    /( found)|( find)/ && /(no )/                           {print "\033[33m" $0 "\033[39m"; next} # Yellow
    /( success)|( found)|( done)/ && !/(no )|(nothing )/    {print "\033[32m" $0 "\033[39m"; next} # Green
    / debug/                                                {print "\033[37m" $0 "\033[39m"; next} # Grey
    1 {print $0}
    '
