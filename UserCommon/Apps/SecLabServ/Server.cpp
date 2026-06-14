#include "Precompiled.h"

#include <WppIncludes.h>

#include "Server.h"

extern BOOL g_IsService;

BOOLEAN MyServerCallbacks::AcceptConnect(PLPC_BASIC_MESSAGE PortMessage)
{
	PLPC_BASIC_MESSAGE64 Message = (PLPC_BASIC_MESSAGE64)PortMessage;
	if (!g_IsService)
	{
		printf("\nLPC test client connected %d\n", (UINT)Message->MessageHeader.ClientId.UniqueProcess);
	}
	return TRUE;
}

VOID MyServerCallbacks::HandleDataRequest(PLPC_BASIC_MESSAGE PortMessage)
{
	PLPC_SEC_LAB_SERVER_MESSAGE64 Message = (PLPC_SEC_LAB_SERVER_MESSAGE64)PortMessage;
	if (!g_IsService)
	{
		printf("LPC server received message: %S\n", Message->Text);
	}
	wsprintfW(Message->Text, L"Response from LPC server your process id is %d",
		(UINT)Message->BasicMessage.MessageHeader.ClientId.UniqueProcess
	);

}
