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

#include "Precompiled.h"

#include "CliRuntimeLib.h"

#define MAX_LINE 2048

void ParserInit();

HANDLE hStdInput = NULL;

void DECLSPEC_NORETURN FatalError(__in DWORD Error)
{
    PWCHAR text;

    if (FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        Error,
        0,
        (PTCHAR)&text,
        0,
        NULL))
    {
        wprintf(text);
        LocalFree(text);
    }

    RaiseException(Error, 0, 0, NULL);
    __debugbreak();
}

int __cdecl ParserWmain(int argc, LPCWSTR argv[])
{

	LONG ret = ERROR_SUCCESS;

	DWORD mode;
	int i;
	char cLine[MAX_LINE];
	char* carg[1024];
	char* cPtr;
	DWORD LineLeft = MAX_LINE;

	hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(hStdInput, &mode);
	SetConsoleMode(hStdInput, mode & (~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT)));

	ParserInit();

	cLine[0] = 0;
	cPtr = cLine;
	for (i = 0; i < argc; i++)
	{
		carg[i] = cPtr;
		LineLeft -= WideCharToMultiByte(CP_ACP, 0, argv[i], -1, cPtr, LineLeft, NULL, NULL);
		cPtr = cLine + MAX_LINE - LineLeft;
	}
	carg[i] = NULL;

	ret = ParserProcessLineEx(argc, carg);

	SetConsoleMode(hStdInput, mode);

	return ret;
}

