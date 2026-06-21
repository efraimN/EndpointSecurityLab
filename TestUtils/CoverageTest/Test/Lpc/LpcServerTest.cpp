/*
Copyright(c) 2026 - present, Ephraim Neuberger.All rights reserved.

The code and materials provided by Ephraim Neuberger are for non - commercial testing and evaluation purposes only.
Ephraim Neuberger reserves all rights not expressly granted.

Any permitted copy must retain this copyright notice and disclaimer.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL EPHRAIM NEUBERGER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include <WppIncludes.h>

#include <ILpcLibServ.h>

#include "LpcTestCommon.h"


class CLpcTestServerCallbacks : public ILpcServReceiverHandler
{
public:
	virtual BOOLEAN AcceptConnect(PLPC_BASIC_MESSAGE PortMessage, BOOLEAN /*ClientAlreadyConnected*/)
	{
		PLPC_BASIC_MESSAGE64 Message = (PLPC_BASIC_MESSAGE64)PortMessage;
		UINT PID;

		PID = (UINT)Message->MessageHeader.ClientId.UniqueProcess;

		ESL_DBG_OUT(DBG_INFO, "LPC test client connected %d", PID);
		printf("\nLPC test client connected %d\n", PID);
		return TRUE;
	}

	virtual VOID HandleDataRequest(PLPC_BASIC_MESSAGE PortMessage)
	{
		PLPC_SEC_LAB_SERVER_MESSAGE64 LPCSecLabServerMessage = (PLPC_SEC_LAB_SERVER_MESSAGE64)PortMessage;

		PWCHAR MessageText;

		MessageText = LPCSecLabServerMessage->Text;

		printf("LPC server received message: %S\n", MessageText);
		wsprintfW(MessageText, L"Response from LPC server your process id is %d",
			(UINT)LPCSecLabServerMessage->BasicMessage.MessageHeader.ClientId.UniqueProcess);
	}
};

void WaitToFinish()
{
	printf("Press any key to exit\n\n");
	_getch();
}

decltype(WaitToFinish)* G_WaitToFinish = WaitToFinish;

/*!
Usage:
CoverageTest LpcServerTest

Do not use

for debug purposes

*/
int TEST_FUNCTION LpcServerTest()
{
	int RetVal = -1;
	ILpcServLib* pILpcServLib = NULL;
	CLpcTestServerCallbacks MyCLpcTestServerCallbacks;

	pILpcServLib = ILpcServLib::GetNewInstance(TRUE);
	if (!pILpcServLib)
	{
		goto Leave;;
	}

	if (!pILpcServLib->Start(
		&MyCLpcTestServerCallbacks,
		sizeof(LPC_SEC_LAB_SERVER_MESSAGE64),
		SEC_LAB_LPC_TEST_PORT_NAME))
	{
		printf("LpcServerTest Start failed\n");
		goto Leave;;
	}

	printf("LpcServerTest server started\n");

	G_WaitToFinish();

	RetVal = 0;
Leave:
	if (pILpcServLib)
	{
		pILpcServLib->Stop();
		ILpcServLib::FreeInstance(pILpcServLib);
		pILpcServLib = NULL;
	}

	return RetVal;
}

extern int LpcClientTest();

/*!
Usage:
CoverageTest LpcLoopbackTest

Do not use

for debug purposes

*/
int TEST_FUNCTION LpcLoopbackTest()
{
	auto RunClientTest = []()
	{
		printf("Loopback mode: server started, continuing to client test\n\n");
		Sleep(1000);
		LpcClientTest();
	};


	G_WaitToFinish = RunClientTest;

	LpcServerTest();

	return 0;
}
