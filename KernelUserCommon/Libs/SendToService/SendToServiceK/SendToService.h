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

#pragma once

#include <ISendToService.h>
#include <ILpcLibClient.h>

class SendToService : public ISendToService
{
public:

	friend ISendToService;

	virtual BOOL Start();
	virtual VOID Stop();

	virtual ULONG_PTR GetServerPid();

	virtual
	BOOL
	SendMessage(
		KernelMessagesToUser MesageType,
		PMessagesToUser Message,
		BOOLEAN HasResponse
	);

private:
	ILpcClientLib* m_Client;
	BOOL m_Inited;

	// delete copy and move constructors and assign operators
	SendToService(SendToService const&);             // Copy construct
	SendToService(SendToService&&);                  // Move construct
	SendToService& operator=(SendToService const&);  // Copy assign
	SendToService& operator=(SendToService&&);       // Move assign
	SendToService();
	~SendToService();
};
