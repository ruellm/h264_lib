/* ------------------------------------------------------------------
 * Copyright (C) 1998-2010 PacketVideo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */

#ifndef OSCLCONFIG_H_INCLUDED
#define OSCLCONFIG_H_INCLUDED

//#undef PV_MEMORY_POOL

#define _CRT_SECURE_DEPRECATE_MEMORY
#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>

#define PV_CPU_ARCH_VERSION 6
#define PV_COMPILER EPV_ARM_GNUC

//typedef unsigned int64_t uint64_t;

// 64-bit int
#define OSCL_NATIVE_INT64_TYPE     int64_t
#define OSCL_NATIVE_UINT64_TYPE    unsigned long long //uint64_t
#define INT64(x) x##LL
#define UINT64(x) x##ULL
#define INT64_HILO(high,low) ((((high##LL))<<32)|low)
#define UINT64_HILO(high,low) ((((high##ULL))<<32)|low)

#define OSCL_HAS_UNICODE_SUPPORT            0
#define OSCL_NATIVE_WCHAR_TYPE wchar_t

#define OSCL_ASSERT_ALWAYS					1
#define OSCL_DISABLE_INLINES				0
#define OSCL_HAS_ANSI_STDLIB_SUPPORT		0
#define OSCL_HAS_ANSI_STDIO_SUPPORT			0
#define OSCL_HAS_ANSI_STRING_SUPPORT		0

#define _STRLIT(x) x
#define _STRLIT_CHAR(x) x
#define _STRLIT_WCHAR(x) L ## x


#define OSCL_INTEGERS_WORD_ALIGNED			0
#define OSCL_BYTE_ORDER_BIG_ENDIAN			0
#define OSCL_BYTE_ORDER_LITTLE_ENDIAN		1
#define OSCL_HAS_GLOBAL_VARIABLE_SUPPORT    0
#define OSCL_MEMFRAG_PTR_BEFORE_LEN			0
#define	OSCL_HAS_TLS_SUPPORT				0
#define OSCL_TLS_IS_KEYED					0
#define OSCL_HAS_BASIC_LOCK					0
#define OSCL_HAS_PRAGMA_PACK				1


//typedef pthread_key_t TOsclTlsKey ;
#define OSCL_TLS_KEY_CREATE_FUNC(key)		0
#define OSCL_TLS_KEY_DELETE_FUNC(key)		0
#define OSCL_TLS_STORE_FUNC(key,ptr)		0
#define OSCL_TLS_GET_FUNC(key)				0


#include <math.h>
#define OSCL_HAS_ANSI_MATH_SUPPORT			1

// include common include for determining sizes from limits.h
#include "osclconfig_limits_typedefs.h"

/*! \addtogroup osclconfig OSCL config
 *
 * @{
 */

//a file to turn off ALL os-specific switches.

//osclconfig
#define OSCL_HAS_UNIX_SUPPORT               0
#define OSCL_HAS_MSWIN_SUPPORT              0
#define OSCL_HAS_MSWIN_PARTIAL_SUPPORT      0
#define OSCL_HAS_SYMBIAN_SUPPORT            0
#define OSCL_HAS_SAVAJE_SUPPORT             0
#define OSCL_HAS_PV_C_OS_SUPPORT            0
#define OSCL_HAS_ANDROID_SUPPORT            0
#define OSCL_HAS_IPHONE_SUPPORT             0

//osclconfig_error
#define OSCL_HAS_SYMBIAN_ERRORTRAP 0

//osclconfig_memory
#define OSCL_HAS_SYMBIAN_MEMORY_FUNCS 0
#define OSCL_HAS_PV_C_OS_API_MEMORY_FUNCS 0

//osclconfig_time
#define OSCL_HAS_PV_C_OS_TIME_FUNCS 0
#define OSCL_HAS_UNIX_TIME_FUNCS    0

//osclconfig_util
#define OSCL_HAS_SYMBIAN_TIMERS 0
#define OSCL_HAS_SYMBIAN_MATH   0

//osclconfig_proc
#define OSCL_HAS_SYMBIAN_SCHEDULER 0
#define OSCL_HAS_SEM_TIMEDWAIT_SUPPORT 0
#define OSCL_HAS_PTHREAD_SUPPORT 0

//osclconfig_io
#define OSCL_HAS_SYMBIAN_COMPATIBLE_IO_FUNCTION 0
#define OSCL_HAS_SAVAJE_IO_SUPPORT 0
#define OSCL_HAS_SYMBIAN_SOCKET_SERVER 0
#define OSCL_HAS_SYMBIAN_DNS_SERVER 0
#define OSCL_HAS_BERKELEY_SOCKETS 0


/*! @} */

#endif // OSCLCONFIG_CHECK_H_INCLUDED


