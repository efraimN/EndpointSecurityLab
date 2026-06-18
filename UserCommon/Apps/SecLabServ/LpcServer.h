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
#include <SendToServiceCommon.h>

class MyServerCallbacks : public ILpcServReceiverHandler
{
public:
	virtual BOOLEAN AcceptConnect(PLPC_BASIC_MESSAGE PortMessage);
	virtual VOID HandleDataRequest(PLPC_BASIC_MESSAGE PortMessage);
};


class MyLpcServer
{
public:
	BOOL Start();
	BOOL Stop();

	MyLpcServer();
	~MyLpcServer();

private:

};

