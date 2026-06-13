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

#include "LpcClientLib.h"


CLpcClientLibInt* CLpcClientLibInt::GetNewInstance(BOOLEAN Use64bitstructs)
{
	return new CLpcClientLibU(Use64bitstructs);
}

void CLpcClientLibInt::FreeInstance(CLpcClientLibInt* Instance)
{
	delete (CLpcClientLibU*)Instance;
}

CLpcClientLibU::CLpcClientLibU(BOOLEAN Use64bitstructs)
{
	m_Use64bitstructs = Use64bitstructs;
	m_PortHandle = NULL;
	m_PortMessageSize = 0;
}

CLpcClientLibU::~CLpcClientLibU()
{

}

BOOL CLpcClientLibU::Start(
	PCWCHAR PortName,
	USHORT PortMessageSize
)
{
	BOOL RetVal = FALSE;
	NTSTATUS status;
	UNICODE_STRING UPortName;
	DWORD ConnectionInformation = 0xDEADBEEF;
	ULONG ConnectionInformationLength = sizeof(DWORD);
	DWORD MaxPortMessageSize;

	static SECURITY_QUALITY_OF_SERVICE DefaultSQos =
	{
		sizeof(SECURITY_QUALITY_OF_SERVICE),
		SecurityImpersonation,
		SECURITY_DYNAMIC_TRACKING,
		FALSE
	};

	RtlInitUnicodeString(&UPortName, PortName);
	status = ZwConnectPort(
		&m_PortHandle,
		&UPortName,
		&DefaultSQos,
		NULL,
		NULL,
		&MaxPortMessageSize,
		&ConnectionInformation,
		&ConnectionInformationLength
	);
	if (!NT_SUCCESS(status))
	{
		if (STATUS_OBJECT_NAME_NOT_FOUND == status)
		{
			ESL_DBG_OUT(DBG_ERROR, "ZwConnectPort failed STATUS_OBJECT_NAME_NOT_FOUND");
		}
		else
		{
            ESL_DBG_OUT(DBG_ERROR, "ZwConnectPort failed 0x%08X", status);
		}
		goto Leave;
	}
	if ((!ConnectionInformation) || (0xCAFEDEAD != ConnectionInformation))
	{
        ESL_DBG_OUT(DBG_ERROR, "Returned bad data closing");
		goto Leave;
	}
	if (PortMessageSize > MaxPortMessageSize)
	{
        ESL_DBG_OUT(DBG_ERROR, "PortMessageSize > MaxPortMessageSize closing");
		goto Leave;
	}

	m_PortMessageSize = PortMessageSize;
    RetVal = TRUE;

Leave:
	if (!RetVal)
	{
		Stop();
	}
	return RetVal;
}

NTSTATUS CLpcClientLibU::SendMessageWaitResponse(
	PLPC_BASIC_MESSAGE LpcMessage,
	BOOLEAN WaitResponse
)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	// STATUS_PORT_DISCONNECTED
	// STATUS_PORT_CLOSED
	// STATUS_LPC_REPLY_LOST
	if (m_Use64bitstructs)
	{
		PLPC_BASIC_MESSAGE64 _LpcMessage;
		_LpcMessage = (PLPC_BASIC_MESSAGE64)LpcMessage;

		_LpcMessage->MessageHeader.MessageType = LPC_NEW_MESSAGE;
		_LpcMessage->MessageHeader.MessageSize = m_PortMessageSize;
		_LpcMessage->MessageHeader.DataSize = (USHORT)(_LpcMessage->MessageHeader.MessageSize - sizeof(PORT_MESSAGE64));
	}
	else
	{
		PLPC_BASIC_MESSAGE32 _LpcMessage;
		_LpcMessage = (PLPC_BASIC_MESSAGE32)LpcMessage;

		_LpcMessage->MessageHeader.MessageType = LPC_NEW_MESSAGE;
		_LpcMessage->MessageHeader.MessageSize = m_PortMessageSize;
		_LpcMessage->MessageHeader.DataSize = (USHORT)(_LpcMessage->MessageHeader.MessageSize - sizeof(PORT_MESSAGE32));
	}


    if (WaitResponse)
    {
        status = ZwRequestWaitReplyPort(
            m_PortHandle,
            (PPORT_MESSAGE)LpcMessage,
            (PPORT_MESSAGE)LpcMessage
        );
    }
    else
    {
        status = ZwRequestPort(
            m_PortHandle,
            (PPORT_MESSAGE)LpcMessage
        );
    }

	return status;
}

VOID CLpcClientLibU::Stop()
{
	if (m_PortHandle)
	{
		ZwClose(m_PortHandle);
		m_PortHandle = NULL;
	}
}
