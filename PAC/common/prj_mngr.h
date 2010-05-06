#ifndef PRJ_MANAGER_H
#define PRJ_MANAGER_H

#include "sys.h"

class project_manager
    {
    public:
        int load_configuration( const char *file_name );

        static project_manager* get_instance();

        static void set_instance( project_manager* new_instance );

    protected:
        file *cfg_file;

    private:
        static project_manager* instance;
    };

#endif // PRJ_MANAGER_H

