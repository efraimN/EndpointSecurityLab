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

#include <ILpcLibServ.h>
#include <ILinkListLibInt.h>

class CLpcServLibU : public ILpcServLib
{
public:

	friend class ILpcServLib;

    virtual
	BOOL Start(
		ILpcServReceiverHandler *LpcReceiverCallBacks,
		USHORT MessageSize,
		PCWCHAR PortName
    );

 	virtual VOID Stop();

private:
    CLpcServLibU(BOOLEAN Use64bitstructs);
    virtual ~CLpcServLibU();

	static
    unsigned 
    WINAPI 
    LpcServLoop(
        void *Param
    );

    virtual
    BOOL 
    InitLpcPort(
        PCWCHAR PortName, 
        PHANDLE PortHandle
    );
 
	virtual VOID LpcServLoop();

    virtual VOID RecoverLpcPort();

	HANDLE m_hPortLoop; 
	ILpcServReceiverHandler * m_LpcReceiverCallBacks = NULL;
	HANDLE m_ConnectionHandle;
	BOOL m_ExitFlag = FALSE;
	USHORT m_MaxMessageSize;
	BOOLEAN m_Use64bitstructs = TRUE;

    PClientContext m_pClientContext;

};

