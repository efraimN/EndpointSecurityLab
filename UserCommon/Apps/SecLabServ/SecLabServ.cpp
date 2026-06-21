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

#include <IServiceLib.h>

#include <SecLabServ.h>

#include <shellapi.h>

#include <fcntl.h>
#include <io.h>

#include "LpcServer.h"

BOOL g_IsService = TRUE;
SecLabService g_SecLabServiceInstance;

ServiceLpcServer pMyLpcServer;

int _stdcall WinMain(
	HINSTANCE /*hInstance*/,
	HINSTANCE /*hPrevInstance*/,
	LPSTR     /*lpCmdLine*/,
	int       /*nShowCmd*/
)
{
	WPP_INIT_TRACING(EXE_FILE_NAMEW);

	int RetVal = -1;
	LPWSTR* Argv = NULL;
	int Argc;


	Argv = CommandLineToArgvW(
		GetCommandLineW(),
		&Argc
	);

	if (Argc == 2)
	{
		if (_wcsicmp(Argv[1], L"-exe") == 0)
		{
			g_IsService = FALSE;
		}
	}

	if (Argv)
	{
		LocalFree(Argv);
	}

	IServiceLib* pServiceLib = NULL;

	pServiceLib = IServiceLib::GetInstance();

	if (!pServiceLib->Start(
		&g_SecLabServiceInstance
	))
	{
		pServiceLib = NULL;
		goto Leave;
	}

	RetVal = pServiceLib->RunServiceLogic(g_IsService);
	if (ERROR_SUCCESS != RetVal)
	{
		goto Leave;
	}


Leave:
	if (pServiceLib)
	{
		pServiceLib->Stop();
	}

	WPP_CLEANUP();

	return RetVal;
}


BOOL SecLabService::ServiceInitLogic(IServiceLib* ServiceLib)
{
	BOOL RetVal;

	ServiceLib->SetAllowStop(TRUE);

	RetVal = pMyLpcServer.Start();

	if (!RetVal)
	{
		ESL_DBG_OUT(DBG_INFO, "LpcServer Start failed LPC not running");
	}
	else
	{
		ESL_DBG_OUT(DBG_INFO, "LpcServer Started");
	}

	return RetVal;
}

BOOL SecLabService::MainServiceLoop(PDWORD WaitInterval, IServiceLib* /*ServiceLib*/)
{

	if (!g_IsService)
	{
		printf("\nLpcServer started\n");
		printf("Press any key to exit\n\n");

		_getch();

		return FALSE;
	}

// 	ESL_DBG_OUT(DBG_INFO, "LpcServer MainLoop");
	*WaitInterval = 500;


	return TRUE; // TRUE will be run again and again
}

int SecLabService::ServiceExitLogic(IServiceLib* /*ServiceLib*/)
{
	if (!pMyLpcServer.Stop())
	{
		ESL_DBG_OUT(DBG_INFO, "ServiceExitLogic exiting with Error");

		return -1;
	}
	ESL_DBG_OUT(DBG_INFO, "ServiceExitLogic Exiting");
	return 0;
}

BOOL SecLabService::ServiceControlHandler(
	PDWORD /*RetVal*/,
	DWORD /*dwControl*/,
	DWORD /*dwEventType*/,
	LPVOID /*lpEventData*/
)
{
	return FALSE;
}
