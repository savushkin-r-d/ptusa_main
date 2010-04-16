//-----------------------------------------------------------------------------
/// @brief 
//
/// 
struct wago_table 
    {
    int     state;
    u_long  id;

    // Digital outputs (DO).
    int DO_cnt;  ///< Amount of DO.
    u_char *DO;  ///< Current values.
    u_char *_DO; ///< To write.

    // Analog outputs (AO).
    int AO_cnt;  ///< Amount of AO.
    float *AO;   ///< Current values.
    float *_AO;  ///< To write.

    // Digital inputs (DI).
    int DI_cnt;  ///< Amount of DI.
    u_char *DI;  ///< Current values.

    // Analog inputs (AI).
    int AI_cnt;  ///< Amount of AI.
    float *AI;   ///< Current values.
    };
//-----------------------------------------------------------------------------
/// @brief 
//
/// 
class wago_manager
    {
    public:
        int read_inputs() = 0;
        int write_outputs() = 0;

        int load_from_stream( char *stream ) = 0;

        wago_manager* get_instance();

    private:
        wago_table **tables;

        int get_DO( u_int table_n, u_int offset );
        int set_DO( u_int table_n, u_int offset, char value );

        int get_DI( u_int table_n, u_int offset );
        int set_DI( u_int table_n, u_int offset, char value );

        float get_AO( u_int table_n, u_int offset );
        int   set_AO( u_int table_n, u_int offset, float value );

        float get_AI( u_int table_n, u_int offset );
        int   set_AI( u_int table_n, u_int offset, float value );
    };
//-----------------------------------------------------------------------------