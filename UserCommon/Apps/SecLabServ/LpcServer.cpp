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

#include "LpcServer.h"

extern BOOL g_IsService;

ILpcServLib* g_pILpcServLib = NULL;
MyServerCallbacks MyServerCallbacksInstance;

MyLpcServer::MyLpcServer()
{
}

MyLpcServer::~MyLpcServer()
{
}

BOOL MyLpcServer::Start()
{
	BOOL RetVal = FALSE;
	g_pILpcServLib = ILpcServLib::GetNewInstance(TRUE);
	if (!g_pILpcServLib)
	{
		goto Leave;;
	}

	if (!g_pILpcServLib->Start(
		&MyServerCallbacksInstance,
		SIZE_OF_LCP_MESSAGE,
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

BOOL MyLpcServer::Stop()
{
	if (g_pILpcServLib)
	{
		g_pILpcServLib->Stop();
		ILpcServLib::FreeInstance(g_pILpcServLib);
		g_pILpcServLib = NULL;
	}

	return TRUE;
}

BOOLEAN MyServerCallbacks::AcceptConnect(PLPC_BASIC_MESSAGE PortMessage)
{
	PLPC_BASIC_MESSAGE64 Message = (PLPC_BASIC_MESSAGE64)PortMessage;
	UINT PID;

	// Assume the driver will connect from DriverEntry or any system thread
	PID = (UINT)Message->MessageHeader.ClientId.UniqueProcess;
	ESL_DBG_OUT(DBG_INFO, "LPC test client connected %d", PID);
	if (!g_IsService)
	{
		printf("\nLPC test client connected %d\n", PID);

	}
#ifndef _DEBUG
	if (PID !=4)
	{
		return FALSE;
	}
#endif
	return TRUE;
}

VOID MyServerCallbacks::HandleDataRequest(PLPC_BASIC_MESSAGE PortMessage)
{
	PMessagesToUser Message = (PMessagesToUser)PortMessage;
	KernelMessagesToUser MesageType;

	MesageType = Message->MesageType;
	ESL_DBG_OUT(DBG_INFO, "LPC server received message: %d", MesageType);
	switch (MesageType)
	{
		case SecLabTset:
		{
			PWCHAR MessageText;
			MessageText = Message->Messages.LPCSecLabServerMessage.Text;
			if (!g_IsService)
			{
				printf("LPC server received message: %S\n", MessageText);
			}
			wsprintfW(MessageText, L"Response from LPC server your process id is %d",
				(UINT)Message->BasicMessage.MessageHeader.ClientId.UniqueProcess);
		}
		break;

		case GetLpcInBlacList:
		{
			PWCHAR HardwareID;
			HardwareID = Message->Messages.GetLPCBlocStatus.HardwareId;
			PBOOL ShouldBlock = &Message->Messages.GetLPCBlocStatus.ShouldBlock;

			if (!g_IsService)
			{
				printf("LPC server received HardwareID: %S\n", HardwareID);
			}

			*ShouldBlock = TRUE;
		}
		break;

		default:
		{
		}
		break;
	}


}

