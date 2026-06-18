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

#include "Precompiled.h"
#include <WppIncludes.h>
#include <BasicMiniFilterLib.h>

BasicMiniFilter::BasicMiniFilter()
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};
	PROC_ENTRY;
	m_Inited = FALSE;
}

BasicMiniFilter::~BasicMiniFilter()
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};
	PROC_ENTRY;
}

IBasicMiniFilter* IBasicMiniFilter::GetInstance()
{
	static BasicMiniFilter Singletone;
	return &Singletone;
}

NTSTATUS
BasicMiniFilter::Start(
	const FLT_OPERATION_REGISTRATION CallBacks[],
	const FLT_CONTEXT_REGISTRATION ContextRegistration[],
	PFLT_INSTANCE_SETUP_CALLBACK InstanceSetupCallback,
	PDRIVER_OBJECT DriverObject
)
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};
	PROC_ENTRY;

	NTSTATUS status = STATUS_UNSUCCESSFUL;
	FLT_REGISTRATION FilterRegistration;

	if (m_Inited)
	{
		ESL_DBG_OUT(DBG_ERROR, ("Ouch... :-( already inited returning STATUS_UNSUCCESSFUL"));
		return status;
	}

	memset(&FilterRegistration, 0, sizeof(FLT_REGISTRATION));

	FilterRegistration.Size = sizeof(FLT_REGISTRATION);
	FilterRegistration.Version = FLT_REGISTRATION_VERSION;
	FilterRegistration.Flags = 0;
	FilterRegistration.ContextRegistration = ContextRegistration;
	FilterRegistration.OperationRegistration = CallBacks;
	FilterRegistration.FilterUnloadCallback = UnloadCallback;
	FilterRegistration.InstanceSetupCallback = InstanceSetupCallback;

#if 0 // No need to initialize; all this is zero for now....
	FilterRegistration.InstanceQueryTeardownCallback;
	FilterRegistration.InstanceTeardownStartCallback;
	FilterRegistration.InstanceTeardownCompleteCallback;
	FilterRegistration.GenerateFileNameCallback;
	FilterRegistration.NormalizeNameComponentCallback;
	FilterRegistration.NormalizeContextCleanupCallback;
#if FLT_MGR_LONGHORN
	FilterRegistration.TransactionNotificationCallback;
	FilterRegistration.NormalizeNameComponentExCallback;
#endif // FLT_MGR_LONGHORN

#if FLT_MGR_WIN8
	FilterRegistration.SectionNotificationCallback;
#endif // FLT_MGR_WIN8
#endif

	m_DriverObject = DriverObject;
	m_DriverUnload = DriverObject->DriverUnload;
	m_DriverObject->DriverUnload = NULL; // since the FltRegisterFilter needs it as null

	status = FltRegisterFilter(
		m_DriverObject,
		&FilterRegistration,
		&m_FilterHandle);
	if (!NT_SUCCESS(status))
	{
		ESL_DBG_OUT(DBG_ERROR, "FltRegisterFilter failed 0x%08X", status);
		m_DriverObject->DriverUnload = m_DriverUnload;
		return status;
	}

	m_Inited = TRUE;
	return status;
}

NTSTATUS BasicMiniFilter::Stop()
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};
	PROC_ENTRY;

	if (!m_Inited)
	{
		ESL_DBG_OUT(DBG_ERROR, "Ouch... :-( FilterStop called but not inited returning STATUS_UNSUCCESSFUL");
		return STATUS_UNSUCCESSFUL;
	}
	FltUnregisterFilter(m_FilterHandle);

	m_Inited = FALSE;
	return STATUS_SUCCESS;
}

NTSTATUS BasicMiniFilter::StartFiltering()
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};
	PROC_ENTRY;

	NTSTATUS status = STATUS_UNSUCCESSFUL;

	if (!m_Inited)
	{
		return status;
	}

	status = FltStartFiltering(m_FilterHandle);
	if (!NT_SUCCESS(status))
	{
		ESL_DBG_OUT(DBG_ERROR, "FltStartFiltering failed 0x%08X", status);
		Stop();
	}
	return status;
}


PFLT_FILTER  BasicMiniFilter::GetFilterHandle()
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};
	PROC_ENTRY;

	PFLT_FILTER RetVal = NULL;
	if (!m_Inited)
	{
		ESL_DBG_OUT(DBG_ERROR, "Ouch... :-( GetFilterHandle called but not inited returning NULL");
		goto Leave;
	}

	RetVal = m_FilterHandle;

Leave:
	return RetVal;
}

NTSTATUS
FLTAPI BasicMiniFilter::UnloadCallback(
	FLT_FILTER_UNLOAD_FLAGS Flags
)
{
	NTSTATUS RetVal;
	BasicMiniFilter* This;
	This = (BasicMiniFilter*)IBasicMiniFilter::GetInstance();

	if (!(Flags & FLTFL_FILTER_UNLOAD_MANDATORY))
	{
		return STATUS_FLT_DO_NOT_DETACH;
	}

	if (This->m_DriverUnload)
	{
		/* After this the CMiniFilter class does not exist anymore, since CrtExit has been called */
		/* Also WPP has being stopped */
		This->m_DriverUnload(This->m_DriverObject);
	}

	RetVal = STATUS_SUCCESS;

	goto Leave;
Leave:
	return RetVal;
}

