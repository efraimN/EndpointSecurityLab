#pragma once

class IServiceLib;

class IServiceLogic
{
public:
	IServiceLogic(const IServiceLogic& other) = delete;
	IServiceLogic& operator=(const IServiceLogic& other) = delete;

	/*returning FALSE will abort and stop the service*/
	virtual BOOL ServiceInitLogic(IServiceLib* ServiceLib) = 0;

	/*returning FALSE will stop the service*/
	/*returning TRUE will be run again and again*/
	virtual BOOL MainServiceLoop(PDWORD WaitInterval, IServiceLib* ServiceLib) = 0;

	/* the return value will be returned by the service and the process*/
	virtual DWORD ServiceExitLogic(IServiceLib* ServiceLib) = 0;

	// if return TRUE the default handler wont be called
	virtual BOOL ServiceControlHandler(
		PDWORD RetVal,
		DWORD dwControl,
		DWORD dwEventType,
		LPVOID lpEventData
		) = 0;

protected:
	IServiceLogic() {};
	~IServiceLogic() {};

private:

};

class IServiceLib
{
public:

	IServiceLib(const IServiceLib& other) = delete;
	IServiceLib& operator=(const IServiceLib& other) = delete;

	static IServiceLib* GetInstance();

	virtual BOOL Start(IServiceLogic* ServiceLogic) = 0;

	virtual VOID Stop() = 0;

	/* 
		if Service_Process = TRUE not return until the service is stopped 
		or MainServiceLoop returns FALSE

		if Service_Process = FALSE will return when MainServiceLoop returns FALSE
	*/
	virtual DWORD RunServiceLogic(BOOL Service_Process) = 0;

	//
	virtual void SetAllowStop(BOOL SshouldAllow) = 0;

	virtual void ReportServiceStatus(DWORD CurrentState, DWORD ExitCode, DWORD WaitHint) = 0;

protected:
	IServiceLib() {};
	~IServiceLib() {};

private:
};
