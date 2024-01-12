#pragma once
#include "includes.h"
#include "tcp_cmctr.h"
#include "tcp_client.h"

class mock_tcp_communicator : public tcp_communicator
{
    public:
    MOCK_METHOD0(evaluate, int());
    MOCK_METHOD(srv_ptr, reg_service, (u_char srv_id, srv_ptr fk));  

    char* get_host_name_eng() { return host_name_eng; };

    //int evaluate() { return 0; };
};

class test_tcp_communicator
{
    public:
    static void replaceEntity(mock_tcp_communicator* p)
    {
        tcp_communicator::is_init = 1;
        tcp_communicator::instance = p;
    }

    static void removeObject()
    {
        tcp_communicator::instance = NULL;
    }
};