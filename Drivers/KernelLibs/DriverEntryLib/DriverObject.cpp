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

// #include <IUserKernelUtils.h>
// using namespace UserKernelUtilsLib;


#include "DriverObject.h"

#include <KernelUtilsLib.h>

DriverObjectImp::DriverObjectImp()
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};
	PROC_ENTRY;

	m_hFile = NULL;
}

DriverObjectImp::~DriverObjectImp()
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

DriverObjectImp* DriverObjectImp::GetInstance()
{
	static DriverObjectImp SingleTone;
	return &SingleTone;
}


NTSTATUS
DriverObjectImp::MajorFunctionDispatcher__(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	return  GetInstance()->m_This->MajorFunctionDispatcher(DeviceObject, Irp);
}

NTSTATUS
DriverObjectImp::DriverObjectMain(
	IN PUNICODE_STRING RegistryPath,
	IN IDriverObject*DriverObjectInstance
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
	NTSTATUS RetVal = STATUS_INSUFFICIENT_RESOURCES;
	
	m_This = DriverObjectInstance;

	RetVal = KernelUtilsLib::IRegistryUtil::InitDriverRegPath(RegistryPath);
	if (!NT_SUCCESS(RetVal))
	{
		ESL_DBG_OUT(DBG_ERROR, "Failed InitDriverRegPath 0x%08X", RetVal);
		goto Leave;
	}
	RetVal = STATUS_INSUFFICIENT_RESOURCES;

	ESL_DBG_OUT(DBG_INFO, "Registry path = %S", KernelUtilsLib::IRegistryUtil::m_RegistryPath.Buffer);
	ESL_DBG_OUT(DBG_INFO, "CurrentControlSetPath = %S", KernelUtilsLib::IRegistryUtil::m_CurrentControlSetPath.Buffer);
	ESL_DBG_OUT(DBG_INFO, "Driver Image path is %S", KernelUtilsLib::IRegistryUtil::m_DriverImageFileName.Buffer);


	for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		DriverEntryLib ::G_DriverObject->MajorFunction[i] = MajorFunctionDispatcher__;
	}

	RetVal = m_This->DoStartStop(TRUE);

Leave:
	ESL_DBG_OUT(DBG_INFO, "Return status =  0x%08X", RetVal);

	return RetVal;
}

void DriverObjectImp::DriverExit()
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};
	PROC_ENTRY;

	ESL_DBG_OUT(DBG_INFO, "Starting the de-init %!FUNC!");

	m_This->DoStartStop(FALSE);

	KernelUtilsLib::IRegistryUtil::ReleaseDriverRegPath();

	if (m_hFile)
	{
		ZwClose(m_hFile);
	}

	return;
}
