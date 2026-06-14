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

#pragma once

#include <ILpcLibClient.h>


class CLpcClientLibU : public CLpcClientLibInt
{
public:

	friend class CLpcClientLibInt;
	virtual BOOL Start(
		PCWCHAR PortName,
		USHORT PortMessageSize
	);

    virtual VOID Stop();

	virtual NTSTATUS SendMessageWaitResponse(
		PLPC_BASIC_MESSAGE LpcMessage,
		BOOLEAN WaitResponse
	);

protected:

private:
	CLpcClientLibU(BOOLEAN Use64bitstructs);
	virtual ~CLpcClientLibU();
    
	HANDLE m_PortHandle;
	USHORT m_PortMessageSize;
	BOOLEAN m_Use64bitstructs;
};

