#pragma once

#include <IServiceLib.h>

class SecLabService : public IServiceLogic
{
public:

	SecLabService(const SecLabService& other) = delete;
	SecLabService& operator=(const SecLabService& other) = delete;

	virtual BOOL ServiceInitLogic(IServiceLib* ServiceLib);
	virtual BOOL MainServiceLoop(PDWORD WaitInterval, IServiceLib* ServiceLib);
	virtual DWORD ServiceExitLogic(IServiceLib* ServiceLib);

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

