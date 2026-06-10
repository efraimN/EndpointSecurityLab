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

#include <WppIncludes.h>

#include "CoverageTest_Cmd.cpp"

#include <Shlwapi.h>

/*!
Usage:
Help
Displays available functions

Help <function>
Displays help on function

Help all
Display help on all functions
*/
int TEST_FUNCTION help(CMD_LINE Args)
{
	return ParserHelpFunction(Args->argc, Args->argv);
}

int __cdecl wmain(int argc, LPCWSTR argv[])
{
	int RetVal;
	DWORD dwMode = 0;
	DWORD OlddwMode = 0;
	HANDLE hOut;

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	WPP_INIT_TRACING(EXE_FILE_NAMEW);
	{
		auto ProcEntry = [](BOOL start)
		{
			if (start)
				ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
			else
				ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

		};
		PROC_ENTRY;

		hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut == INVALID_HANDLE_VALUE)
		{
			printf_s("Failed on GetStdHandle");
			RetVal = GetLastError();
			goto Leave;
		}

		if (!GetConsoleMode(hOut, &dwMode))
		{
			printf_s("Failed on GetConsoleMode");
			RetVal = GetLastError();
			goto Leave;
		}

		OlddwMode = dwMode;

		dwMode |= ENABLE_PROCESSED_OUTPUT | 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
		if (!SetConsoleMode(hOut, dwMode))
		{
			printf_s("Failed on SetConsoleMode");
			RetVal = GetLastError();
			goto Leave;
		}

		RetVal = ParserWmain(argc, argv);
	}
	WPP_CLEANUP();

Leave:
	if (!SetConsoleMode(hOut, OlddwMode))
	{
		printf_s("Failed on SetConsoleMode");
		return GetLastError();
	}

	return RetVal;
}

VOID ConVert2DosDev(PCHAR InputFile, PCHAR* Converted2DevPath)
{
	CHAR DosDevices[] = "\\DosDevices\\";
	size_t DosDevicesLenth = ARRAY_SIZE(DosDevices);
	size_t JsonFileLength;
	PCHAR PConvertedTo2DevPath = NULL;
	JsonFileLength = strlen(InputFile);
	PConvertedTo2DevPath = new CHAR[JsonFileLength + DosDevicesLenth];
	if (!PConvertedTo2DevPath)
	{
		return;
	}
	ZeroMemory(PConvertedTo2DevPath, JsonFileLength + DosDevicesLenth);
	RtlCopyMemory(PConvertedTo2DevPath, DosDevices, DosDevicesLenth);
	RtlCopyMemory(&PConvertedTo2DevPath[DosDevicesLenth - 1], InputFile, JsonFileLength);
	*Converted2DevPath = PConvertedTo2DevPath;
}


/*!
Usage:
TestExtra

*/
int TEST_FUNCTION_EXTRA TestExtra(UINT8 arg1)
{
	UINT8 abc;
	int RetVal = -1;
	int ExtraArgc;
	char** ExtraArgv;

	abc = arg1;

	GetExtraArgs(
		"-TestExtra1",
		&ExtraArgc,
		&ExtraArgv
	);

	if (ExtraArgc <= 2)
	{
		ErrorNotEnoughParams();
		goto Leave;

	}
	RetVal = ParserProcessLineEx(ExtraArgc, ExtraArgv);


Leave:
	if (ExtraArgv)
	{
		FreeExtraArgs(ExtraArgv);
	}
	return RetVal;
}

/*!
Usage:
TestExtra1

*/
int TEST_FUNCTION_HIDE TestExtra1(UINT8 arg1, UINT8 arg2)
{
	UNREFERENCED_PARAMETER(arg1);
	UNREFERENCED_PARAMETER(arg2);
	return 0;
}
