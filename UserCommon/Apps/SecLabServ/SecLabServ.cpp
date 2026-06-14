#include "Precompiled.h"

#include <WppIncludes.h>

#include <IServiceLib.h>

#include <SecLabServ.h>

#include <shellapi.h>

#include <fcntl.h>
#include <io.h>

#include "Server.h"

BOOL g_IsService = TRUE;
SecLabService g_SecLabServiceInstance;

void OpenConsole() {
	if (AllocConsole()) {
		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
		freopen_s(&f, "CONOUT$", "w", stderr);
		freopen_s(&f, "CONIN$", "r", stdin);
	}
}

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

	if (!g_IsService)
	{
		OpenConsole();
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

ILpcServLib* g_pILpcServLib = NULL;
MyServerCallbacks MyServerCallbacksInstance;

BOOL SecLabService::ServiceInitLogic(IServiceLib* ServiceLib)
{
	BOOL RetVal = FALSE;
	ServiceLib->SetAllowStop(TRUE);

	g_pILpcServLib = ILpcServLib::GetNewInstance(TRUE);
	if (!g_pILpcServLib)
	{
		goto Leave;;
	}

	if (!g_pILpcServLib->Start(
		&MyServerCallbacksInstance,
		sizeof(LPC_SEC_LAB_SERVER_MESSAGE64),
		SEC_LAB_SERVER_PORT_NAME
	))
	{
		ESL_DBG_OUT(DBG_ERROR, "LpcServerTest Start failed\n");
		goto Leave;;
	}

	RetVal = TRUE;
Leave:
	return RetVal;
}

BOOL SecLabService::MainServiceLoop(PDWORD WaitInterval, IServiceLib* /*ServiceLib*/)
{

	if (!g_IsService)
	{
		printf("LpcServer started\n");
		printf("Press any key to exit\n\n");
		_getch();
		return FALSE;
	}

	*WaitInterval = 5000;


// Leave:
	return TRUE; // TRUE will be run again and again
}

DWORD SecLabService::ServiceExitLogic(IServiceLib* /*ServiceLib*/)
{
	if (g_pILpcServLib)
	{
		g_pILpcServLib->Stop();
		ILpcServLib::FreeInstance(g_pILpcServLib);
		g_pILpcServLib = NULL;
	}
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
