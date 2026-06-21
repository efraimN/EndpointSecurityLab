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

class ILpcClientLib
{
public:
    // delete copy and move constructors and assign operators
    ILpcClientLib(ILpcClientLib const&);             // Copy construct
    ILpcClientLib(ILpcClientLib&&);                  // Move construct
    ILpcClientLib& operator=(ILpcClientLib const&);  // Copy assign
    ILpcClientLib& operator=(ILpcClientLib&&);       // Move assign

    static ILpcClientLib* GetNewInstance(BOOLEAN Use64bitstructs);
	static void FreeInstance(ILpcClientLib* Instance);

	virtual BOOL Start(
		PCWCHAR PortName
	) = 0;

	virtual VOID Stop() = 0;

	virtual ULONG_PTR GetServerPid() = 0;

	virtual NTSTATUS SendMessageWaitResponse(
		PLPC_BASIC_MESSAGE LpcMessage,
		BOOLEAN WaitResponse
	) = 0;

protected:
	ILpcClientLib() {};
	~ILpcClientLib() {};
private:
};

