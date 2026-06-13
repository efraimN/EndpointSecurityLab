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
	virtual BOOLEAN AcceptConnect(PLPC_BASIC_MESSAGE PortMessage)
	{
		PLPC_BASIC_MESSAGE64 Message = (PLPC_BASIC_MESSAGE64)PortMessage;
		printf("\nLPC test client connected %d\n", (UINT)Message->MessageHeader.ClientId.UniqueProcess);
		return TRUE;
	}

	virtual VOID HandleDataRequest(PLPC_BASIC_MESSAGE PortMessage)
	{
		PLPC_TEST_MESSAGE64 Message = (PLPC_TEST_MESSAGE64)PortMessage;
		printf("LPC server received message: %S\n", Message->Text);
		wsprintfW(Message->Text, L"Response from LPC server your process id is %d",
			(UINT)Message->BasicMessage.MessageHeader.ClientId.UniqueProcess
		);

	}
};


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
		sizeof(LPC_TEST_MESSAGE64),
		LPC_TEST_PORT_NAME))
	{
		printf("LpcServerTest Start failed\n");
		goto Leave;;
	}

	printf("LpcServerTest server started\n");

	printf("Press any key to exit\n\n");
	_getch();

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

