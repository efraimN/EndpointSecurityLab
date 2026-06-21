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

#include "SendToService.h"


SendToService::SendToService()
{
	m_Client = NULL;
	m_Inited = FALSE;
}

SendToService::~SendToService()
{
}

ISendToService* ISendToService::GetInstance()
{
	static SendToService Singletone;
	return &Singletone;
}

BOOL SendToService::Start()
{
	BOOL RetVal = FALSE;
	m_Client = ILpcClientLib::GetNewInstance(TRUE);
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
Leave:
	m_Inited = TRUE;
	if (!RetVal)
	{
		Stop();
		m_Inited = FALSE;
	}
	return RetVal;
}

VOID SendToService::Stop()
{
	if (m_Inited)
	{
		if (m_Client)
		{
			m_Client->Stop();
			ILpcClientLib::FreeInstance(m_Client);
		}
		m_Inited = FALSE;
		m_Client = NULL;
	}
}

BOOL SendToService::SendMessage(
	KernelMessagesToUser MesageType,
	PMessagesToUser Message,
	BOOLEAN HasResponse
)
{
	if (!m_Inited)
	{
		return FALSE;
	}

	Message->UserMessage.MesageType = MesageType;
	if(!NT_SUCCESS (m_Client->SendMessageWaitResponse(
		(PLPC_BASIC_MESSAGE)Message,
		HasResponse
	)))
	{
		return FALSE;
	}

	return TRUE;
}

ULONG_PTR SendToService::GetServerPid()
{
	if (!m_Inited)
	{
		return 0;
	}
	return m_Client->GetServerPid();
}


