#include "../canflylib/CanFly_CoreLibrary.h"
#include "CanFly_CoreLibrary_CanFly_Neutron.h"

using namespace canflylib::CanFly_CoreLibrary;


HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CreateCanMsgInt16___STATIC__CanFlyCanFlyMsg__CanFlyCanFlyID__I2__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        int16_t param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT16( stack, 1, param1 ) );

        int8_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 2, param2 ) );

        UNSUPPORTED retValue = Neutron::CreateCanMsgInt16( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CreateCanMsgInt16_2___STATIC__CanFlyCanFlyMsg__CanFlyCanFlyID__I2__I2__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        int16_t param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT16( stack, 1, param1 ) );

        int16_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT16( stack, 2, param2 ) );

        int8_t param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 3, param3 ) );

        UNSUPPORTED retValue = Neutron::CreateCanMsgInt16_2( param0, param1, param2, param3, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CreateCanMsgUint16___STATIC__CanFlyCanFlyMsg__CanFlyCanFlyID__U2__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        uint16_t param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT16( stack, 1, param1 ) );

        int8_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 2, param2 ) );

        UNSUPPORTED retValue = Neutron::CreateCanMsgUint16( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CreateCanMsgUint16_2___STATIC__CanFlyCanFlyMsg__CanFlyCanFlyID__U2__U2__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        uint16_t param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT16( stack, 1, param1 ) );

        uint16_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT16( stack, 2, param2 ) );

        int8_t param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 3, param3 ) );

        UNSUPPORTED retValue = Neutron::CreateCanMsgUint16_2( param0, param1, param2, param3, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CreateCanMsgInt32___STATIC__CanFlyCanFlyMsg__CanFlyCanFlyID__I4__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        int8_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 2, param2 ) );

        UNSUPPORTED retValue = Neutron::CreateCanMsgInt32( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CreateCanMsgInt32_2___STATIC__CanFlyCanFlyMsg__CanFlyCanFlyID__I4__I4__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        signed int param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 2, param2 ) );

        int8_t param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 3, param3 ) );

        UNSUPPORTED retValue = Neutron::CreateCanMsgInt32_2( param0, param1, param2, param3, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CreateCanMsguint___STATIC__CanFlyCanFlyMsg__CanFlyCanFlyID__U4__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        unsigned int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 1, param1 ) );

        int8_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 2, param2 ) );

        UNSUPPORTED retValue = Neutron::CreateCanMsguint( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CreateCanMsguint_2___STATIC__CanFlyCanFlyMsg__CanFlyCanFlyID__U4__U4__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        unsigned int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 1, param1 ) );

        unsigned int param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 2, param2 ) );

        int8_t param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 3, param3 ) );

        UNSUPPORTED retValue = Neutron::CreateCanMsguint_2( param0, param1, param2, param3, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CreateCanMsgFloat___STATIC__CanFlyCanFlyMsg__CanFlyCanFlyID__R4__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        float param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_float( stack, 1, param1 ) );

        int8_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 2, param2 ) );

        UNSUPPORTED retValue = Neutron::CreateCanMsgFloat( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CreateCanMsgInt8___STATIC__CanFlyCanFlyMsg__CanFlyCanFlyID__U1__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        uint8_t param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 1, param1 ) );

        int8_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 2, param2 ) );

        UNSUPPORTED retValue = Neutron::CreateCanMsgInt8( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CreateCanMsgInt8_2___STATIC__CanFlyCanFlyMsg__CanFlyCanFlyID__U1__U1__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        uint8_t param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 1, param1 ) );

        uint8_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 2, param2 ) );

        int8_t param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 3, param3 ) );

        UNSUPPORTED retValue = Neutron::CreateCanMsgInt8_2( param0, param1, param2, param3, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CreateCanMsgInt8_3___STATIC__CanFlyCanFlyMsg__CanFlyCanFlyID__U1__U1__U1__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        uint8_t param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 1, param1 ) );

        uint8_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 2, param2 ) );

        uint8_t param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 3, param3 ) );

        int8_t param4;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 4, param4 ) );

        UNSUPPORTED retValue = Neutron::CreateCanMsgInt8_3( param0, param1, param2, param3, param4, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CreateCanMsgInt8_4___STATIC__CanFlyCanFlyMsg__CanFlyCanFlyID__U1__U1__U1__U1__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        uint8_t param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 1, param1 ) );

        uint8_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 2, param2 ) );

        uint8_t param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 3, param3 ) );

        uint8_t param4;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 4, param4 ) );

        int8_t param5;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 5, param5 ) );

        UNSUPPORTED retValue = Neutron::CreateCanMsgInt8_4( param0, param1, param2, param3, param4, param5, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CreateCanMsgUint8___STATIC__CanFlyCanFlyMsg__CanFlyCanFlyID__I1__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        int8_t param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 1, param1 ) );

        int8_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 2, param2 ) );

        UNSUPPORTED retValue = Neutron::CreateCanMsgUint8( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CreateCanMsgUint8_2___STATIC__CanFlyCanFlyMsg__CanFlyCanFlyID__I1__I1__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        int8_t param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 1, param1 ) );

        int8_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 2, param2 ) );

        int8_t param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 3, param3 ) );

        UNSUPPORTED retValue = Neutron::CreateCanMsgUint8_2( param0, param1, param2, param3, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CreateCanMsgUint8_3___STATIC__CanFlyCanFlyMsg__CanFlyCanFlyID__I1__I1__I1__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        int8_t param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 1, param1 ) );

        int8_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 2, param2 ) );

        int8_t param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 3, param3 ) );

        int8_t param4;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 4, param4 ) );

        UNSUPPORTED retValue = Neutron::CreateCanMsgUint8_3( param0, param1, param2, param3, param4, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CreateCanMsgUint8_4___STATIC__CanFlyCanFlyMsg__CanFlyCanFlyID__I1__I1__I1__I1__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        int8_t param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 1, param1 ) );

        int8_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 2, param2 ) );

        int8_t param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 3, param3 ) );

        int8_t param4;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 4, param4 ) );

        int8_t param5;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 5, param5 ) );

        UNSUPPORTED retValue = Neutron::CreateCanMsgUint8_4( param0, param1, param2, param3, param4, param5, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::GetParamFloat___STATIC__R4__CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        float retValue = Neutron::GetParamFloat( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_float( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::GetParamInt8___STATIC__U1__CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        uint8_t retValue = Neutron::GetParamInt8( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT8( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::GetParamInt8Array___STATIC__SZARRAY_U1__CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        CLR_RT_TypedArray_UINT8 retValue = Neutron::GetParamInt8Array( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT8_ARRAY( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::GetParamUint8___STATIC__I1__CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        int8_t retValue = Neutron::GetParamUint8( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_INT8( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::GetParamUint8Array___STATIC__SZARRAY_I1__CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        CLR_RT_TypedArray_INT8 retValue = Neutron::GetParamUint8Array( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_INT8_ARRAY( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::GetParamInt16___STATIC__I2__CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        int16_t retValue = Neutron::GetParamInt16( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_INT16( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::GetParamInt16Array___STATIC__SZARRAY_I2__CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        CLR_RT_TypedArray_INT16 retValue = Neutron::GetParamInt16Array( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_INT16_ARRAY( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::GetParamUint16___STATIC__U2__CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        uint16_t retValue = Neutron::GetParamUint16( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT16( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::GetParamUint16Array___STATIC__SZARRAY_U2__CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        CLR_RT_TypedArray_UINT16 retValue = Neutron::GetParamUint16Array( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT16_ARRAY( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::GetParamInt32___STATIC__I4__CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        signed int retValue = Neutron::GetParamInt32( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_INT32( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::GetParamuint___STATIC__U4__CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        unsigned int retValue = Neutron::GetParamuint( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT32( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CanSend___STATIC__VOID__CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        Neutron::CanSend( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CanSendRaw___STATIC__VOID__CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        Neutron::CanSendRaw( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::CanSendRepy___STATIC__VOID__CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        Neutron::CanSendRepy( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegisterService___STATIC__VOID__I1__CanFlyMsgHookFn( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        int8_t param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        Neutron::RegisterService( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::Subscribe___STATIC__U4__CanFlyMsgHookFn__SZARRAY_CanFlyCanFlyID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        CLR_RT_TypedArray_UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED_ARRAY( stack, 1, param1 ) );

        unsigned int retValue = Neutron::Subscribe( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT32( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::UnSubscribe___STATIC__VOID__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        Neutron::UnSubscribe( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::SemaphoreCreate___STATIC__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED retValue = Neutron::SemaphoreCreate(  hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::SemaphoreClose___STATIC__VOID__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        Neutron::SemaphoreClose( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::SemaphoreSignal___STATIC__VOID__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        Neutron::SemaphoreSignal( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::SemaphoreWait___STATIC__VOID__CanFlyHandle__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        unsigned int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 1, param1 ) );

        Neutron::SemaphoreWait( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegCreateKey___STATIC__U4__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        unsigned int retValue = Neutron::RegCreateKey( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT32( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegOpenKey___STATIC__U4__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        unsigned int retValue = Neutron::RegOpenKey( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT32( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegEnumKey___STATIC__VOID__U4__BYREF_CanFlyFieldType__BYREF_U2__BYREF_SZARRAY_U1__BYREF_STRING__BYREF_U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

         param1;
        UINT8 heapblock1[CLR_RT_HEAP_BLOCK_SIZE];
        NANOCLR_CHECK_HRESULT( Interop_Marshal__ByRef( stack, heapblock1, param1 ) );

         param2;
        UINT8 heapblock2[CLR_RT_HEAP_BLOCK_SIZE];
        NANOCLR_CHECK_HRESULT( Interop_Marshal__ByRef( stack, heapblock2, param2 ) );

         param3;
        UINT8 heapblock3[CLR_RT_HEAP_BLOCK_SIZE];
        NANOCLR_CHECK_HRESULT( Interop_Marshal__ByRef( stack, heapblock3, param3 ) );

         param4;
        UINT8 heapblock4[CLR_RT_HEAP_BLOCK_SIZE];
        NANOCLR_CHECK_HRESULT( Interop_Marshal__ByRef( stack, heapblock4, param4 ) );

         param5;
        UINT8 heapblock5[CLR_RT_HEAP_BLOCK_SIZE];
        NANOCLR_CHECK_HRESULT( Interop_Marshal__ByRef( stack, heapblock5, param5 ) );

        Neutron::RegEnumKey( param0, param1, param2, param3, param4, param5, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegDeleteKey___STATIC__VOID__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        Neutron::RegDeleteKey( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegDeleteValue___STATIC__VOID__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        Neutron::RegDeleteValue( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::ReqQueryKey___STATIC__VOID__U4__BYREF_CanFlyFieldType__BYREF_STRING__BYREF_U2__BYREF_U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

         param1;
        UINT8 heapblock1[CLR_RT_HEAP_BLOCK_SIZE];
        NANOCLR_CHECK_HRESULT( Interop_Marshal__ByRef( stack, heapblock1, param1 ) );

         param2;
        UINT8 heapblock2[CLR_RT_HEAP_BLOCK_SIZE];
        NANOCLR_CHECK_HRESULT( Interop_Marshal__ByRef( stack, heapblock2, param2 ) );

         param3;
        UINT8 heapblock3[CLR_RT_HEAP_BLOCK_SIZE];
        NANOCLR_CHECK_HRESULT( Interop_Marshal__ByRef( stack, heapblock3, param3 ) );

         param4;
        UINT8 heapblock4[CLR_RT_HEAP_BLOCK_SIZE];
        NANOCLR_CHECK_HRESULT( Interop_Marshal__ByRef( stack, heapblock4, param4 ) );

        Neutron::ReqQueryKey( param0, param1, param2, param3, param4, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegQueryChild___STATIC__VOID__U4__STRING__BYREF_U4__BYREF_CanFlyFieldType__BYREF_U2( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

         param2;
        UINT8 heapblock2[CLR_RT_HEAP_BLOCK_SIZE];
        NANOCLR_CHECK_HRESULT( Interop_Marshal__ByRef( stack, heapblock2, param2 ) );

         param3;
        UINT8 heapblock3[CLR_RT_HEAP_BLOCK_SIZE];
        NANOCLR_CHECK_HRESULT( Interop_Marshal__ByRef( stack, heapblock3, param3 ) );

         param4;
        UINT8 heapblock4[CLR_RT_HEAP_BLOCK_SIZE];
        NANOCLR_CHECK_HRESULT( Interop_Marshal__ByRef( stack, heapblock4, param4 ) );

        Neutron::RegQueryChild( param0, param1, param2, param3, param4, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegRenameValue___STATIC__VOID__U4__STRING__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        const char* param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 2, param2 ) );

        Neutron::RegRenameValue( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetInt8___STATIC__I1__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        int8_t retValue = Neutron::RegGetInt8( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_INT8( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetInt8___STATIC__VOID__U4__STRING__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        int8_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT8( stack, 2, param2 ) );

        Neutron::RegSetInt8( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetUint8___STATIC__U1__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        uint8_t retValue = Neutron::RegGetUint8( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT8( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetUint8___STATIC__VOID__U4__STRING__U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        uint8_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 2, param2 ) );

        Neutron::RegSetUint8( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetInt16___STATIC__I2__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        int16_t retValue = Neutron::RegGetInt16( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_INT16( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetInt16___STATIC__VOID__U4__STRING__I2( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        int16_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT16( stack, 2, param2 ) );

        Neutron::RegSetInt16( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetUint16___STATIC__U2__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        uint16_t retValue = Neutron::RegGetUint16( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT16( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetUint16___STATIC__VOID__U4__STRING__U2( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        uint16_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT16( stack, 2, param2 ) );

        Neutron::RegSetUint16( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetInt32___STATIC__I4__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        signed int retValue = Neutron::RegGetInt32( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_INT32( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetInt32___STATIC__VOID__U4__STRING__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        signed int param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 2, param2 ) );

        Neutron::RegSetInt32( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetUint32___STATIC__U4__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        unsigned int retValue = Neutron::RegGetUint32( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT32( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetUint32___STATIC__VOID__U4__STRING__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        unsigned int param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 2, param2 ) );

        Neutron::RegSetUint32( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetLLA___STATIC__CanFlyLLA__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        UNSUPPORTED retValue = Neutron::RegGetLLA( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetLLA___STATIC__VOID__U4__STRING__CanFlyLLA( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        Neutron::RegSetLLA( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetXYZ___STATIC__CanFlyXYZ__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        UNSUPPORTED retValue = Neutron::RegGetXYZ( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetXYZ___STATIC__VOID__U4__STRING__CanFlyXYZ( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        Neutron::RegSetXYZ( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetMatrix___STATIC__CanFlyMatrix__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        UNSUPPORTED retValue = Neutron::RegGetMatrix( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetMatrix___STATIC__VOID__U4__STRING__CanFlyMatrix( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        Neutron::RegSetMatrix( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetQTN___STATIC__CanFlyQTN__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        UNSUPPORTED retValue = Neutron::RegGetQTN( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetQTN___STATIC__VOID__U4__STRING__CanFlyQTN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        Neutron::RegSetQTN( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetString___STATIC__STRING__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        const char* retValue = Neutron::RegGetString( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_LPCSTR( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetString___STATIC__VOID__U4__STRING__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        const char* param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 2, param2 ) );

        Neutron::RegSetString( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetFloat___STATIC__R4__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        float retValue = Neutron::RegGetFloat( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_float( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetFloat___STATIC__VOID__U4__STRING__R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        float param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_float( stack, 2, param2 ) );

        Neutron::RegSetFloat( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetBool___STATIC__BOOLEAN__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        bool retValue = Neutron::RegGetBool( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_bool( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetBool___STATIC__VOID__U4__STRING__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        bool param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_bool( stack, 2, param2 ) );

        Neutron::RegSetBool( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegStreamOpen___STATIC__CanFlyHandle__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        UNSUPPORTED retValue = Neutron::RegStreamOpen( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegStreamCreate___STATIC__CanFlyHandle__U4__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        unsigned int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 0, param0 ) );

        const char* param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param1 ) );

        UNSUPPORTED retValue = Neutron::RegStreamCreate( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegStreamClose___STATIC__VOID__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        Neutron::RegStreamClose( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegStreamDelete___STATIC__VOID__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        Neutron::RegStreamDelete( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegStreamEof___STATIC__BOOLEAN__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        bool retValue = Neutron::RegStreamEof( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_bool( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegStreamRead___STATIC__SZARRAY_U1__CanFlyHandle__U2( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        uint16_t param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT16( stack, 1, param1 ) );

        CLR_RT_TypedArray_UINT8 retValue = Neutron::RegStreamRead( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT8_ARRAY( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegStreamWrite___STATIC__VOID__CanFlyHandle__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        CLR_RT_TypedArray_UINT8 param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT8_ARRAY( stack, 1, param1 ) );

        Neutron::RegStreamWrite( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegStreamGetPos___STATIC__U4__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        unsigned int retValue = Neutron::RegStreamGetPos( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT32( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegStreamSetPos___STATIC__VOID__CanFlyHandle__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        unsigned int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 1, param1 ) );

        Neutron::RegStreamSetPos( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegStreamLength___STATIC__U4__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        unsigned int retValue = Neutron::RegStreamLength( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT32( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegStreamTruncate___STATIC__VOID__CanFlyHandle__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        unsigned int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 1, param1 ) );

        Neutron::RegStreamTruncate( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegStreamCopy___STATIC__VOID__CanFlyHandle__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        Neutron::RegStreamCopy( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegStreamPath___STATIC__STRING__CanFlyHandle__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        bool param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_bool( stack, 1, param1 ) );

        const char* retValue = Neutron::RegStreamPath( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_LPCSTR( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}
