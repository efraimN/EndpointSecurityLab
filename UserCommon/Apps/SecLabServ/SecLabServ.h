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

#include <IServiceLib.h>

class SecLabService : public IServiceLogic
{
public:

	SecLabService(const SecLabService& other) = delete;
	SecLabService& operator=(const SecLabService& other) = delete;

	virtual BOOL ServiceInitLogic(IServiceLib* ServiceLib);
	virtual BOOL MainServiceLoop(PDWORD WaitInterval, IServiceLib* ServiceLib);
	virtual int ServiceExitLogic(IServiceLib* ServiceLib);

	virtual BOOL ServiceControlHandler(
		PDWORD RetVal,
		DWORD dwControl,
		DWORD dwEventType,
		LPVOID lpEventData
	);

	SecLabService() {};
	virtual ~SecLabService() {};

private:
};

