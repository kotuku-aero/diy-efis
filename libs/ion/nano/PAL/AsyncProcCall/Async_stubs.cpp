//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "../Include/nanoPAL.h"

//--//

// continuation list
void HAL_CONTINUATION::InitializeList()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
}

void HAL_CONTINUATION::Uninitialize()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
}

void HAL_CONTINUATION::InitializeCallback( HAL_CALLBACK_FPN entryPoint, void* argument )
{
    (void)entryPoint;
    (void)argument;

    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
}

void HAL_CONTINUATION::Enqueue()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
}

void HAL_CONTINUATION::Abort  ()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
}

bool HAL_CONTINUATION::Dequeue_And_Execute()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
    return TRUE;
}

bool HAL_CONTINUATION::IsLinked()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
    return false;
}

//-//
// completion list

void HAL_COMPLETION::InitializeList()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
}

void HAL_COMPLETION::Uninitialize()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
}

void HAL_COMPLETION::EnqueueTicks( uint64_t eventTimeTicks)
{
    (void)eventTimeTicks;

    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
}

void HAL_COMPLETION::EnqueueDelta( uint32_t uSecFromNow )
{
    (void)uSecFromNow;

    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
}

void HAL_COMPLETION::Abort()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
}

void HAL_COMPLETION::Execute()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
}

void HAL_COMPLETION::DequeueAndExec()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
}

void HAL_COMPLETION::WaitForInterrupts( uint64_t expireTimeInSysTicks, uint32_t sleepLevel, uint64_t wakeEvents )
{
    (void)expireTimeInSysTicks;
    (void)sleepLevel;
    (void)wakeEvents;

    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
}
