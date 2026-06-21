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

#include <LpcLibCommonInc.h>

class ILpcServReceiverHandler
{
public:
	virtual VOID HandleDataRequest(PLPC_BASIC_MESSAGE PortMessage) = 0;
	virtual BOOLEAN AcceptConnect(PLPC_BASIC_MESSAGE PortMessage, BOOLEAN ClientAlreadyConnected) = 0;
};

class ILpcServLib
{
public:
    // delete copy and move constructors and assign operators
    ILpcServLib(ILpcServLib const&) = delete;             // Copy construct
    ILpcServLib(ILpcServLib&&) = delete;                  // Move construct
    ILpcServLib& operator=(ILpcServLib const&) = delete;  // Copy assign
    ILpcServLib& operator=(ILpcServLib&&) = delete;       // Move assign

	static ILpcServLib* GetNewInstance(BOOLEAN Use64bitstructs);
	static void FreeInstance(ILpcServLib* Instance);

	virtual BOOL Start(
		ILpcServReceiverHandler *LpcCallBacks,
		USHORT MessageSize,
		PCWCHAR PortName
	) = 0;

 	virtual VOID Stop() = 0;

protected:
	ILpcServLib() {};
	~ILpcServLib() {};
};

