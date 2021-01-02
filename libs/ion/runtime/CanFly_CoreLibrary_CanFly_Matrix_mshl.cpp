#include "../canflylib/CanFly_CoreLibrary.h"
#include "CanFly_CoreLibrary_CanFly_Matrix.h"

using namespace canflylib::CanFly_CoreLibrary;


HRESULT Library_CanFly_CoreLibrary_CanFly_Matrix::_ctor___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        Matrix::.ctor(  hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Matrix::get_Item___R4__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 2, param1 ) );

        float retValue = Matrix::get_Item( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_float( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Matrix::set_Item___VOID__I4__I4__R4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 2, param1 ) );

        float param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_float( stack, 3, param2 ) );

        Matrix::set_Item( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Matrix::Dot___CanFlyXYZ__CanFlyXYZ( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param0 ) );

        UNSUPPORTED retValue = Matrix::Dot( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Matrix::Transpose___CanFlyMatrix( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED retValue = Matrix::Transpose(  hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Matrix::op_Addition___STATIC__CanFlyMatrix__CanFlyMatrix__CanFlyMatrix( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED retValue = Matrix::op_Addition( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Matrix::op_Subtraction___STATIC__CanFlyMatrix__CanFlyMatrix__CanFlyMatrix( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED retValue = Matrix::op_Subtraction( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Matrix::op_Multiply___STATIC__CanFlyMatrix__CanFlyMatrix__CanFlyMatrix( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED retValue = Matrix::op_Multiply( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Matrix::op_ExclusiveOr___STATIC__CanFlyMatrix__CanFlyMatrix__CanFlyMatrix( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED retValue = Matrix::op_ExclusiveOr( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Matrix::op_Division___STATIC__CanFlyMatrix__CanFlyMatrix__CanFlyMatrix( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED retValue = Matrix::op_Division( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}
