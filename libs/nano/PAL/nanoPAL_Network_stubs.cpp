//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_COM.h>

bool  Network_Initialize()
{ 
    return true;
};

bool  Network_Uninitialize()
{
    return true;
};

void SOCKETS_CloseConnections()
{
    NATIVE_PROFILE_PAL_COM();
}
