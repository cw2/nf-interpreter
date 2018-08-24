//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

// compose sprintf format string according to requested parameters
void nf_GetFormatString(char* formatStr, char formatCh, int precision, bool isLong, bool isFloat, bool isSigned)
{
    sprintf(formatStr, "%%%s%d%s%s", isFloat ? "." : isSigned ? "-0" : "0", precision, isLong ? "l" : isFloat ? "f" : "", formatCh == 'X' ? "X" :  isFloat ? "" : isSigned ? "d" : "u");
}

HRESULT Library_corlib_native_System_Number::FormatNative___STATIC__STRING__OBJECT__CHAR__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock*     pArgs = &(stack.Arg0());
    CLR_RT_HeapBlock*     value = pArgs;
    CLR_DataType          dt;
    CLR_RT_TypeDescriptor desc;
    char result[ 24 ];
    // This is temporary buffer to create proper format string.  
    char formatStr[ 8 ];
    char formatCh  = (char)pArgs[ 1 ].NumericByRef().u1;
    int  precision =       pArgs[ 2 ].NumericByRef().s4;

    NANOCLR_CHECK_HRESULT(desc.InitializeFromObject( *value ));
    NANOCLR_CHECK_HRESULT(value->PerformUnboxing( desc.m_handlerCls ));

    dt = value->DataType();

    if(formatCh == 'G' && precision == 0)
    {
        precision = 1;
    }

    switch(dt)
    {
        case DATATYPE_I1:
            // get format string
            nf_GetFormatString(formatStr, formatCh, precision, false, false, true);
            sprintf(result, formatStr, value->NumericByRef().s1 );
            break;

        case DATATYPE_U1:
            // get format string
            nf_GetFormatString(formatStr, formatCh, precision, false, false, false);
            sprintf(result, formatStr, value->NumericByRef().u1 );
            break;

        case DATATYPE_I2:
            // get format string
            nf_GetFormatString(formatStr, formatCh, precision, false, false, true);
            sprintf(result, formatStr, value->NumericByRef().s2 );
            break;

        case DATATYPE_U2:
            // get format string
            nf_GetFormatString(formatStr, formatCh, precision, false, false, false);
            sprintf(result, formatStr, value->NumericByRef().u2 );
            break;

        case DATATYPE_I4:
            // get format string
            nf_GetFormatString(formatStr, formatCh, precision, false, false, true);
            sprintf(result, formatStr, value->NumericByRef().s4);
            break;

        case DATATYPE_U4:
            // get format string
            nf_GetFormatString(formatStr, formatCh, precision, false, false, false);
            sprintf(result, formatStr, value->NumericByRef().u4);
            break;

        case DATATYPE_I8:
            {  
              #if defined(_WIN32)
                hal_snprintf( result, ARRAYSIZE(result), "%I64d",(CLR_INT64_TEMP_CAST)value->NumericByRef().s8 ); 
              #else
                // get format string
                nf_GetFormatString(formatStr, formatCh, precision, true, false, true);
                sprintf(result, formatStr, (CLR_INT64_TEMP_CAST)value->NumericByRef().s8); 
              #endif // defined(_WIN32)
            }
            break;

        case DATATYPE_U8:
            {
              #if defined(_WIN32)
                hal_snprintf( result, ARRAYSIZE(result), "%I64u",(CLR_UINT64_TEMP_CAST)value->NumericByRef().u8 ); 
              #else
                // get format string
                nf_GetFormatString(formatStr, formatCh, precision, true, false, false);
                sprintf(result, formatStr, (CLR_UINT64_TEMP_CAST)value->NumericByRef().u8 );
              #endif // defined(_WIN32)
            }
            break;

        case DATATYPE_R4:
            {
                // All the formatCh have been converted to upper case in the managed layer
                _ASSERTE(formatCh == 'G' || formatCh == 'N' || formatCh == 'F' || formatCh == 'D');

                if (precision < 0 || precision > 99) 
                {
                    NANOCLR_SET_AND_LEAVE( CLR_E_INVALID_PARAMETER );
                }

                // DATATYPE_R4 is float and uses a precision of 9, standard sprintf format is "%.9g"

              #if defined(_WIN32) || defined(WIN32) || defined(_WIN32_WCE)
                hal_snprintf( result, ARRAYSIZE(result), "%.9g", value->NumericByRef().r4 );
              #else

              #if !defined(NANOCLR_EMULATED_FLOATINGPOINT)
                // use default precision is none is specfied AND if format in not generic
                precision = (formatCh == 'G' && precision == 1) ? 9 : precision;

                // get format string
                nf_GetFormatString(formatStr, formatCh, precision, false, true, false);

                sprintf(result, formatStr, value->NumericByRef().r4);
              #else
                CLR_INT32 f = value->NumericByRef().r4;
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
              #endif // !defined(NANOCLR_EMULATED_FLOATINGPOINT)
              #endif // defined(_WIN32)
            }
            break;

        case DATATYPE_R8:
            {
                // All the formatCh have been converted to upper case in the managed layer
                _ASSERTE(formatCh == 'G' || formatCh == 'N' || formatCh == 'F' || formatCh == 'D');

                if (precision < 0 || precision > 99) 
                {
                    NANOCLR_SET_AND_LEAVE( CLR_E_INVALID_PARAMETER );
                }

                // DATATYPE_R8 is double and uses a precision of 15, standard sprintf format is "%.15g"

              #if defined(_WIN32) || defined(WIN32) || defined(_WIN32_WCE)
                hal_snprintf( result, ARRAYSIZE(result), "%.15g", (CLR_DOUBLE_TEMP_CAST)value->NumericByRef().r8 );
              #else
              #if !defined(NANOCLR_EMULATED_FLOATINGPOINT)
                // use default precision is none is specfied AND format is not generic
                precision = (formatCh == 'G' && precision == 1) ? 15 : precision;

                // get format string
                nf_GetFormatString(formatStr, formatCh, precision, false, true, false);
                
                sprintf(result, formatStr, (CLR_DOUBLE_TEMP_CAST)value->NumericByRef().r8);
              #else
                CLR_INT64 d = (CLR_DOUBLE_TEMP_CAST)value->NumericByRef().r8;
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
              #endif // !defined(NANOCLR_EMULATED_FLOATINGPOINT)
              #endif // defined(_WIN32)
            }
            break;

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_SET_AND_LEAVE(stack.SetResult_String( result ));

    NANOCLR_NOCLEANUP();
}
