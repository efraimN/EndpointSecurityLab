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
#include <SendToServiceCommon.h>

class ISendToService
{
public:

    static ISendToService* GetInstance();

	virtual BOOL Start() = 0;
	virtual VOID Stop() = 0;

	virtual
	BOOL
	SendMessage(
		KernelMessagesToUser MesageType,
		PMessagesToUser Message,
		BOOLEAN HasResponse
	) = 0;

protected:
	ISendToService() {};
	~ISendToService() {};
private:
    // delete copy and move constructors and assign operators
	ISendToService(ISendToService const&);             // Copy construct
	ISendToService(ISendToService&&);                  // Move construct
	ISendToService& operator=(ISendToService const&);  // Copy assign
	ISendToService& operator=(ISendToService&&);       // Move assign
};

