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

/*
Following the possible values that can be used as 'type' for the arguments in test functions
{
	"INT8"
	"UINT8"
	"INT16"
	"UINT16"
	"INT32"
	"UINT32"
	"INT64"
	"UINT64"
	"char*"
	"CMD_LINE"
};
*/

typedef struct _PARSER_FUNC
{
	char* name;
	int(*ParserCmdFunc)(_PARSER_FUNC* Cmd, int argc, char* argv[]);
	int numParams;
	int* kind;
	char* help;
	bool hideFromParser;
	bool hasExtraParams;
} PARSER_FUNC;

typedef struct _Arguments
{
    int argc;
    char** argv;
}Arguments, *PArguments;

#define TEST_FUNCTION
#define TEST_FUNCTION_HIDE
#define TEST_FUNCTION_EXTRA
#define TEST_FUNCTION_EXTRA_HIDE

#define TEST_FUNCTION_DIS
#define CMD_LINE PArguments

#define MAX_CMD_LEN 500
#define MAX_COMMANDS 100

extern int ParserHelpFunction(int argc, char* argv[]);
extern int ParserWmain(int argc, LPCWSTR argv[]);
extern int ParserProcessLineEx(int argc, char* argv[]);

extern void GetExtraArgs(
	_In_ char* FuncName,
	_Out_ int* ExtraArgc,
	_Out_ char*** ExtraArgv
);

extern void FreeExtraArgs(
	_In_ char** ExtraArgv
);

void ErrorNotEnoughParams();
void ErrorExtraParams(int argc, char* argv[]);


extern PCHAR CopyFileToMemBuff(PCHAR NtFilePathAndName, size_t* Filelength);
extern PCHAR ProfileGetJsonBuff(PCHAR JsonFile, size_t* Filelength, PCHAR JsonKey);
