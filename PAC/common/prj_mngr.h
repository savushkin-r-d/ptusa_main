#ifndef PRJ_MANAGER_H
#define PRJ_MANAGER_H

#include "sys.h"

class project_manager
    {
    public:
        int load_configuration( char *file_name );

        static project_manager* get_instance();

        static void set_instance( project_manager* new_instance );

    protected:
        virtual int config_file_open( char *file_name ) = 0;
        virtual int config_file_read( void *buffer, int count ) = 0;
        virtual int config_file_close() = 0;

    private:
        static project_manager* instance;
    };

#endif // PRJ_MANAGER_H