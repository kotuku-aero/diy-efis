#include "../canflylib/CanFly_CoreLibrary.h"
#include "CanFly_CoreLibrary_CanFly_XYZ.h"

using namespace canflylib::CanFly_CoreLibrary;


HRESULT Library_CanFly_CoreLibrary_CanFly_XYZ::_ctor___VOID__R4__R4__R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        float param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_float( stack, 1, param0 ) );

        float param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_float( stack, 2, param1 ) );

        float param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_float( stack, 3, param2 ) );

        XYZ::.ctor( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_XYZ::get_X___R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        float retValue = XYZ::get_X(  hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_float( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_XYZ::set_X___VOID__R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        float param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_float( stack, 1, param0 ) );

        XYZ::set_X( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_XYZ::get_Y___R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        float retValue = XYZ::get_Y(  hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_float( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_XYZ::set_Y___VOID__R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        float param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_float( stack, 1, param0 ) );

        XYZ::set_Y( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_XYZ::get_Z___R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        float retValue = XYZ::get_Z(  hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_float( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_XYZ::set_Z___VOID__R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        float param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_float( stack, 1, param0 ) );

        XYZ::set_Z( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}
