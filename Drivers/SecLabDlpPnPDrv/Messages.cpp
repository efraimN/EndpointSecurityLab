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

#include "Messages.h"


SendMessages::SendMessages()
{
	m_Client = NULL;
	m_Inited = FALSE;
}

SendMessages::~SendMessages()
{
}

SendMessages* SendMessages::GetInstance()
{
	static SendMessages* Singletone;
	return Singletone;
}

BOOL SendMessages::Start()
{
	BOOL RetVal = FALSE;
	m_Client = CLpcClientLibInt::GetNewInstance(TRUE);
	if (!m_Client)
	{
		ESL_DBG_OUT(DBG_ERROR, "LpcClientTest GetNewInstance failed");
		goto Leave;
	}

	if (!m_Client->Start(SEC_LAB_SERVER_PORT_NAME))
	{
		ESL_DBG_OUT(DBG_ERROR, "LpcClientTest Start failed");
		goto Leave;
	}

	ESL_DBG_OUT(DBG_INFO, "LpcClientTest client started");

	RetVal = TRUE;
	m_Inited = TRUE;
Leave:
	if (!RetVal)
	{
		Stop();
	}
	return TRUE;
}

VOID SendMessages::Stop()
{
	if (m_Inited)
	{
		CLpcClientLibInt::FreeInstance(m_Client);
		m_Inited = FALSE;
		m_Client = NULL;
	}
}

BOOL SendMessages::SendMessage(
	KernelMessagesToUser MesageType,
	PMessagesToUser Message,
	BOOLEAN HasResponse
)
{
	Message->MesageType = MesageType;
	if(!NT_SUCCESS (m_Client->SendMessageWaitResponse(
		(PLPC_BASIC_MESSAGE)Message,
		HasResponse
	)))
	{
		return FALSE;
	}

	return TRUE;
}

