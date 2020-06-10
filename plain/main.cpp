#include <iostream>
using namespace std;
#include"modbus.h"
#include"open62541.h"
//addcommit
#include<iostream>
#include<cmath>
using namespace std;
int a=0;

static void update_modbus_value(UA_Server *server)
{
        UA_Variant value;
        UA_Variant_setScalar(&value,&a,&UA_TYPES[UA_TYPES_INT32]);
        UA_NodeId myIegerNodeId =UA_NODEID_STRING(1,"the.answer");
        UA_Server_writeValue(server,myIegerNodeId,value);
}
static void update_modbus_read_value(UA_Server *server)
{
        UA_Variant read_value;
        UA_NodeId myIegerNodeId=UA_NODEID_STRING(1,"the.answer");
        int check=UA_Server_readValue(server,myIegerNodeId,&read_value);
        if(check==0)
        {
            cout<<"read_value success"<<endl;
        }
        else
        {
            cout<<"read_value failed"<<endl; //读取不成功！
        }
        int *k=NULL;
        k=(int*)read_value.data;
        cout<<(*k)<<endl;
}

static void addVariable(UA_Server *server)
{
    /* Define the attribute of the myInteger variable node */
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    UA_Variant_setScalar(&attr.value, &a, &UA_TYPES[UA_TYPES_INT32]);
    attr.description = UA_LOCALIZEDTEXT("en-US","the answer");
    attr.displayName = UA_LOCALIZEDTEXT("en-US","the answer");
    attr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    /* Add the variable node to the information model */
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, "the.answer");
    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, "the answer");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, myIntegerNodeId, parentNodeId,
                              parentReferenceNodeId, myIntegerName,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);
}
static void beforereadmodbus(UA_Server *server,
                             const UA_NodeId *sessionId,
                             void *sessionContext,
                             const UA_NodeId *nodeId,
                             void *nodeContext,
                             const UA_NumericRange *range,
                             const UA_DataValue *data){
        update_modbus_value(server);
}
static void afterreadmodbus(UA_Server *server,
                            const UA_NodeId *sessionId,
                            void *sessionContext,
                            const UA_NodeId *nodeId,
                            void *nodeContext,
                            const UA_NumericRange *range,
                            const UA_DataValue *data){
    update_modbus_read_value(server);
//    UA_LOG_INFO(UA_Log_Stdout,UA_LOGCATEGORY_USERLAND,
//                "tHE VARIABLE WAS UPDATED");
}
static void addmodbusvaluebacktoaddvariable(UA_Server *server)
{
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, "the.answer");
    UA_ValueCallback callback ;

    callback.onRead=beforereadmodbus;
    callback.onWrite=afterreadmodbus;
    UA_Server_setVariableNode_valueCallback(server,myIntegerNodeId,callback);
}
static volatile UA_Boolean running = true;
static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

int main(void) {
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    UA_Server *server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));
    addVariable(server);
   addmodbusvaluebacktoaddvariable(server);
    UA_StatusCode retval = UA_Server_run(server, &running);

    UA_Server_delete(server);
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
