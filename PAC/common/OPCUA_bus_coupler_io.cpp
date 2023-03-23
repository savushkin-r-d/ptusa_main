#include "OPCUA_bus_coupler_io.h"
#include "log.h"

//-----------------------------------------------------------------------------
int io_manager_OPCUA::write_outputs()
    {
    if (0 == nodes_count) return 0;

    for (u_int i = 0; i < nodes_count; i++)
        {
        io_node* nd = nodes[i];

        if (!nd->is_active)
            {
            continue;
            }
        if (nd->AO_cnt > 0)
            {
            for (u_int i = 0; i < nd->AO_cnt; i++)
                {
                UA_Variant value;
                int_2 AOValue = nd->AO_[i];
                UA_Variant_setScalar(&value, &AOValue, &UA_TYPES[UA_TYPES_INT16]);
                std::string str = "AO " + std::to_string(i);
                UA_NodeId currentNodeId = UA_NODEID_STRING(0, (char*)str.c_str());

                UA_StatusCode st = UA_Server_writeValue(OPCUAServer::getInstance().getServer(), currentNodeId, value);

                memcpy(&(nd->AO[i]), &(nd->AO_[i]), 2);
                }
            }

        if (nd->DO_cnt > 0)
            {
            for (u_int i = 0; i < nd->DO_cnt; i++)
                {
                UA_Variant value;
                u_char DOValue = nd->DO_[i];
                UA_Variant_setScalar(&value, &DOValue, &UA_TYPES[UA_TYPES_INT16]);
                std::string str = "DO " + std::to_string(i);
                UA_NodeId currentNodeId = UA_NODEID_STRING(0, (char*)str.c_str());

                UA_StatusCode st = UA_Server_writeValue(OPCUAServer::getInstance().getServer(), currentNodeId, value);

                memcpy(&(nd->DO[i]), &(nd->DO_[i]), 1);
                }
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int io_manager_OPCUA::read_inputs()
    {
    if (0 == nodes_count) return 0;

    for (u_int i = 0; i < nodes_count; i++)
        {
        io_node* nd = nodes[i];

        if (!nd->is_active)
            {
            continue;
            }

        if (nd->AI_cnt > 0)
            {
            for (u_int i = 0; i < nd->AI_cnt; i++)
                {
                UA_Variant value;
                UA_Variant_init(&value);
                std::string str = "AI " + std::to_string(i);
                UA_NodeId currentNodeId = UA_NODEID_STRING(0, (char*)str.c_str());
                UA_StatusCode st = UA_Server_readValue(OPCUAServer::getInstance().getServer(), currentNodeId, &value);
                nd->AI[i] = *(int_2*)value.data;
                }
            }

        if (nd->DI_cnt > 0)
            {
            for (u_int i = 0; i < nd->DI_cnt; i++)
                {
                UA_Variant value;
                UA_Variant_init(&value);
                std::string str = "DI " + std::to_string(i);
                UA_NodeId currentNodeId = UA_NODEID_STRING(0, (char*)str.c_str());
                UA_StatusCode st = UA_Server_readValue(OPCUAServer::getInstance().getServer(), currentNodeId, &value);
                nd->DI[i] = *(u_char*)value.data;
                }
            }
        }
            
    return 0;
    }
//-----------------------------------------------------------------------------
io_manager_OPCUA::io_manager_OPCUA()
{
}
//-----------------------------------------------------------------------------
io_manager_OPCUA::~io_manager_OPCUA()
{
}
//-----------------------------------------------------------------------------
