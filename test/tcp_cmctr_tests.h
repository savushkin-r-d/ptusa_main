#pragma once
#include "includes.h"

#include "tcp_cmctr.h"
#include "tcp_client.h"
#include "dtime.h"

#ifdef LINUX_OS
#include "l_tcp_client.h"
#else
#include "w_tcp_client.h"
#endif
