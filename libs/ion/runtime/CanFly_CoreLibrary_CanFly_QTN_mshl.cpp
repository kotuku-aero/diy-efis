#include "../canflylib/CanFly_CoreLibrary.h"
#include "CanFly_CoreLibrary_CanFly_QTN.h"

using namespace canflylib::CanFly_CoreLibrary;


HRESULT Library_CanFly_CoreLibrary_CanFly_QTN::_ctor___VOID__R4__R4__R4__R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        float param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_float( stack, 1, param0 ) );

        float param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_float( stack, 2, param1 ) );

        float param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_float( stack, 3, param2 ) );

        float param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_float( stack, 4, param3 ) );

        QTN::.ctor( param0, param1, param2, param3, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_QTN::get_Q0___R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        float retValue = QTN::get_Q0(  hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_float( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_QTN::set_Q0___VOID__R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        float param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_float( stack, 1, param0 ) );

        QTN::set_Q0( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_QTN::get_Q1___R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        float retValue = QTN::get_Q1(  hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_float( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_QTN::set_Q1___VOID__R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        float param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_float( stack, 1, param0 ) );

        QTN::set_Q1( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_QTN::get_Q2___R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        float retValue = QTN::get_Q2(  hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_float( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_QTN::set_Q2___VOID__R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        float param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_float( stack, 1, param0 ) );

        QTN::set_Q2( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_QTN::get_Q3___R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        float retValue = QTN::get_Q3(  hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_float( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_QTN::set_Q3___VOID__R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        float param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_float( stack, 1, param0 ) );

        QTN::set_Q3( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}
