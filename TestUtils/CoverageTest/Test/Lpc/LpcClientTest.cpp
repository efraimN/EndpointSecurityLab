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

#include <ILpcLibClient.h>
#include "LpcTestCommon.h"

/*!
Usage:
CoverageTest LpcClientTest

Do not use

for debug purposes

*/
int TEST_FUNCTION LpcClientTest()
{
	NTSTATUS status;
	LPC_SEC_LAB_SERVER_MESSAGE64 LPCSecLabServerMessage;
	ILpcClientLib* Client;

	Client = ILpcClientLib::GetNewInstance(TRUE);
	if (!Client)
	{
		printf("LpcClientTest GetNewInstance failed\n");
		return -1;
	}

	if (!Client->Start(SEC_LAB_LPC_TEST_PORT_NAME))
	{
		printf("LpcClientTest Start failed\n");
		ILpcClientLib::FreeInstance(Client);
		return -2;
	}

	printf("LpcClientTest client started\n");

	RtlZeroMemory(&LPCSecLabServerMessage, sizeof(LPC_SEC_LAB_SERVER_MESSAGE64));
	wcscpy_s(LPCSecLabServerMessage.Text, L"Hello from LPC client message 1");
	status = Client->SendMessageWaitResponse(
		(PLPC_BASIC_MESSAGE)&LPCSecLabServerMessage,
		FALSE
	);
	if (!NT_SUCCESS(status))
	{
		printf("LpcClientTest SendMessageWaitResponse async failed\n");
	}
	else
	{
		printf("LpcClientTest SendMessageWaitResponse sync succeed\n\n");
	}

	RtlZeroMemory(&LPCSecLabServerMessage, sizeof(LPC_SEC_LAB_SERVER_MESSAGE64));
	wcscpy_s(LPCSecLabServerMessage.Text, L"Hello from LPC client message 2");
	status = Client->SendMessageWaitResponse(
		(PLPC_BASIC_MESSAGE)&LPCSecLabServerMessage,
		TRUE
	);
	if (!NT_SUCCESS(status))
	{
		printf("LpcClientTest SendMessageWaitResponse sync failed\n");
	}
	else
	{
		printf("LpcClientTest SendMessageWaitResponse sync received message:\n%S\n\n",
			LPCSecLabServerMessage.Text);
	}


	Client->Stop();
	ILpcClientLib::FreeInstance(Client);

	return NT_SUCCESS(status) ? 0 : (int)status;
}

