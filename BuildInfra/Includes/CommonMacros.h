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

// Macro for adding quotes
#define _TO_STR(_N) #_N
#define TO_STR(_N) _TO_STR(_N)
#define _TO_STR_WIDE(_N) L#_N
#define TO_STR_WIDE(_N) _TO_STR_WIDE(_N)

// Macros for concatenating tokens
#define CAT2(X,Y) X##Y
#define CAT(X,Y) CAT2(X,Y)

#define CAT_2 CAT
#define CAT_3(X,Y,Z) CAT(X,CAT(Y,Z))
#define CAT_4(A,X,Y,Z) CAT(A,CAT_3(X,Y,Z))
// etc...

#define TODO(mess) message(__FILE__ "("  TO_STR(__LINE__) ") : TODO > " __FUNCTION__ ":" mess) 

#define EXE_FILE_NAME TO_STR(TARGET_NAME)
#define EXE_FILE_NAMEW TO_STR_WIDE(TARGET_NAME)

#ifndef _countof
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#endif

#ifndef INIT_UNICODE_STRING
#define INIT_UNICODE_STRING(a) sizeof(a), sizeof(a), a
#endif

#ifndef MIN
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#endif

#ifndef MAX
#define MAX(x,y) ((x)>(y) ? (x):(y))
#endif
