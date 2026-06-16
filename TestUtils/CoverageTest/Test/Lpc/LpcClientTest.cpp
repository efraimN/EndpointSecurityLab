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

#include <SecLabServerCommon.h>

/*!
Usage:
CoverageTest LpcClientTest

Do not use

for debug purposes

*/
int TEST_FUNCTION LpcClientTest()
{
	NTSTATUS status;
	LPC_SEC_LAB_SERVER_MESSAGE64 Message;
	CLpcClientLibInt* Client;

	RtlZeroMemory(&Message, sizeof(Message));

	Client = CLpcClientLibInt::GetNewInstance(TRUE);
	if (!Client)
	{
		printf("LpcClientTest GetNewInstance failed\n");
		return -1;
	}

	if (!Client->Start(SEC_LAB_SERVER_PORT_NAME))
	{
		printf("LpcClientTest Start failed\n");
		CLpcClientLibInt::FreeInstance(Client);
		return -2;
	}

	printf("LpcClientTest client started\n");

	wcscpy_s(Message.Text, L"Hello from LPC client message 1");
	status = Client->SendMessageWaitResponse(
		(PLPC_BASIC_MESSAGE)&Message,
		FALSE
	);

	if (!NT_SUCCESS(status))
	{
		printf("LpcClientTest SendMessageWaitResponse async failed\n");
	}

	printf("LpcClientTest SendMessageWaitResponse sync succeed\n\n");

	wcscpy_s(Message.Text, L"Hello from LPC client message 2");
	status = Client->SendMessageWaitResponse(
		(PLPC_BASIC_MESSAGE)&Message,
		TRUE
	);

	if (!NT_SUCCESS(status))
	{
		printf("LpcClientTest SendMessageWaitResponse sync failed\n");
	}

	printf("LpcClientTest SendMessageWaitResponse sync received message:\n%S\n\n",
		Message.Text);

	Client->Stop();
	CLpcClientLibInt::FreeInstance(Client);

	return NT_SUCCESS(status) ? 0 : (int)status;
}

