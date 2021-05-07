#include <signal.h>
#include "open62541.h"
#include "SteamEngine.h"
#include "MCP3008Driver.h"
#include <stdio.h>

#define NODEID_PT100_BOILER 2000
#define NODEID_PT100_HEATER 2001
#define NODEID_DHT11_ENV    2002

/* predefined identifier for later use */
// UA_NodeId = {namespaceindex, identifiertype, identifier}
UA_NodeId temperatureSensorTypeId = {1, UA_NODEIDTYPE_NUMERIC, {1001}};

void defineTemperatureSensorType(UA_Server* server){

    /* Define the object type for "TemperatureSensor" */
    UA_ObjectTypeAttributes tstAttr = UA_ObjectTypeAttributes_default;
    tstAttr.displayName = UA_LOCALIZEDTEXT("en-US", "TemperatureSensorType");
    UA_Server_addObjectTypeNode(server, UA_NODEID_NULL,
                                UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
                                UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
                                UA_QUALIFIEDNAME(1, "TemperatureSensorType"), tstAttr,
                                NULL, &temperatureSensorTypeId);

    /* Define the attributes held by the variables */
	/* Sensor name */
    UA_String str = UA_STRING("PT100");
    UA_VariableAttributes sensorNameAttr = UA_VariableAttributes_default;
    sensorNameAttr.displayName = UA_LOCALIZEDTEXT("en-US", "SensorName");
    sensorNameAttr.accessLevel = UA_ACCESSLEVELMASK_READ;
    sensorNameAttr.dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    UA_Variant_setScalar(&sensorNameAttr.value, &str, &UA_TYPES[UA_TYPES_STRING]);

    UA_NodeId sensorNameId;
	// temperatureSensorTypeId is the parent nodeid here!!
    UA_Server_addVariableNode(server, UA_NODEID_NULL, temperatureSensorTypeId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "SensorName"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), sensorNameAttr, NULL, &sensorNameId);
    
    /* Make the sensor name mandatory */
    UA_Server_addReference(server, sensorNameId,
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

	/* Location of the sensor */
    str = UA_STRING("Heater");
    UA_VariableAttributes locationAttr = UA_VariableAttributes_default;
    locationAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Location");
    locationAttr.accessLevel = UA_ACCESSLEVELMASK_READ;
    locationAttr.dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    UA_Variant_setScalar(&locationAttr.value, &str, &UA_TYPES[UA_TYPES_STRING]);

    UA_NodeId sensorLocationId;
	// temperatureSensorTypeId is the parent nodeid here!!
    UA_Server_addVariableNode(server, UA_NODEID_NULL, temperatureSensorTypeId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "Location"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), locationAttr, NULL, &sensorLocationId);

    /* Make the sensor location mandatory */
    UA_Server_addReference(server, sensorLocationId,
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);


	/* Current temperature value of the sensor */
    UA_Double temp = 47.11;    // default temperature value for when the node is created
    UA_VariableAttributes tempValAttr = UA_VariableAttributes_default;
    UA_Variant_setScalar(&tempValAttr.value, &temp, &UA_TYPES[UA_TYPES_DOUBLE]);
    tempValAttr.description = UA_LOCALIZEDTEXT("en-US", "Temperature");
    tempValAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Temperature");
    tempValAttr.dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;
    tempValAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    /* Add the variable to the information model */
    UA_NodeId tmpValId = UA_NODEID_STRING(1, "temp.value");
	// temperatureSensorTypeId is the parent nodeid here!!
    UA_Server_addVariableNode(server, tmpValId, temperatureSensorTypeId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "temp.value"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), tempValAttr, NULL, &tmpValId);


    /* Make the sensor location mandatory */
    UA_Server_addReference(server, tmpValId,
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

}

/* add all instances of each sensor */
void addTemperatureSensorInstance(UA_Server* server, int nsIdent, int numIdent, char* name){
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    oAttr.displayName = UA_LOCALIZEDTEXT("en-US", name);
	UA_NodeId nodeId = UA_NODEID_NUMERIC(nsIdent, numIdent); // by me
    UA_Server_addObjectNode(server, UA_NODEID_NULL,
                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                            UA_QUALIFIEDNAME(nsIdent, name),
                            temperatureSensorTypeId, /* this refers to the object type
                                           identifier */
                            oAttr, NULL, &nodeId);
}

/* CTORS */
UA_StatusCode temperatureSensorTypeConstructor(UA_Server* server, 
                    const UA_NodeId *sessionId, void *sessionContext,
                    const UA_NodeId *typeId, void *typeContext,
                    const UA_NodeId *nodeId, void **nodeContext){

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "New temperature sensor created");


    /* At this point we could replace the node context .. */

    return UA_STATUSCODE_GOOD;
}

/* add all CTORS to the server */
void addTemperatureTypeConstructor(UA_Server* server){

    UA_NodeTypeLifecycle lifecycle;
    lifecycle.constructor = temperatureSensorTypeConstructor;
    lifecycle.destructor = NULL;
    UA_Server_setNodeTypeLifecycle(server, temperatureSensorTypeId, lifecycle);
}

/* add variable value callback for changing the value of the server variable binded to the physical sensor value */
void beforeReadTemp1(UA_Server *server,
               const UA_NodeId *sessionId, void *sessionContext,
               const UA_NodeId *nodeid, void *nodeContext,
               const UA_NumericRange *range, const UA_DataValue *data) {
    UA_DateTime now = UA_DateTime_now();
    UA_Variant value;
    UA_Variant_setScalar(&value, &now, &UA_TYPES[UA_TYPES_DATETIME]);
    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "current-time");
    UA_Server_writeValue(server, currentNodeId, value);
}

void afterWriteTemp1(UA_Server *server,
               const UA_NodeId *sessionId, void *sessionContext,
               const UA_NodeId *nodeId, void *nodeContext,
               const UA_NumericRange *range, const UA_DataValue *data) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                "The variable was updated");
}

void addValueCallbackToCurrentTemp1Variable(UA_Server *server) {
    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "temp.value");
    UA_ValueCallback callback ;
    callback.onRead = beforeReadTemp1;
    callback.onWrite = afterWriteTemp1;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId, callback);
} 
