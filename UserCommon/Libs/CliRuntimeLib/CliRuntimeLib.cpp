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

#include <array>

extern void DECLSPEC_NORETURN FatalError(__in DWORD Error);

#define MAX_VAL_BUF_LEN 128

#define whitespace(c) (((c) == ' ') || ((c) == '\t'))

#ifndef MIN
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#endif

#ifndef MAX
#define MAX(x,y) ((x)>(y) ? (x):(y))
#endif

#define ROUND_LEN(x) ((x + 3) & ~3)


/* GLOBALS ********************************************************************/

#define MAX_PARSER_RECURSION 16

typedef struct _PARSER_ARGS_CONTEXT
{
	int argc;
	char** argv;
	int numParams;
} PARSER_ARGS_CONTEXT;

static std::array<PARSER_ARGS_CONTEXT, MAX_PARSER_RECURSION> G_ParserArgsStack;
static int G_ParserArgsDepth = 0;

static int  G_ParserCommandsCount = 0;
PARSER_FUNC *G_ParserCommands = NULL;

typedef struct
{
	char* name;
	int     size;
	int     isSigned;
	char* format;
} PARSER_VAR_KIND;

/* FUNCTIONS ******************************************************************/

PARSER_VAR_KIND var_kinds[] =
{
    {"INT8",        sizeof(INT8),   1, "%hhi"},     // kind = 1
    {"UINT8",       sizeof(UINT8),  0, "%hhu"},     // kind = 2
    {"INT16",       sizeof(INT16),  1, "%hi"},      // kind = 3
    {"UINT16",      sizeof(UINT16), 0, "%hu"},      // kind = 4
    {"INT32",       sizeof(INT32),  1, "%li"},      // kind = 5
    {"UINT32",      sizeof(UINT32), 0, "%lu"},      // kind = 6
    {"INT64",       sizeof(INT64),  1, "%lli"},     // kind = 7
    {"UINT64",      sizeof(UINT64), 0, "%llu"},     // kind = 8
    {"char*",       sizeof(char*),  0, "%s"},       // kind = 9
    { "CMD_LINE", 0, 0, NULL },                     // kind = 10
};

static
void WaitForInput()
{
	printf_s("press any key to continue");
	while (!_kbhit());
    if (_getch()) {};
}

bool CaseInSensStringCompare(std::string& str1, std::string& str2)
{
    auto CmpChar = [](char& c1, char& c2)
    {
        return (c1 == c2 || std::toupper(c1) == std::toupper(c2));
    };

	return ((str1.size() == str2.size()) && std::equal(str1.begin(), str1.end(), str2.begin(), CmpChar));
}

void ParserRegisterModule(PARSER_FUNC* Func, int FuncCount)
{
    G_ParserCommands = Func;
    G_ParserCommandsCount = FuncCount;
}

static PARSER_FUNC* ParserFindCommand(char* InCmdName, bool AllowHidden)
{
	std::string CmdName = InCmdName;

	int i;

	if (CmdName == "?")
	{
		CmdName = "-help";
	}

	for (i = 0; i < G_ParserCommandsCount; i++)
	{
		if (G_ParserCommands[i].hideFromParser && !AllowHidden)
		{
			continue;
		}
		
		std::string tmp = G_ParserCommands[i].name;
		if (CaseInSensStringCompare(CmdName, tmp))
		{
			return &G_ParserCommands[i];
		}
	}

	return NULL;
}

static int ParserPrintCmdHelp(char *CmdName)
{
    PARSER_FUNC 	*func;

    func = ParserFindCommand(CmdName, true);
    if(func == NULL)
    {
        printf_s("No such command: %s\r\n", CmdName);
		WaitForInput();
        return -1;
    }

    if (func->help)
    {
        printf_s("    help for: %s\r\n", func->name);
        printf_s("%s\r\n", func->help);
    }
    return 0;
}

int ParserHelpFunction(int argc, char* argv[])
{
    char CmdName[MAX_CMD_LEN+1]={0};

    if((argc - 2) == 0)
    {
        /* If no argument then print all the available cmd names*/
        int i;
        printf_s("Available commands are:\r\n");
        for(i=0; i< G_ParserCommandsCount; i++)
        {
			if (G_ParserCommands[i].hideFromParser)
			{
				continue;
			}
			printf_s("    %s\r\n", G_ParserCommands[i].name);
        }
        printf_s("type -help <command> for help on specific command:\r\n");
        printf_s("type -help all for help on all the commands:\r\n");
    }
    else
    {
        /* Print specific cmd help*/
		strcpy_s(CmdName, MAX_CMD_LEN, argv[2]);
        if (0 == _stricmp(CmdName,"all"))
        {
            int i;
            for(i=0; i< G_ParserCommandsCount; i++)
            {
				if (G_ParserCommands[i].hideFromParser)
				{
					continue;
				}
				ParserPrintCmdHelp(G_ParserCommands[i].name);
            }
        }
        else
        {
            ParserPrintCmdHelp(CmdName);
        }
    }
    return 0;
}

void ErrorNotEnoughParams()
{
	printf_s("not enough parameters\r\n");
	WaitForInput();
}

char Buffer[2048];
static
void ErrorExtraParamsInternal(int argc, char* argv[], int FirstExtraParam)
{
	char* Line = (char*)Buffer;

	Line[0] = 0;

	for (int i = FirstExtraParam; i < argc; i++)
	{
		strcat_s(Line, 2048, argv[i + 2]);
		strcat_s(Line, 2048, " ");
	}

	printf_s("extra parameter(s) found %s\r\n", Line);
	WaitForInput();
}

void ErrorExtraParams(int argc, char* argv[])
{
	ErrorExtraParamsInternal(argc-2, argv, 0);
}

static
void SetCurrentArgsNumParams(
	_In_ int NumParams
);
BOOL ProcessParameters(PVOID Params, PARSER_FUNC* Cmd, int argc, char* argv[])
{
	INT i;
	PARSER_VAR_KIND Kind;
	PCHAR VarStructPtr = (PCHAR)Params;
	CHAR Fmt[16];
	UINT n;
	int err;

	SetCurrentArgsNumParams(Cmd->numParams);

	if ((Cmd->kind) && Cmd->kind[0] == 10)
	{
		PArguments Args = (PArguments)Params;

		Args->argc = argc;
		Args->argv = argv;
		return TRUE;
	}

	argc -= 2;

	if ((Cmd->kind) && (Cmd->kind[0] == 0))
	{
		if (argc == 0)
		{
			return TRUE; // is a void functions
		}

		ErrorExtraParamsInternal(argc, argv, 0);
		return FALSE;
	}

	if (argc < Cmd->numParams)
	{
		ErrorNotEnoughParams();
		return FALSE;
	}

	for (i = 0; i <= Cmd->numParams; i++)
	{
		if ((i == Cmd->numParams) && (argc > Cmd->numParams))
		{
			if (Cmd->hasExtraParams)
			{
				return TRUE;
			}

			ErrorExtraParamsInternal(argc, argv, Cmd->numParams);
			return FALSE;
		}

		if (i == Cmd->numParams)
		{
			return TRUE; // finished
		}

		Kind = var_kinds[Cmd->kind[i] - 1];

		if (!strcmp(Kind.format, "%s"))
		{
			memcpy(VarStructPtr, &argv[i + 2], Kind.size);
			VarStructPtr += Kind.size;
		}
		else
		{
			strcpy_s(Fmt, 16, Kind.format);
			strcat_s(Fmt, 16, "%n"); // This will tell us how many chars we read

			err = sscanf_s(argv[i + 2], Fmt, VarStructPtr, &n);
			if ((err != 1) || (n == 0))
			{
				printf_s("Invalid parameter %s\r\n", argv[i + 2]);
				WaitForInput();
				return FALSE;
			}

			if (n != strlen(argv[i + 2]))
			{
				printf_s("Invalid parameter %s\r\n", argv[i + 2]);
				WaitForInput();
				return FALSE;
			}

			VarStructPtr += Kind.size;
		}
	}

	return FALSE;
}

static void PushCurrentArgsForExtraArgs(int argc, char* argv[]);
static void PopCurrentArgsForExtraArgs();

int ParserProcessLine(int argc, char* argv[])
{
    PARSER_FUNC *cmd;
	int RetVal = -1;
	bool AllowHidden;

	AllowHidden = (G_ParserArgsDepth > 0);

	PushCurrentArgsForExtraArgs(argc, argv);

    if (!argv[1])
    {
        printf_s("Type -help or ? for help\r\n");
        goto Leave;
    }

	cmd = ParserFindCommand(argv[1], AllowHidden);
	if (cmd == NULL)
	{
		printf_s("No such command\r\n");
		WaitForInput();
		goto Leave;
	}

	RetVal = cmd->ParserCmdFunc(cmd, argc, argv);

Leave:
	PopCurrentArgsForExtraArgs();
	return RetVal;
}

void ParserCallError(int err)
{
    FatalError(err);
}

static void PushCurrentArgsForExtraArgs(int argc, char* argv[])
{
	if (G_ParserArgsDepth >= MAX_PARSER_RECURSION)
	{
		FatalError(ERROR_STACK_OVERFLOW);
	}

	G_ParserArgsStack[G_ParserArgsDepth].argc = argc;
	G_ParserArgsStack[G_ParserArgsDepth].argv = argv;
	G_ParserArgsStack[G_ParserArgsDepth].numParams = -1;

	G_ParserArgsDepth++;
}

static void PopCurrentArgsForExtraArgs()
{
	if (G_ParserArgsDepth <= 0)
	{
		return;
	}

	G_ParserArgsDepth--;

	G_ParserArgsStack[G_ParserArgsDepth].argc = 0;
	G_ParserArgsStack[G_ParserArgsDepth].argv = NULL;
	G_ParserArgsStack[G_ParserArgsDepth].numParams = -1;
}

static
void SetCurrentArgsNumParams(
	_In_ int NumParams
)
{
	if (G_ParserArgsDepth <= 0)
	{
		return;
	}

	G_ParserArgsStack[G_ParserArgsDepth - 1].numParams = NumParams;
}

void GetExtraArgs(
	_In_ char* FuncName,
	_Out_ int* ExtraArgc,
	_Out_ char*** ExtraArgv
)
{
	int ExtraStart;
	int ExtraCount;
	char** NewArgv;
	PARSER_ARGS_CONTEXT* Context;

	*ExtraArgc = 0;
	*ExtraArgv = NULL;

	if (G_ParserArgsDepth <= 0)
	{
		return;
	}

	Context = &G_ParserArgsStack[G_ParserArgsDepth - 1];

	if (!Context->argv || Context->numParams < 0)
	{
		return;
	}

	ExtraStart = 2 + Context->numParams;

	if (Context->argc < ExtraStart)
	{
		return;
	}

	ExtraCount = Context->argc - ExtraStart;

	NewArgv = new char* [ExtraCount + 3];
	if (!NewArgv)
	{
		return;
	}

	NewArgv[0] = (char*)"dummy";
	NewArgv[1] = FuncName;

	for (int i = 0; i < ExtraCount; i++)
	{
		NewArgv[i + 2] = Context->argv[ExtraStart + i];
	}

	NewArgv[ExtraCount + 2] = NULL;

	*ExtraArgc = ExtraCount + 2;
	*ExtraArgv = NewArgv;
}

void FreeExtraArgs(
	_In_ char** ExtraArgv
)
{
	delete[] ExtraArgv;
}


int ParserProcessLineEx(int argc, char* argv[])
{
	LONG ret = ERROR_SUCCESS;

#ifdef _DEBUG
	ret = ParserProcessLine(argc, argv);
#else
	__try
	{
		ret = ParserProcessLine(argc, argv);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		ret = GetExceptionCode();
		FatalError(ret);
	}
#endif

	return ret;
}
