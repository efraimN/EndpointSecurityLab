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

#include <ILpcLibServ.h>

#include <SendToServiceCommon.h>

PVOID FindBytes(
	const VOID* Pattern,
	SIZE_T PatternSize,
	const VOID* Buffer,
	SIZE_T BufferSize
)
{
	if (!Pattern || !Buffer || !PatternSize || PatternSize > BufferSize)
	{
		return NULL;
	}

	const BYTE* p = (const BYTE*)Pattern;
	const BYTE* b = (const BYTE*)Buffer;

	for (SIZE_T i = 0; i <= BufferSize - PatternSize; i++)
	{
		if (RtlCompareMemory(b + i, p, PatternSize) == PatternSize)
		{
			return (PVOID)(b + i);
		}
	}

	return NULL;
}

class MyServerCallbacks : public ILpcServReceiverHandler
{
public:
	virtual BOOLEAN AcceptConnect(PLPC_BASIC_MESSAGE PortMessage, BOOLEAN ClientAlreadyConnected);
	virtual VOID HandleDataRequest(PLPC_BASIC_MESSAGE PortMessage);
};

extern BOOL g_IsService;

ILpcServLib* g_pILpcServLib = NULL;
MyServerCallbacks MyServerCallbacksInstance;

ServiceLpcServer::ServiceLpcServer()
{
}

ServiceLpcServer::~ServiceLpcServer()
{
}

BOOL ServiceLpcServer::Start()
{
	BOOL RetVal = FALSE;
	g_pILpcServLib = ILpcServLib::GetNewInstance(TRUE);
	if (!g_pILpcServLib)
	{
		goto Leave;;
	}

	if (!g_pILpcServLib->Start(
		&MyServerCallbacksInstance,
		sizeof(MessagesToUser),
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

BOOL ServiceLpcServer::Stop()
{
	if (g_pILpcServLib)
	{
		g_pILpcServLib->Stop();
		ILpcServLib::FreeInstance(g_pILpcServLib);
		g_pILpcServLib = NULL;
	}

	return TRUE;
}

BOOLEAN MyServerCallbacks::AcceptConnect(PLPC_BASIC_MESSAGE PortMessage, BOOLEAN ClientAlreadyConnected)
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
	if (PID ==4)
	{
		return TRUE;
	}
#endif
	return !ClientAlreadyConnected;
}

VOID MyServerCallbacks::HandleDataRequest(PLPC_BASIC_MESSAGE PortMessage)
{
	PMessagesToUser Message = (PMessagesToUser)PortMessage;
	KernelMessagesToUser MesageType;

	MesageType = Message->UserMessage.MesageType;
	Message->UserMessage.Result = TRUE;
	ESL_DBG_OUT(DBG_INFO, "LPC server received message: %d", MesageType);
	switch (MesageType)
	{
		case GetUsbBlocStatusMessage:
		{
			PWCHAR HardwareID;
			HardwareID = Message->UserMessage.Messages.GetUsbBlocStatus.HardwareId;
			PBOOL ShouldBlock = &Message->UserMessage.Messages.GetUsbBlocStatus.ShouldBlock;

			if (!g_IsService)
			{
				printf("LPC server received HardwareID: %S\n", HardwareID);
			}

			*ShouldBlock = TRUE;
		}
		break;
		case GetOpenFileVeredictMessage:
		{
			PGet_Open_File_Veredict pGetOpenFileVeredict;
			PWCHAR vSid;
			PBOOL ShouldBlock;
			NTSTATUS status;
			BYTE Buffer[256];
			IO_STATUS_BLOCK iosb = {};
			LARGE_INTEGER ByteOffset = {};
			BYTE Patern[] = "DOS mode";

			pGetOpenFileVeredict = &Message->UserMessage.Messages.GetOpenFileVeredict;
			vSid = pGetOpenFileVeredict->SidString;
			ShouldBlock = &pGetOpenFileVeredict->ShouldBlock;
			*ShouldBlock = FALSE; // just in case
			if (!g_IsService)
			{
				printf("LPC server received vSid: %S\n", vSid);
			}

			ByteOffset.QuadPart = 0;

			status = ZwReadFile(
				(HANDLE)pGetOpenFileVeredict->FileHandle,
				NULL,       // Event
				NULL,       // ApcRoutine
				NULL,       // ApcContext
				&iosb,
				Buffer,
				sizeof(Buffer),
				&ByteOffset,
				NULL        // Key
			);

			if (!NT_SUCCESS(status))
			{
				break;
			}

			if (FindBytes(
				Patern,
				sizeof(Patern)-1,
				Buffer,
				255
			))
			{
				*ShouldBlock = TRUE;
			}
	
			ZwClose((HANDLE)pGetOpenFileVeredict->FileHandle);

		}
		break;
		default:
		{
			Message->UserMessage.Result = FALSE;
		}
		break;
	}


}

