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


/*
Multi-client support notes:

Keep a list of active PClientContext objects. instead of only one m_pClientContext
Add a client context to the list only after ZwCompleteConnectPort succeeds.
Remove the client context from the list on LPC_PORT_CLOSED, then close CommPortHandle and delete the context.
Stop() and RecoverLpcPort() must walk the active client-context list and close/free all remaining contexts, because LPC_PORT_CLOSED may not be received during shutdown or recovery.
Optionally allow AcceptConnect to initialize a server-defined per-client context, for example ClientContext->UserContext, if the server user needs to associate state with each client.


*/


#include "Precompiled.h"
#include <WppIncludes.h>

#include "LpcServLib.h"

using namespace  UserKernelUtilsLib;

#include <process.h>
#include <intrin.h>

ILpcServLib* ILpcServLib::GetNewInstance(BOOLEAN Use64bitstructs)
{
	return (ILpcServLib*) new CLpcServLibU(Use64bitstructs);
}

void ILpcServLib::FreeInstance(ILpcServLib* Instance)
{
	delete (CLpcServLibU*) Instance;
}


CLpcServLibU::CLpcServLibU(BOOLEAN Use64bitstructs)
{
	m_Use64bitstructs = Use64bitstructs;
	m_hPortLoop = NULL;
	m_ConnectionHandle = NULL;
	m_pClientContext = NULL;
}

CLpcServLibU::~CLpcServLibU()
{
}


BOOL CLpcServLibU::InitLpcPort(PCWCHAR PortName, PHANDLE PortHandle)
{
    NTSTATUS status;
    BOOL RetVal = FALSE;
    UNICODE_STRING UPortName;
    OBJECT_ATTRIBUTES ObjectAttributes;

    RtlInitUnicodeString(&UPortName, PortName);

    InitializeObjectAttributes(
        &ObjectAttributes,
        &UPortName,
        OBJ_CASE_INSENSITIVE,
        0,
        NULL
    );

    status = ZwCreateWaitablePort(
        PortHandle,
        &ObjectAttributes,
        0,
        m_MaxMessageSize,
        0);

    if (!NT_SUCCESS(status))
    {
        RtlSetLastWin32ErrorAndNtStatusFromNtStatus(status);
        ESL_DBG_OUT(DBG_ERROR, "ZwCreateWaitablePort Failed MaxMessageSize = 0x%X port = %S status = 0x%08X", m_MaxMessageSize, UPortName.Buffer, status);
        goto Leave;
    }

    if (status == STATUS_OBJECT_NAME_EXISTS)
    {
        RtlSetLastWin32Error(ERROR_ALREADY_EXISTS);
        ESL_DBG_OUT(DBG_ERROR, "ZwCreateWaitablePort Failed status = 0x%08X", status);
        goto Leave;
    }

	RetVal = TRUE;
Leave:
    return RetVal;
}

VOID CLpcServLibU::Stop()
{
	m_ExitFlag = TRUE;

	if (m_hPortLoop)
	{
		WaitForSingleObject(m_hPortLoop, INFINITE);
		ZwClose(m_hPortLoop);
		m_hPortLoop = NULL;
	}

	if (m_ConnectionHandle)
	{
		ZwClose(m_ConnectionHandle);
		m_ConnectionHandle = NULL;
	}

	if (m_pClientContext)
	{
		ZwClose(m_pClientContext->CommPortHandle);
		delete m_pClientContext;
		m_pClientContext = NULL;
	}
}

BOOL CLpcServLibU::Start(
    ILpcServReceiverHandler* LpcReceiverCallBacks,
    USHORT MessageSize,
    PCWCHAR PortName
)
{
    BOOL RetVal = FALSE;

    m_MaxMessageSize = MessageSize;
    m_LpcReceiverCallBacks = LpcReceiverCallBacks;

	if (!InitLpcPort(PortName, &m_ConnectionHandle))
	{
		goto Leave;
	}
  
    m_ExitFlag = FALSE;

    m_hPortLoop = (HANDLE)_beginthreadex(NULL, 0, CLpcServLibU::LpcServLoop, this, 0, NULL);
    if (!m_hPortLoop)
    {
        ESL_DBG_OUT(DBG_ERROR, "_beginthreadex Failed");
        goto Leave;
    }

    RtlSetLastWin32Error(ERROR_SUCCESS);
    RetVal = TRUE;

Leave:
    if (!RetVal)
    {
        Stop();
    }
    return RetVal;
}

unsigned WINAPI CLpcServLibU::LpcServLoop(void *Param)
{
	CLpcServLibU *This = (CLpcServLibU*)Param;
	This->LpcServLoop();

	return 0;
}

VOID CLpcServLibU::RecoverLpcPort()
{
}

VOID CLpcServLibU::LpcServLoop()
{
	NTSTATUS status;
	PVOID Context;
	DWORD dwMilliseconds;
	LARGE_INTEGER Time;
	BOOLEAN Accept;
	char * Buff = NULL;
	PPORT_MESSAGE Message = NULL;
	DWORD *ConnectionInformation;
	PUSHORT MessageType;

	for (;;)
	{
		dwMilliseconds = 1000;
		Time.QuadPart = __emul(dwMilliseconds, -10000);

		if (Buff)
		{
			delete [] Buff;
			Buff = NULL;
		}

		if (m_ExitFlag)
		{
			ESL_DBG_OUT(DBG_INFO, "Got thread exit signal aborting LpcServLoop");
			goto Leave;
		}

		Buff = new char[m_MaxMessageSize];
		if (!Buff)
		{
			ESL_DBG_OUT(DBG_ERROR, "Not Enough memory aborting LpcServLoop");
            goto Leave;
        }
        RtlZeroMemory(Buff, m_MaxMessageSize);

		Message = (PPORT_MESSAGE)Buff;

		status = ZwReplyWaitReceivePortEx(
			m_ConnectionHandle,
			&Context,
			NULL,			//ReplyMessage
			Message,		//ReceiveMessage
			&Time
		);
        if ((status == STATUS_TIMEOUT))
        {
            continue;
        }
        if (!NT_SUCCESS(status))
        {
            ESL_DBG_OUT(DBG_ERROR, "ZwReplyWaitReceivePortEx failure status 0x%X possible client failure", status);
			continue;
		}

		if (m_Use64bitstructs)
		{
			MessageType = &((PLPC_BASIC_MESSAGE64)Message)->MessageHeader.MessageType;
			ConnectionInformation = &((PCONNECT_MESSAGE64)Message)->ConnectionInformation;
        }
		else
		{
			MessageType = &((PLPC_BASIC_MESSAGE32)Message)->MessageHeader.MessageType;
			ConnectionInformation = &((PCONNECT_MESSAGE32)Message)->ConnectionInformation;
        }
 
        *MessageType &= (USHORT)0x0ff;
        
        switch (*MessageType)
		{
			case LPC_CONNECTION_REQUEST:
			{
				HANDLE CommPortHandle;
				PClientContext pClientContext;
				CommPortHandle = NULL;
				if (m_pClientContext)
				{
					ESL_DBG_OUT(DBG_ERROR, "Already conected to client");
					Accept = FALSE;
				}
				else
				{
					if (m_LpcReceiverCallBacks)
					{
						Accept = m_LpcReceiverCallBacks->AcceptConnect((PLPC_BASIC_MESSAGE)Message);
					}
					else
					{
						ESL_DBG_OUT(DBG_ERROR, "No LPC Callback");
						Accept = FALSE;
					}
					if (*ConnectionInformation != 0xDEADBEEF)
					{
						ESL_DBG_OUT(DBG_ERROR, "ConnectMessage received Data is not DEADBEEF 0x%X failing connection", (UINT)*ConnectionInformation);
						Accept = FALSE;
					}
				}
				if (!Accept)
				{
					ESL_DBG_OUT(DBG_ERROR, "ZwReplyWaitReceivePortEx Going to fail the LPC connection");
				}

				pClientContext = new CientContext();

				*ConnectionInformation = 0xCAFEDEAD;

				status = ZwAcceptConnectPort(
					&CommPortHandle,
					pClientContext, //Context
					Message,
					Accept,
					NULL,		//ServerView,
					NULL		//ClientView
				);
				if (!NT_SUCCESS(status))
				{
					ESL_DBG_OUT(DBG_ERROR, "ZwAcceptConnectPort Failed status = 0x%08X", status);
					delete pClientContext;
					break;
				}
				if (!CommPortHandle)
				{
					delete pClientContext;
					break;
				}
				if (!Accept)
				{
					ZwClose(CommPortHandle);
					delete pClientContext;
					break;
				}
				pClientContext->CommPortHandle = CommPortHandle;
				status = ZwCompleteConnectPort(CommPortHandle);
				if (!NT_SUCCESS(status))
				{
					ESL_DBG_OUT(DBG_ERROR, "ZwCompleteConnectPort Failed status = 0x%08X", status);
					ZwClose(CommPortHandle);
					delete pClientContext;
					break;
				}
				m_pClientContext = pClientContext;
			}
			break;
			case LPC_PORT_CLOSED:
			{
				if (Context)
				{
					PClientContext pClientContext = (PClientContext)Context;
					ZwClose(pClientContext->CommPortHandle);
					delete pClientContext;
					m_pClientContext = NULL;
				}
			}
			break;
			case LPC_DATAGRAM:
			case LPC_REQUEST:
			{
				if (Message)
				{
					if (m_LpcReceiverCallBacks) m_LpcReceiverCallBacks->HandleDataRequest((PLPC_BASIC_MESSAGE)Message);
					if (*MessageType == LPC_REQUEST)
					{
						status = ZwReplyPort(((PClientContext)Context)->CommPortHandle, Message);
						if (!NT_SUCCESS(status))
						{
							ESL_DBG_OUT(DBG_ERROR, "ZwReplyPort Failed status = 0x%08X", status);
							break;
						}
					}
				}
			}
			break;
		}
	}

Leave:

    if (m_ConnectionHandle)
	{
		ZwClose(m_ConnectionHandle);
		m_ConnectionHandle = NULL;
	}
	if (Buff)
	{
		delete [] Buff;
		Buff = NULL;
	}

	RtlSetLastWin32Error(ERROR_SUCCESS);
}
