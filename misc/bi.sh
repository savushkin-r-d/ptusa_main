( cmake -P ./misc/bi.cmake ) 2>&1 | awk -v IGNORECASE=1 '
    /( failed)|( not found)|( warning)|( could not find)/   {print "\033[33m" $0 "\033[39m"; next}
    /( success)|( found)|( done)/                           {print "\033[32m" $0 "\033[39m"; next}
    / debug/                                                {print "\033[37m" $0 "\033[39m"; next}
    1 {print "\033[37m" $0 "\033[39m"}
    '
