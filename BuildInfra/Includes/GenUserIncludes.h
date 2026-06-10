/*
Copyright (c) 2026 - present, Ephraim Neuberger. All rights reserved.

The code and materials provided by Ephraim Neuberger are for non-commercial testing and evaluation purposes only.
Ephraim Neuberger reserves all rights not expressly granted.

Any permitted copy must retain this copyright notice and disclaimer.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL EPHRAIM NEUBERGER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define NTDDI_VERSION NTDDI_WIN7
#define _WIN32_WINNT _WIN32_WINNT_WIN7

#ifdef __cplusplus
extern "C"
{
#endif

#include <WinSDKVer.h>

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN


#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1 //Behind the scenes, the call to strcpy is changed to a call to strcpy_s with the size argument supplied automatically... and so on
#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES 1 // enables template overloads of the secure variants
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1


#define _CRTDBG_MAP_ALLOC

#ifdef DEBUG_NEW
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif

#define _WINSOCKAPI_    // stops windows.h including winsock.h
#define WIN32_NO_STATUS
#define _WMISTR_          // do not yet load Wmistr.h
#include <Windows.h>
#undef _WMISTR_
#undef WIN32_NO_STATUS

#include <Wmistr.h>

#include <ntstatus.h>

#include <WInternl.h>
#define _NTDEF_

#ifndef _NTDDK_
#include <winioctl.h>
#endif

#include <NtDllInc.h>
#include <RtlNtIncludes.h>

#include <CommonMacros.h>
#include <CastMacros.h>

#include <oem.h>

#ifdef __cplusplus
}
#endif

#define NOEXCEPT noexcept

#include <WPPControl.h>

#include <Debug.h>

#include<new>
#include <crtdbg.h>

#include <stdio.h>
#include <stdarg.h>

#ifndef MAX_PATH_
#define MAX_PATH_ 512
#endif

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x1000
#endif
