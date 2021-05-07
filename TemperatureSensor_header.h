#ifndef STEAMENGINE_H_
#define STEAMENGINE_H_

#include "open62541.h"

#ifdef __cplusplus
extern "C" {
#endif

void turnOnOffHeating(UA_Server* server);

/* defines all sensor types */
void defineTemperatureSensorType(UA_Server* server);

/* add all instances of each sensor */
void addTemperatureSensorInstance(UA_Server* server, int nsIdent, int numIdent, char* name);

/* CTORS */
UA_StatusCode temperatureSensorTypeConstructor(UA_Server* server, 
                    const UA_NodeId *sessionId, void *sessionContext,
                    const UA_NodeId *typeId, void *typeContext,
                    const UA_NodeId *nodeId, void **nodeContext);

/* add all CTORS to the server */
void addTemperatureTypeConstructor(UA_Server* server);

/* Methods for connecting the serverside sensor variables with the physical sensor values. This is done with the Value Callback Method */
void addValueCallbackToCurrentTemp1Variable(UA_Server *server);


#ifdef __cplusplus
} // extern "C"
#endif

#endif /* STEAMENGINE_H_ */



