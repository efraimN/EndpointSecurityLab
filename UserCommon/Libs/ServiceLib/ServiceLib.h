#pragma once

#include <IServiceLib.h>

class ServiceLibImp : public IServiceLib
{
public:
	friend IServiceLib;
	ServiceLibImp(const ServiceLibImp& other) = delete;
	ServiceLibImp& operator=(const ServiceLibImp& other) = delete;

	virtual BOOL Start(IServiceLogic* ServiceLogic);

	virtual VOID Stop();

	/* Will not return until the service is stopped */
	virtual DWORD RunServiceLogic(BOOL Service_Process);

	virtual void SetAllowStop(BOOL ShouldAllow);

	virtual void ReportServiceStatus(DWORD CurrentState, DWORD ExitCode, DWORD WaitHint);

private:
	ServiceLibImp();
	~ServiceLibImp();

	static
	VOID WINAPI ServiceMains(DWORD dwArgc, LPTSTR* lpszArgv);

	virtual VOID ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);


	static
	DWORD WINAPI ServiceControlHandlerFunctionEx(
		DWORD dwControl,
		DWORD dwEventType,
		LPVOID lpEventData,
		LPVOID lpContext
	);

	virtual
	DWORD ServiceControlHandlerFunctionEx(
		DWORD dwControl,
		DWORD dwEventType,
		LPVOID lpEventData
	);


private:
	BOOL m_Inited = FALSE;

	IServiceLogic* m_ServiceLogic; 

	SERVICE_STATUS_HANDLE m_StatusHandle;
	SERVICE_STATUS m_ServiceStatus;

	HANDLE m_ServiceStopEvent  = NULL;

	DWORD m_ExitCode = 0;
	
	BOOL m_Service_Process = TRUE;

};
