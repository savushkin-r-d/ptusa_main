#pragma once

#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/types_generated.h>

#include "PAC_dev.h"
#include "tech_def.h"

class OPCUA_server {
 public:
  static OPCUA_server& get_instance();

  void init(short int port = 4841);

  void create_dev_objects();

  void create_PAC_info();

  UA_StatusCode start();

  void evaluate();

  void shutdown();

  UA_StatusCode init_all_and_start() {
    init();
    create_dev_objects();
    create_PAC_info();

    return start();
  }

  static UA_StatusCode read_state(UA_Server* server, const UA_NodeId* sessionId,
                                  void* sessionContext, const UA_NodeId* nodeId,
                                  void* nodeContext, UA_Boolean sourceTimeStamp,
                                  const UA_NumericRange* range,
                                  UA_DataValue* dataValue);

  static UA_StatusCode write_state(UA_Server* server,
                                   const UA_NodeId* sessionId,
                                   void* sessionContext,
                                   const UA_NodeId* nodeId, void* nodeContext,
                                   const UA_NumericRange* range,
                                   const UA_DataValue* value);

  static UA_StatusCode read_value(UA_Server* server, const UA_NodeId* sessionId,
                                  void* sessionContext, const UA_NodeId* nodeId,
                                  void* nodeContext, UA_Boolean sourceTimeStamp,
                                  const UA_NumericRange* range,
                                  UA_DataValue* dataValue);

  static UA_StatusCode write_value(UA_Server* server,
                                   const UA_NodeId* sessionId,
                                   void* sessionContext,
                                   const UA_NodeId* nodeId, void* nodeContext,
                                   const UA_NumericRange* range,
                                   const UA_DataValue* value);

  static UA_StatusCode read_PAC_info_value(UA_Server*, const UA_NodeId*, void*,
                                           const UA_NodeId*, void* nodeContext,
                                           UA_Boolean, const UA_NumericRange*,
                                           UA_DataValue* dataValue);

  ~OPCUA_server();

  // Explicitly delete the copy constructors.
  OPCUA_server(OPCUA_server const&) = delete;
  OPCUA_server(OPCUA_server&&) = delete;
  OPCUA_server& operator=(OPCUA_server const&) = delete;
  OPCUA_server& operator=(OPCUA_server&&) = delete;

  UA_Server* get_server() const;

 private:
  OPCUA_server() = default;

  UA_Server* server = nullptr;

  bool is_dev_objects_created = false;
  bool is_PAC_info_created = false;
};

#define G_OPCUA_SERVER OPCUA_server::get_instance()
