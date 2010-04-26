#ifndef PRJ_MANAGER_W750_H
#define PRJ_MANAGER_W750_H

#include "prj_mngr.h"
#include "sys_w750.h"

class project_manager_w750 : public project_manager
    {
    public:
       project_manager_w750()
           {
           cfg_file = new file_w750();
           }
    };
#endif // PRJ_MANAGER_W750_H
