//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nf_system_text.h"


HRESULT Library_nf_system_text_System_Text_UTF8Decoder::Convert___VOID__SZARRAY_U1__I4__I4__SZARRAY_CHAR__I4__I4__BOOLEAN__BYREF_I4__BYREF_I4__BYREF_BOOLEAN( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_UTF8_DECODER();
    HRESULT hr;

    CLR_RT_HeapBlock_Array* pArrayBytes;
    int32_t               byteIndex;
    int32_t               byteCount;
    CLR_RT_HeapBlock_Array* pArrayChars;
    int32_t               charIndex;
    int32_t               charCount;
    int32_t               byteUsed;
    int32_t               charUsed;
    bool                    completed;

    CLR_RT_UnicodeHelper uh;
    uint8_t* byteStart;

    // Get all the parameters
    pArrayBytes = stack.Arg1().DereferenceArray(); FAULT_ON_NULL(pArrayBytes);
    byteIndex   = stack.Arg2().NumericByRef().s4;
    byteCount   = stack.Arg3().NumericByRef().s4;

    pArrayChars = stack.Arg4(  ).DereferenceArray(); FAULT_ON_NULL(pArrayChars);
    charIndex   = stack.ArgN( 5 ).NumericByRef().s4;
    charCount   = stack.ArgN( 6 ).NumericByRef().s4;

    // Parameters error checking
    if (byteIndex < 0 || 
        byteCount < 0 || 
        charIndex < 0 || 
        charCount < 0 ||
        (int32_t)pArrayBytes->m_numOfElements - byteIndex < byteCount ||
        (int32_t)pArrayChars->m_numOfElements - charIndex < charCount )
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    _ASSERTE(pArrayBytes->m_typeOfElement == DATATYPE_U1);
    _ASSERTE(pArrayChars->m_typeOfElement == DATATYPE_CHAR);

    _ASSERTE(stack.ArgN( 8 ).DataType()  == DATATYPE_BYREF);
    _ASSERTE(stack.ArgN( 9 ).DataType()  == DATATYPE_BYREF);
    _ASSERTE(stack.ArgN( 10 ).DataType() == DATATYPE_BYREF);


    // Setup the UnicodeHelper
    byteStart = pArrayBytes->GetElement( byteIndex );
    uh.SetInputUTF8( (const char*)byteStart );

    uh.m_outputUTF16      = (uint16_t*)pArrayChars->GetElement( charIndex );
    uh.m_outputUTF16_size = charCount;

    uh.ConvertFromUTF8( charCount, false, byteCount );

    // Calculate return values
    byteUsed  = (int32_t)(uh.m_inputUTF8 - byteStart);
    charUsed  = charCount - uh.m_outputUTF16_size;
    completed = (byteUsed == byteCount);

    stack.ArgN(8).Dereference()->SetInteger( byteUsed );
    stack.ArgN(9).Dereference()->SetInteger( charUsed );
    stack.ArgN(10).Dereference()->SetBoolean( completed );

    NANOCLR_NOCLEANUP();
}
