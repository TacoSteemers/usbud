#include <windows.h>
#include "logging.h"

char *getuid()
{
    return getenv("user");
}