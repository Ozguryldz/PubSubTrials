#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>

#include <stdlib.h>



static void
variables_nodeids(void) {
    UA_NodeId id1 = UA_NODEID_NUMERIC(1, 1234);
    id1.namespaceIndex = 3;

    printf("                 %d                  ",id1.identifier.numeric);

 
    UA_NodeId_clear(&id1);
}


int main(void) {

    variables_nodeids();

    return EXIT_SUCCESS;
}
