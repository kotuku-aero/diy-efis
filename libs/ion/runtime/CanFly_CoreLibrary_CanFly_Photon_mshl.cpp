#include "../canflylib/CanFly_CoreLibrary.h"
#include "CanFly_CoreLibrary_CanFly_Photon.h"

using namespace canflylib::CanFly_CoreLibrary;


HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::OpenScreen___STATIC__CanFlyHandle__U2__CanFlywndproc__U2( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        uint16_t param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT16( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        uint16_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT16( stack, 2, param2 ) );

        UNSUPPORTED retValue = Photon::OpenScreen( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::CreateWindow___STATIC__CanFlyHandle__CanFlyHandle__CanFlyRect__CanFlywndproc__U2( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        uint16_t param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT16( stack, 3, param3 ) );

        UNSUPPORTED retValue = Photon::CreateWindow( param0, param1, param2, param3, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::CreateChildWindow___STATIC__CanFlyHandle__CanFlyHandle__CanFlyRect__CanFlywndproc__U2( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        uint16_t param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT16( stack, 3, param3 ) );

        UNSUPPORTED retValue = Photon::CreateChildWindow( param0, param1, param2, param3, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::CloseWindow___STATIC__VOID__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        Photon::CloseWindow( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetWindowRect___STATIC__CanFlyRect__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED retValue = Photon::GetWindowRect( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetWindowPos___STATIC__CanFlyRect__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED retValue = Photon::GetWindowPos( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::SetWindowPos___STATIC__VOID__CanFlyHandle__CanFlyRect( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        Photon::SetWindowPos( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetWindowData___STATIC__OBJECT__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED retValue = Photon::GetWindowData( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetParent___STATIC__CanFlyHandle__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED retValue = Photon::GetParent( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetWindowById___STATIC__CanFlyHandle__CanFlyHandle__U2( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        uint16_t param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT16( stack, 1, param1 ) );

        UNSUPPORTED retValue = Photon::GetWindowById( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetFirstChild___STATIC__CanFlyHandle__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED retValue = Photon::GetFirstChild( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetNextSibling___STATIC__CanFlyHandle__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED retValue = Photon::GetNextSibling( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetPreviousSibling___STATIC__CanFlyHandle__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED retValue = Photon::GetPreviousSibling( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::InsertBefore___STATIC__VOID__CanFlyHandle__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        Photon::InsertBefore( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::InsertAfter___STATIC__VOID__CanFlyHandle__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        Photon::InsertAfter( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetZOrder___STATIC__U1__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        uint8_t retValue = Photon::GetZOrder( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT8( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::SetZOrder___STATIC__VOID__CanFlyHandle__U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        uint8_t param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT8( stack, 1, param1 ) );

        Photon::SetZOrder( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::SetWindowData___STATIC__VOID__CanFlyHandle__OBJECT( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        Photon::SetWindowData( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetMessage___STATIC__BOOLEAN__CanFlyHandle__BYREF_CanFlyHandle__BYREF_CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

         param1;
        UINT8 heapblock1[CLR_RT_HEAP_BLOCK_SIZE];
        NANOCLR_CHECK_HRESULT( Interop_Marshal__ByRef( stack, heapblock1, param1 ) );

         param2;
        UINT8 heapblock2[CLR_RT_HEAP_BLOCK_SIZE];
        NANOCLR_CHECK_HRESULT( Interop_Marshal__ByRef( stack, heapblock2, param2 ) );

        bool retValue = Photon::GetMessage( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_bool( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::DispatchMessage___STATIC__VOID__CanFlyHandle__CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        Photon::DispatchMessage( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::PostMessage___STATIC__VOID__CanFlyHandle__CanFlyCanFlyMsg__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        signed int param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 2, param2 ) );

        Photon::PostMessage( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::SendMessage___STATIC__VOID__CanFlyHandle__CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        Photon::SendMessage( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::CallDefaultWindowProc___STATIC__BOOLEAN__CanFlyHandle__CanFlyCanFlyMsg( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        bool retValue = Photon::CallDefaultWindowProc( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_bool( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::CanvasClose___STATIC__VOID__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        Photon::CanvasClose( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::CreateRectCanvas___STATIC__CanFlyHandle__CanFlyExtent( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED retValue = Photon::CreateRectCanvas( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::CreateBitmapCanvas___STATIC__CanFlyHandle__CanFlyBitmap( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED retValue = Photon::CreateBitmapCanvas( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::CreatePngCanvas___STATIC__CanFlyHandle__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED retValue = Photon::CreatePngCanvas( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::LoadPng___STATIC__VOID__CanFlyHandle__CanFlyHandle__CanFlyPoint( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        Photon::LoadPng( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetCanvasExtents___STATIC__CanFlyExtent__CanFlyHandle__BYREF_U2( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

         param1;
        UINT8 heapblock1[CLR_RT_HEAP_BLOCK_SIZE];
        NANOCLR_CHECK_HRESULT( Interop_Marshal__ByRef( stack, heapblock1, param1 ) );

        UNSUPPORTED retValue = Photon::GetCanvasExtents( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetCanvasExtents___STATIC__CanFlyExtent__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED retValue = Photon::GetCanvasExtents( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetOrientation___STATIC__VOID__CanFlyHandle__BYREF_SZARRAY_U2( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

         param1;
        UINT8 heapblock1[CLR_RT_HEAP_BLOCK_SIZE];
        NANOCLR_CHECK_HRESULT( Interop_Marshal__ByRef( stack, heapblock1, param1 ) );

        Photon::GetOrientation( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::SetOrientation___STATIC__VOID__CanFlyHandle__U2( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        uint16_t param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT16( stack, 1, param1 ) );

        Photon::SetOrientation( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::Polyline___STATIC__VOID__CanFlyHandle__CanFlyRect__CanFlyPen__SZARRAY_CanFlyPoint( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        CLR_RT_TypedArray_UNSUPPORTED param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED_ARRAY( stack, 3, param3 ) );

        Photon::Polyline( param0, param1, param2, param3, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::Ellipse___STATIC__VOID__CanFlyHandle__CanFlyRect__CanFlyPen__U4__CanFlyRect( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        unsigned int param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 3, param3 ) );

        UNSUPPORTED param4;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 4, param4 ) );

        Photon::Ellipse( param0, param1, param2, param3, param4, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::Polygon___STATIC__VOID__CanFlyHandle__CanFlyRect__CanFlyPen__U4__SZARRAY_CanFlyPoint( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        unsigned int param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 3, param3 ) );

        CLR_RT_TypedArray_UNSUPPORTED param4;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED_ARRAY( stack, 4, param4 ) );

        Photon::Polygon( param0, param1, param2, param3, param4, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::PolyPolygon___STATIC__VOID__CanFlyHandle__CanFlyRect__CanFlyPen__U4__SZARRAY_CanFlyPoint( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        unsigned int param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 3, param3 ) );

        CLR_RT_TypedArray_UNSUPPORTED param4;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED_ARRAY( stack, 4, param4 ) );

        Photon::PolyPolygon( param0, param1, param2, param3, param4, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::Rectangle___STATIC__VOID__CanFlyHandle__CanFlyRect__CanFlyPen__U4__CanFlyRect( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        unsigned int param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 3, param3 ) );

        UNSUPPORTED param4;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 4, param4 ) );

        Photon::Rectangle( param0, param1, param2, param3, param4, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::RoundRect___STATIC__VOID__CanFlyHandle__CanFlyRect__CanFlyPen__U4__CanFlyRect__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        unsigned int param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 3, param3 ) );

        UNSUPPORTED param4;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 4, param4 ) );

        signed int param5;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 5, param5 ) );

        Photon::RoundRect( param0, param1, param2, param3, param4, param5, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::BitBlt___STATIC__VOID__CanFlyHandle__CanFlyRect__CanFlyRect__CanFlyHandle__CanFlyRect__CanFlyPoint( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        UNSUPPORTED param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 3, param3 ) );

        UNSUPPORTED param4;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 4, param4 ) );

        UNSUPPORTED param5;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 5, param5 ) );

        Photon::BitBlt( param0, param1, param2, param3, param4, param5, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetPixel___STATIC__U4__CanFlyHandle__CanFlyRect__CanFlyPoint( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        unsigned int retValue = Photon::GetPixel( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT32( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::SetPixel___STATIC__U4__CanFlyHandle__CanFlyRect__CanFlyPoint__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        unsigned int param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 3, param3 ) );

        unsigned int retValue = Photon::SetPixel( param0, param1, param2, param3, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT32( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::Arc___STATIC__VOID__CanFlyHandle__CanFlyRect__CanFlyPen__CanFlyPoint__I4__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        UNSUPPORTED param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 3, param3 ) );

        signed int param4;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 4, param4 ) );

        signed int param5;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 5, param5 ) );

        signed int param6;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 6, param6 ) );

        Photon::Arc( param0, param1, param2, param3, param4, param5, param6, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::Pie___STATIC__VOID__CanFlyHandle__CanFlyRect__CanFlyPen__U4__CanFlyPoint__I4__I4__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        unsigned int param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 3, param3 ) );

        UNSUPPORTED param4;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 4, param4 ) );

        signed int param5;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 5, param5 ) );

        signed int param6;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 6, param6 ) );

        signed int param7;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 7, param7 ) );

        signed int param8;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 8, param8 ) );

        Photon::Pie( param0, param1, param2, param3, param4, param5, param6, param7, param8, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::OpenFont___STATIC__CanFlyHandle__STRING__U2( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        const char* param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 0, param0 ) );

        uint16_t param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT16( stack, 1, param1 ) );

        UNSUPPORTED retValue = Photon::OpenFont( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::DrawText___STATIC__VOID__CanFlyHandle__CanFlyRect__CanFlyHandle__U4__U4__STRING__CanFlyPoint__CanFlyRect__CanFlyTextOutStyle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        UNSUPPORTED param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 2, param2 ) );

        unsigned int param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 3, param3 ) );

        unsigned int param4;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 4, param4 ) );

        const char* param5;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 5, param5 ) );

        UNSUPPORTED param6;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 6, param6 ) );

        UNSUPPORTED param7;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 7, param7 ) );

        UNSUPPORTED param8;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 8, param8 ) );

        Photon::DrawText( param0, param1, param2, param3, param4, param5, param6, param7, param8, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::TextExtent___STATIC__CanFlyExtent__CanFlyHandle__CanFlyHandle__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        const char* param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 2, param2 ) );

        UNSUPPORTED retValue = Photon::TextExtent( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UNSUPPORTED( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::InvalidateRect___STATIC__VOID__CanFlyHandle__CanFlyRect( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        UNSUPPORTED param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 1, param1 ) );

        Photon::InvalidateRect( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::IsValid___STATIC__BOOLEAN__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        bool retValue = Photon::IsValid( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_bool( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::BeginPaint___STATIC__VOID__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        Photon::BeginPaint( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::EndPaint___STATIC__VOID__CanFlyHandle( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        UNSUPPORTED param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED( stack, 0, param0 ) );

        Photon::EndPaint( param0, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}
