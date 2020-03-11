#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

const char *HOST = STRINGIZE_VALUE_OF(TA_HOST);
const char *PORT = STRINGIZE_VALUE_OF(TA_PORT);
const char *API = STRINGIZE_VALUE_OF(TA_API);

#include "use_models.h"

int main(void) { return runner(HOST, PORT, API); }
