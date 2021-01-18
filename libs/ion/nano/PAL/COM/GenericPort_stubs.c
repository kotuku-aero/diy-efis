//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_COM.h>
#include "../Include/nanoPAL_COM.h"

bool GenericPort_Initialize(int comPortNum)
{
    (void)comPortNum;

    NATIVE_PROFILE_PAL_COM();
    return true;
}

bool GenericPort_Uninitialize(int comPortNum)
{
    (void)comPortNum;

    NATIVE_PROFILE_PAL_COM();
    return true;
}

uint32_t GenericPort_Write(int comPortNum, const char* data, size_t size)
{
    (void)comPortNum;
    (void)data;
    (void)size;

    NATIVE_PROFILE_PAL_COM();
    return 0;
}

int GenericPort_Read(int comPortNum, char* data, size_t size)
{
    (void)comPortNum;
    (void)data;
    (void)size;

    NATIVE_PROFILE_PAL_COM();
    return 0;
}

bool GenericPort_Flush(int comPortNum)
{
    (void)comPortNum;

    NATIVE_PROFILE_PAL_COM();
    return true;
}

bool GenericPort_IsSslSupported(int comPortNum)
{
    (void)comPortNum;

    NATIVE_PROFILE_PAL_COM();
    return false;
}

bool GenericPort_UpgradeToSsl(int comPortNum, unsigned int flags) 
{
    (void)comPortNum;
    (void)flags;

    NATIVE_PROFILE_PAL_COM();
    return false; 
}

bool GenericPort_IsUsingSsl(int comPortNum)
{
    (void)comPortNum;

    NATIVE_PROFILE_PAL_COM();
    return false;
}
