$#include "rfid_reader.h"

//-----------------------------------------------------------------------------
struct EPC_info
    {
    char* EPC_str;
    int  antenna;
    int RSSI;
    };

class rfid_reader
    {
    public:

        enum CONSTANTS
            {
            ST_ACTIVE = 0,
            ST_CANT_CONNECT = -1,
            };

        static rfid_reader* add_reader( const char* ip_address, int number );

        int evaluate();

        const EPC_info* get_EPC_info( int idx ) const;
        int get_EPC_cnt() const;
    };
