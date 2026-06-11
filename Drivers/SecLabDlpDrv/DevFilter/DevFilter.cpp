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

#include "DevFilter.h"

void* __cdecl DevFilter::operator new(size_t nSize, PDRIVER_OBJECT DriverObject, DEVICE_TYPE Type)
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};
	PROC_ENTRY;

	PVOID RetVal = NULL;
	DevFilter* pDevFilter = NULL;

	pDevFilter = (DevFilter*)BasicDriverDevice::operator new(nSize, DriverObject, FALSE, NULL, Type);
	if (!pDevFilter)
	{
		goto Leave;
	}

	pDevFilter->m_PDO = 0;
	pDevFilter->m_TopOfStackBeforeUs = 0;

	RetVal = pDevFilter;
Leave:
	if (!RetVal)
	{
		if (pDevFilter)
		{
			BasicDriverDevice::operator delete(pDevFilter);
		}
	}

	return RetVal;
}

void __cdecl DevFilter::operator delete(void* p)
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};
	PROC_ENTRY;

	BasicDriverDevice::operator delete(p);
}

DevFilter::DevFilter(UINT32 Signature, PWCHAR HardwareID, PDEVICE_OBJECT PDO) : BasicDriverDevice(Signature)
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};
	PROC_ENTRY;

	m_HardwareID = HardwareID;
	m_PDO = PDO;
}

DevFilter::~DevFilter()
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};
	PROC_ENTRY;

	if (m_HardwareID)
	{
		delete[] m_HardwareID;
	}
}

// For now this will block any device
static
BOOL IsDeviceOnList(PWCHAR /*HardwareID*/, ULONG /*HardwareIDSize*/, BOOL* LearnMode)
{
	*LearnMode = FALSE;
	return TRUE;
}

BOOL DevFilter::AttachToDevice(IN PDEVICE_OBJECT PDO, PWCHAR HardwareID, BOOL LearnMode)
{
	BOOL RetVal = FALSE;
	PDEVICE_OBJECT	TopDeviceObject = NULL;
	ULONG DeviceType = FILE_DEVICE_UNKNOWN;
	DevFilter* pDevFilter;
	{
		TopDeviceObject = IoGetAttachedDeviceReference(PDO);
		DeviceType = TopDeviceObject->DeviceType;
		ObDereferenceObject(TopDeviceObject);
	}

	pDevFilter = new(DriverEntryLib::G_DriverObject, DeviceType) DevFilter(MY_FILTER_DEVICES_SIGNATURE, HardwareID, PDO);
	if (!pDevFilter)
	{
		goto Leave;
	}

	pDevFilter->m_LearnMode = LearnMode;

	pDevFilter->m_TopOfStackBeforeUs = IoAttachDeviceToDeviceStack(pDevFilter->m_pDeviceObject, pDevFilter->m_PDO);
	if (!pDevFilter->m_TopOfStackBeforeUs)
	{
		delete pDevFilter;
		goto Leave;
	}
	pDevFilter->m_pDeviceObject->Flags |= (pDevFilter->m_TopOfStackBeforeUs->Flags & (DO_BUFFERED_IO | DO_DIRECT_IO | DO_POWER_PAGABLE));


	RetVal = TRUE;
Leave:
	if (RetVal)
	{
		pDevFilter->m_pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	}
	return RetVal;
}


NTSTATUS
DevFilter::AddDevice(
	IN PDRIVER_OBJECT,
	IN PDEVICE_OBJECT   PDO
)
{
	PWCHAR HardwareID = NULL;
	ULONG HardwareIDSize = 0;

	BOOL LearnMode = TRUE;

	PWCHAR ClassName = NULL;
	ULONG ClassNameSize = 0;

	ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "Inside function %!FUNC!" );

	HardwareID = GetDeviceInformation(PDO, DevicePropertyHardwareID, &HardwareIDSize);
	if (!HardwareID)
	{
		goto Leave;
	}

	ClassName = GetDeviceInformation(PDO, DevicePropertyClassName, &ClassNameSize);
	if (ClassName)
	{
		ESL_DBG_OUT(DBG_INFO, "GetDeviceInformation ClassName %S", ClassName);
	}

	if (!IsDeviceOnList(HardwareID, HardwareIDSize / 2, &LearnMode))
	{
		goto Leave;
	}

	ESL_DBG_OUT(DBG_INFO, "Device %S is on list and will %s blocked", HardwareID, LearnMode ? "be NOT" : "be");

// 	SendAttachDetachMessage(HardwareID, TRUE, LearnMode);

	if (!AttachToDevice(PDO, HardwareID, LearnMode))
	{
		goto Leave;
	}
	HardwareID = NULL; // if AttachToDevice succeeded then the deletion will be later on

Leave:
	if (ClassName)
	{
		delete[]ClassName;
	}
	if (HardwareID)
	{
		delete[]HardwareID;
	}
	return STATUS_SUCCESS;
}

NTSTATUS DevFilter::MajorFunctionDispatcher(IN PIRP Irp)
{
	PIO_STACK_LOCATION IoStackCurLoc;
	IoStackCurLoc = IoGetCurrentIrpStackLocation(Irp);
	switch (IoStackCurLoc->MajorFunction)
	{
		case IRP_MJ_PNP:
		{
			UCHAR MinorFunction = IoStackCurLoc->MinorFunction;

			ESL_DBG_OUT(DBG_INFO, "MajorFunctionDispatcher = IRP_MJ_PNP #0x%04X MinorFunction #0x%04X",
				IoStackCurLoc->MajorFunction,
				MinorFunction);

			return PnPDispatch(Irp, MinorFunction);
		}
		break;

		default:
		{
			ESL_DBG_OUT(DBG_INFO, "MajorFunction = 0x%04X MinorFunction = 0x%04X PID %d PrevMode = %s RequestorMode = %s CurrentLocation = %d",
				IoStackCurLoc->MajorFunction,
				IoStackCurLoc->MinorFunction,
				DOWNCASTHANDLE(PsGetProcessId(PsGetCurrentProcess())),
				(ExGetPreviousMode() == KernelMode) ? "KernelMode" : "UserMode",
				(Irp->RequestorMode == KernelMode) ? "KernelMode" : "UserMode",
				Irp->CurrentLocation
			);
		}
		break;
	}

	return Default_IRP_Dispatcher(Irp);
}

NTSTATUS DevFilter::StartDeviceCompletition(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);
	PKEVENT  Event;

	Event = (PKEVENT)Context;
	KeSetEvent(Event, 0, FALSE);
	return STATUS_MORE_PROCESSING_REQUIRED;
}


NTSTATUS
DevFilter::PnPDispatch(
	IN PIRP Irp,
	IN UCHAR MinorFunction
)
{
	NTSTATUS RetVal = STATUS_SUCCESS;
	switch (MinorFunction)
	{
		case IRP_MN_START_DEVICE:
		{

			KEVENT Event;

			KeInitializeEvent(
				&Event,
				NotificationEvent,
				FALSE
			);
			IoCopyCurrentIrpStackLocationToNext(Irp);
			IoSetCompletionRoutine(
				Irp,
				StartDeviceCompletition,
				&Event,
				TRUE,
				TRUE,
				TRUE
			);
			RetVal = IoCallDriver(m_TopOfStackBeforeUs, Irp);

			if (STATUS_PENDING == RetVal)
			{
				KeWaitForSingleObject(
					&Event,
					Executive, // Waiting for reason of a driver
					KernelMode, // Waiting in kernel mode
					FALSE, // No alert
					NULL); // No timeout
				RetVal = STATUS_SUCCESS;
			}

			if (NT_SUCCESS(RetVal) && NT_SUCCESS(Irp->IoStatus.Status))
			{
				if (!m_LearnMode)
				{
					RetVal = STATUS_ACCESS_DENIED;
				}
			}

			Irp->IoStatus.Status = RetVal;
			IoCompleteRequest(Irp, IO_NO_INCREMENT);
			ESL_DBG_OUT(DBG_INFO, "IRP_MN_START_DEVICE RetVal 0x%X", RetVal);
		}
		break;

		case IRP_MN_REMOVE_DEVICE:
		{
//		 	SendAttachDetachMessage(HardwareID, FALSE, LearnMode);
			Irp->IoStatus.Status = STATUS_SUCCESS;
			RetVal = Default_IRP_Dispatcher(Irp);

			IoDetachDevice(m_TopOfStackBeforeUs);

			ESL_DBG_OUT(DBG_INFO, "IRP_MN_REMOVE_DEVICE RetVal 0x%X", RetVal);
			delete this;
		}
		break;

		default:
		{
			ESL_DBG_OUT(DBG_INFO, "PnP MinorFunction = 0x%04X PID %d PrevMode = %s RequestorMode = %s CurrentLocation = %d",
				MinorFunction,
				DOWNCASTHANDLE(PsGetProcessId(PsGetCurrentProcess())),
				(ExGetPreviousMode() == KernelMode) ? "KernelMode" : "UserMode",
				(Irp->RequestorMode == KernelMode) ? "KernelMode" : "UserMode",
				Irp->CurrentLocation
			);
			RetVal = Default_IRP_Dispatcher(Irp);
		}
	}

	return RetVal;
}

NTSTATUS
DevFilter::Default_IRP_Dispatcher(
	IN PIRP Irp
)
{
	//
	// Pass the IRP to the target
	//
	IoSkipCurrentIrpStackLocation(Irp);

	return IoCallDriver(m_TopOfStackBeforeUs, Irp);
}

PWCHAR DevFilter::GetDeviceInformation(PDEVICE_OBJECT PDO, DEVICE_REGISTRY_PROPERTY Property, PULONG PropertyStringSize)
{
	PWCHAR HardwareID = NULL;
	ULONG Resl = 256;
	NTSTATUS status = STATUS_BUFFER_TOO_SMALL;
	PWCHAR RetVal = NULL;

	do
	{
		if (HardwareID)
		{
			delete[] HardwareID;
			HardwareID = NULL;
		}
		HardwareID = new ('oIdG') WCHAR[Resl / sizeof(WCHAR)];
		if (!HardwareID)
		{
			break;
		}
		status = IoGetDeviceProperty(
			PDO,
			Property,
			Resl,
			HardwareID,
			&Resl
		);
	} while (STATUS_BUFFER_TOO_SMALL == status);

	if (!NT_SUCCESS(status))
	{
		goto Leave;
	}
	*PropertyStringSize = Resl;
	RetVal = HardwareID;
Leave:
	if (!RetVal)
	{
		if (HardwareID)
		{
			delete[] HardwareID;
		}
	}

	return RetVal;
}
