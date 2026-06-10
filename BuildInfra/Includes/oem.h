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
#include <ProjectVer.h>

#ifndef _TO_STR
#define _TO_STR(_N) #_N
#endif
#ifndef TO_STR
#define TO_STR(_N) _TO_STR(_N)
#endif

#ifndef FileDescription
#define FileDescription TO_STR(TARGET_NAME)
#endif

#define MAKE_VER_STRING(maj, min, submin, bld)    _TO_STR(maj) "." _TO_STR(min) "." _TO_STR(submin) "." _TO_STR(bld)

#define  COMPANYNAME              "Ephraim Neuberger"
#define  LEGALCOPYRIGHT           "Copyright Ephraim Neuberger (C) 2026 All rights reserved."
#define  PRODUCTNAME              TO_STR(TARGET_NAME)

#ifdef _DEBUG
#define  FILEDESCRIPTION          FileDescription " " TO_STR(OS_VERSION) " Debug" 
#else
#define  FILEDESCRIPTION          FileDescription " " TO_STR(OS_VERSION) " "
#endif
#define  INTERNALNAME             TO_STR(TARGET_NAME)
#define  ORIGINALFILENAME         TO_STR(TARGET_NAME)


// Define our driver version
#define VER_FILEVERSION         OEM_MAJOR_VERSION,OEM_MINOR_VERSION,OEM_MAJ_BUILD_VERSION,OEM_MIN_BUILD_VERSION
#define VER_FILEVERSION_STR     MAKE_VER_STRING(OEM_MAJOR_VERSION,OEM_MINOR_VERSION,OEM_MAJ_BUILD_VERSION,OEM_MIN_BUILD_VERSION)

#define VENDORDESCRIPTOR        PRODUCTNAME

