#include "Precompiled.h"
#include <WppIncludes.h>

#include "ServiceLib.h"


ServiceLibImp::ServiceLibImp()
{

}

ServiceLibImp::~ServiceLibImp()
{

}

IServiceLib* IServiceLib::GetInstance()
{
	static
	ServiceLibImp SingleTone;

	return &SingleTone;
}

BOOL ServiceLibImp::Start(
	IServiceLogic* ServiceLogic
)
{
	OBJECT_ATTRIBUTES Oa;
	NTSTATUS status;

	if (m_Inited)
	{
		goto Leave;
	}
	
	m_ServiceLogic = ServiceLogic;

	InitializeObjectAttributes(&Oa, NULL, OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = ZwCreateEvent(
		&m_ServiceStopEvent,
		GENERIC_ALL,
		&Oa,
		NotificationEvent,
		FALSE);

	if (!NT_SUCCESS(status))
	{
		goto Leave;
	}

	m_Inited = TRUE;
Leave:
	return m_Inited;
}

VOID ServiceLibImp::Stop()
{
	if (!m_Inited)
	{
		goto Leave;
	}

	if (m_ServiceStopEvent)
	{
		ZwClose(m_ServiceStopEvent);
	}

	m_Inited = FALSE;
Leave:
	return;
}

DWORD ServiceLibImp::RunServiceLogic(BOOL Service_Process)
{
	m_Service_Process = Service_Process;
	auto RunAsProcess = [&]()
	{
		if (!m_ServiceLogic->ServiceInitLogic(this))
		{
			goto Leave;
		}

		for (;;)
		{
			DWORD WaitInterval = 0;
			if (!m_ServiceLogic->MainServiceLoop(&WaitInterval, this))
			{
				break;
			}
			if (WaitForSingleObjectEx(m_ServiceStopEvent, WaitInterval, FALSE) != WAIT_TIMEOUT)
			{
				break;
			}
		}

	Leave:
		m_ExitCode = m_ServiceLogic->ServiceExitLogic(this);

		return m_ExitCode;
	};

	if (!m_Service_Process)
	{
		return RunAsProcess();
	}

	if (!m_Inited)
	{
		goto Leave;
	}


	SERVICE_TABLE_ENTRYW ServiceTable[] = {
	{ L"", ServiceMains},
	{ NULL, NULL }
	};

	if (!StartServiceCtrlDispatcherW(ServiceTable))
	{
		m_ExitCode = GetLastError();
	}

Leave:
	return m_ExitCode;
}

void ServiceLibImp::SetAllowStop(BOOL ShouldAllow)
{
	if (ShouldAllow)
	{
		m_ServiceStatus.dwControlsAccepted |= SERVICE_ACCEPT_STOP;
	}
	else
	{
		m_ServiceStatus.dwControlsAccepted &= ~SERVICE_ACCEPT_STOP;
	}
	if (m_Service_Process)
	{
		SetServiceStatus(m_StatusHandle, &m_ServiceStatus);
	}
}

VOID WINAPI ServiceLibImp::ServiceMains(DWORD dwArgc, LPTSTR* lpszArgv)
{
	((ServiceLibImp*)IServiceLib::GetInstance())->ServiceMain(
		dwArgc, lpszArgv);
}

VOID ServiceLibImp::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	UNREFERENCED_PARAMETER(dwArgc);
	UNREFERENCED_PARAMETER(lpszArgv);
	DWORD WaitInterval = 0;

	m_StatusHandle = RegisterServiceCtrlHandlerExW (L"", ServiceControlHandlerFunctionEx, this);

	m_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	m_ServiceStatus.dwServiceSpecificExitCode = 0;

	ReportServiceStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	if (!m_ServiceLogic->ServiceInitLogic(this))
	{
		goto Leave;
	}

	ReportServiceStatus(SERVICE_RUNNING, NO_ERROR, 0);

	for (;;)
	{
		if (!m_ServiceLogic->MainServiceLoop(&WaitInterval, this))
		{
			break;
		}
		if (WaitForSingleObjectEx(m_ServiceStopEvent, WaitInterval, FALSE) != WAIT_TIMEOUT)
		{
			break;
		}
	}

Leave:
	m_ExitCode = m_ServiceLogic->ServiceExitLogic(this);

	ReportServiceStatus(SERVICE_STOPPED, m_ExitCode, 0);
}


void ServiceLibImp::ReportServiceStatus(DWORD CurrentState, DWORD ExitCode, DWORD WaitHint)
{
	static DWORD dwCheckPoint = 1;

	m_ServiceStatus.dwCurrentState = CurrentState;
	m_ServiceStatus.dwWin32ExitCode = ExitCode;
	m_ServiceStatus.dwWaitHint = WaitHint;

	if (CurrentState == SERVICE_START_PENDING)
	{
		m_ServiceStatus.dwControlsAccepted = 0;
	}

	if ((CurrentState == SERVICE_RUNNING) || (CurrentState == SERVICE_STOPPED))
	{
		m_ServiceStatus.dwCheckPoint = 0;
	}
	else
	{
		m_ServiceStatus.dwCheckPoint = dwCheckPoint++;
	}

	if (m_Service_Process)
	{
		// Report the status of the service to the SCM.
		SetServiceStatus(m_StatusHandle, &m_ServiceStatus);
	}
}

DWORD WINAPI ServiceLibImp::ServiceControlHandlerFunctionEx(
	DWORD dwControl,
	DWORD dwEventType,
	LPVOID lpEventData,
	LPVOID lpContext
)
{
	return ((ServiceLibImp*)lpContext)->ServiceControlHandlerFunctionEx(
		dwControl,
		dwEventType,
		lpEventData
		);
}

DWORD ServiceLibImp::ServiceControlHandlerFunctionEx(
	DWORD dwControl,
	DWORD dwEventType,
	LPVOID lpEventData
)
{
	DWORD RetVal = ERROR_CALL_NOT_IMPLEMENTED;
	LONG  PreviousEventState;

	if (m_ServiceLogic->ServiceControlHandler(
		&RetVal,
		dwControl,
		dwEventType,
		lpEventData
	))
	{
		return RetVal;
	}

	switch (dwControl)
	{
		case SERVICE_CONTROL_STOP:
		{
			ReportServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
			ZwSetEvent(m_ServiceStopEvent, &PreviousEventState);
			RetVal = NO_ERROR;
		}
		break;
		case SERVICE_CONTROL_SHUTDOWN:
		{
			ReportServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
			ZwSetEvent(m_ServiceStopEvent, &PreviousEventState);
			RetVal = NO_ERROR;
		}
		break;
	}

	return RetVal;
}
