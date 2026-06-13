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

#include <IRegUtils.h>

#include "DevFilterDrvHooks.h"
#include "DevFilterUtils.h"


DevFilterDrvHooks::DevFilterDrvHooks()
{
	m_DriverObject = NULL;
	m_DriverName[49] = 0;
	m_DriverName[0] = 0;
	m_MajorFunctionDispatcher = NULL;
}

DevFilterDrvHooks::~DevFilterDrvHooks()
{
}

BOOL Hook(PDRIVER_OBJECT /*DriverObject*/)
{
	return TRUE;
}

void Unhook()
{
}

BOOL DevFilterDrvHooks::Start()
{
	BOOL RetVal = FALSE;
	NTSTATUS status;
	WCHAR DriverObjectName[128];
	UNICODE_STRING usDriverObjectName;
	PDRIVER_OBJECT DriverObject = NULL;

	status = RtlStringCchPrintfW(
		DriverObjectName,
		ARRAYSIZE(DriverObjectName),
		L"\\Driver\\%ws",
		m_DriverName
	);

	if (!NT_SUCCESS(status))
	{
		goto Leave;
	}

	RtlInitUnicodeString(&usDriverObjectName, DriverObjectName);

	status = ObReferenceObjectByName(
		&usDriverObjectName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		0,
		*IoDriverObjectType,
		KernelMode,
		NULL,
		(PVOID*)&DriverObject
	);

	if (!NT_SUCCESS(status))
	{
		goto Leave;
	}

	if (!Hook(DriverObject))
	{
		goto Leave;
	}

	m_DriverObject = DriverObject;
	RetVal = TRUE;
Leave:
	if (!RetVal)
	{
		if (DriverObject)
		{
			ObDereferenceObject(DriverObject);
		}
	}
	return RetVal;
}

void DevFilterDrvHooks::Stop()
{
	if (!m_DriverObject)
	{
		return;
	}

	Unhook();

	ObDereferenceObject(m_DriverObject);
	m_DriverObject = NULL;
}

NTSTATUS
DevFilterDrvHooks::MajorFunctionDispatcher(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	return DevFilterDrvHooksManager::GetInstance()->
		FindByDeviceObject(DeviceObject)->_MajorFunctionDispatcher(DeviceObject, Irp);
}

NTSTATUS
DevFilterDrvHooks::_MajorFunctionDispatcher(IN PDEVICE_OBJECT /*DeviceObject*/, IN PIRP /*Irp*/)
{
	return 0;
}



//////////////////////////////////////////////////////////////////////////

DevFilterDrvHooksManager*
DevFilterDrvHooksManager::GetInstance()
{
	static DevFilterDrvHooksManager SingleTone;
	return &SingleTone;
}

DevFilterDrvHooksManager::DevFilterDrvHooksManager()
{
	m_DevFilterDrvHooksArray = NULL;
	m_NumberOfHooks = 0;
}

DevFilterDrvHooksManager::~DevFilterDrvHooksManager()
{
}

BOOL
DevFilterDrvHooksManager::Init(
	PWCHAR* ClassGuidArray
)
{
	BOOL RetVal;
	NTSTATUS status;
	PWCHAR DriverName;
	DevFilterUtils* Utils;
	PWCHAR* TmpGuidArray = NULL;

	RetVal = FALSE;
	status = STATUS_UNSUCCESSFUL;
	DriverName = NULL;
	Utils = DevFilterUtils::GetInstance();

	if (!Utils->Start())
	{
		goto Leave;
	}

	TmpGuidArray = ClassGuidArray;
	while (*TmpGuidArray)
	{
		status = Utils->GetDriversNamesByClassGuid(
			*TmpGuidArray
		);

		if (!NT_SUCCESS(status))
		{
			TmpGuidArray++;
			continue;
		}

		TmpGuidArray++;
	}

	m_NumberOfHooks = 0;
	// Get the Drivers count
	DriverName = Utils->GetFirstDriverName();

	while (DriverName)
	{
		m_NumberOfHooks++;
		DriverName = Utils->GetNextDriverName();
	}

	if (!m_NumberOfHooks)
	{
		goto Leave;
	}

	m_DevFilterDrvHooksArray = (DevFilterDrvHooks**)
#ifdef _NTDDK_
		new('nHfD')
#else
		new(std::nothrow)
#endif
		PVOID[m_NumberOfHooks];

	if (!m_DevFilterDrvHooksArray)
	{
		goto Leave;
	}

#ifndef _NTDDK_
	RtlZeroMemory(m_DevFilterDrvHooksArray, m_NumberOfHooks*sizeof(PVOID));
#endif

	DriverName = Utils->GetFirstDriverName();

	for (UINT i=0;i< m_NumberOfHooks;i++)
	{
		m_DevFilterDrvHooksArray[i] = 
#ifdef _NTDDK_
			new('xHfD')
#else
			new(std::nothrow)
#endif
			DevFilterDrvHooks();

		if (!m_DevFilterDrvHooksArray[i])
		{
			goto Leave;
		}
		RtlCopyMemory(m_DevFilterDrvHooksArray[i]->m_DriverName, DriverName, MIN(wcslen(DriverName)+1, 50)*sizeof(WCHAR));
		DriverName = Utils->GetNextDriverName();
	}

	RetVal = TRUE;

Leave:
	Utils->Stop();
	if (!RetVal)
	{
		Stop();
	}
	return RetVal;
}

void
DevFilterDrvHooksManager::Start()
{
	DWORD i;
	DevFilterDrvHooks* pDevFilterDrvHooks;

	pDevFilterDrvHooks = NULL;

	for (i = 0; i < m_NumberOfHooks; i++)
	{
		pDevFilterDrvHooks = m_DevFilterDrvHooksArray[i];

		if (!pDevFilterDrvHooks)
		{
			continue;
		}

		if (!pDevFilterDrvHooks->Start())
		{
			delete pDevFilterDrvHooks;
			m_DevFilterDrvHooksArray[i] = NULL;
		}
	}

	return;
}

void
DevFilterDrvHooksManager::Stop()
{
	DWORD i;
	DevFilterDrvHooks* pDevFilterDrvHooks;

	pDevFilterDrvHooks = NULL;

	if (!m_DevFilterDrvHooksArray)
	{
		goto Leave;
	}

	for (i = 0; i < m_NumberOfHooks; i++)
	{
		pDevFilterDrvHooks = m_DevFilterDrvHooksArray[i];

		if (!pDevFilterDrvHooks)
		{
			continue;
		}

		pDevFilterDrvHooks->Stop();

		delete pDevFilterDrvHooks;
		m_DevFilterDrvHooksArray[i] = NULL;
	}

	delete[] m_DevFilterDrvHooksArray;
	m_DevFilterDrvHooksArray = NULL;
	m_NumberOfHooks = 0;

Leave:
	return;
}

BOOL
DevFilterDrvHooksManager::AddMajorFunctionDispatcher(
	_In_ PWSTR DriverName,
	_In_ PDRIVER_DISPATCH MajorFunctionDispatcher
)
{
	BOOL RetVal;
	DWORD i;
	DevFilterDrvHooks* Hook;

	RetVal = FALSE;
	Hook = NULL;

	for (i = 0; i < m_NumberOfHooks; i++)
	{
		Hook = m_DevFilterDrvHooksArray[i];

		if (!Hook)
		{
			continue;
		}

		if (_wcsicmp(Hook->m_DriverName, DriverName) != 0)
		{
			continue;
		}

		Hook->m_MajorFunctionDispatcher = MajorFunctionDispatcher;

		RetVal = TRUE;
		goto Leave;
	}

Leave:
	return RetVal;
}

DevFilterDrvHooks*
DevFilterDrvHooksManager::FindByDeviceObject(
	_In_ PDEVICE_OBJECT DeviceObject
)
{
	DevFilterDrvHooks* RetVal;
	DWORD i;
	DevFilterDrvHooks* Hook;

	RetVal = NULL;
	Hook = NULL;

	for (i = 0; i < m_NumberOfHooks; i++)
	{
		Hook = m_DevFilterDrvHooksArray[i];

		if (!Hook)
		{
			continue;
		}

		if (Hook->m_DriverObject != DeviceObject->DriverObject)
		{
			continue;
		}

		RetVal = Hook;
		goto Leave;
	}

Leave:
	return RetVal;
}