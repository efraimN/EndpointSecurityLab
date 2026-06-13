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

#include "DevFilterUtils.h"

#include <IRegUtils.h>

DevFilterUtils* DevFilterUtils::GetInstance()
{
	static DevFilterUtils SingleTone;
	return &SingleTone;
}

DevFilterUtils::DevFilterUtils()
{
	m_DriverNamesList = NULL;
	m_FirstDriverName = NULL;
}

DevFilterUtils::~DevFilterUtils()
{
}

BOOL DevFilterUtils::Start()
{
	m_DriverNamesList = UserKernelUtilsLib::ILinkListLib::GetNewInstance();
	if (!m_DriverNamesList)
	{
		return FALSE;
	}
	m_DriverNamesList->Start();
	return TRUE;
}

void DevFilterUtils::Stop()
{
	if (m_DriverNamesList)
	{
		m_DriverNamesList->Stop();
		UserKernelUtilsLib::ILinkListLib::FreeInstance(m_DriverNamesList);
		m_DriverNamesList = NULL;
	}
}

static
BOOLEAN
FindDriverNameCallback(
	_In_ UserKernelUtilsLib::ILinkListElement* Element,
	_In_ PVOID FindContext
)
{
	DriverNameListElement* DriverElement;
	PWSTR DriverName;

	DriverElement = (DriverNameListElement*)Element;
	DriverName = (PWSTR)FindContext;

	if (_wcsicmp(DriverElement->DriverName, DriverName) == 0)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL
DevFilterUtils::AddDriverNameToList(
	_In_ PWSTR DriverName
)
{
	BOOL RetVal;
	DriverNameListElement* Element;

	RetVal = FALSE;
	Element = NULL;

	if (!m_DriverNamesList)
	{
		goto Leave;
	}

	if (m_DriverNamesList->GetElement(
		FindDriverNameCallback,
		DriverName,
		FALSE
	))
	{
		RetVal = TRUE;
		goto Leave;
	}

	Element =
#ifdef _NTDDK_
		new('rvDD')
#else
		new(std::nothrow)
#endif
		DriverNameListElement();

	if (!Element)
	{
		goto Leave;
	}

	wcsncpy(
		Element->DriverName,
		DriverName,
		ARRAYSIZE(Element->DriverName) - 1
	);
	Element->DriverName[ARRAYSIZE(Element->DriverName) - 1] = 0;
	m_DriverNamesList->PushItemTail(Element);

	RetVal = TRUE;

Leave:
	return RetVal;
}

BOOL DevFilterUtils::GetServiceKey(
	_In_ HANDLE KeyHandle,
	_Out_writes_(ServiceLength) PWCHAR Service,
	_In_ DWORD ServiceLength
)
{
	NTSTATUS status;
	BOOL RetVal = FALSE;

	RtlZeroMemory(
		Service,
		ServiceLength
	);

	status = UserKernelUtilsLib::IRegUtils::GetInstance()->RegGetValue(
		RTL_REGISTRY_HANDLE,
		(PWSTR)KeyHandle,
		L"Service",
		Service,
		&ServiceLength
	);

	if (!NT_SUCCESS(status))
	{
		goto Leave;
	}

	RetVal = TRUE;
Leave:
	return RetVal;
}

BOOL
DevFilterUtils::GetDriversNamesByClassGuidRegEnumCallback(
	_In_ HANDLE KeyHandle,
	_In_opt_ PVOID Context
)
{
	BOOL RetVal;
	NTSTATUS status;
	WCHAR ClassGuid[128];
	DWORD ClassGuidLength;
	PWSTR Ctx = (PWSTR)Context;
	WCHAR Service[128];

	UserKernelUtilsLib::IRegUtils* regUtils;

	RetVal = TRUE;
	regUtils = NULL;

	regUtils = UserKernelUtilsLib::IRegUtils::GetInstance();

	RtlZeroMemory(
		ClassGuid,
		sizeof(ClassGuid)
	);

	ClassGuidLength = sizeof(ClassGuid);

	status = regUtils->RegGetValue(
		RTL_REGISTRY_HANDLE,
		(PWSTR)KeyHandle,
		L"ClassGUID",
		ClassGuid,
		&ClassGuidLength
	);

	if (!NT_SUCCESS(status))
	{
		goto Leave;
	}

	if (_wcsicmp(ClassGuid, Ctx) != 0)
	{
		goto Leave;
	}

	if (!DevFilterUtils::GetInstance()->GetServiceKey(
		KeyHandle,
		Service,
		sizeof(Service)
	))
	{
		goto Leave;
	}

	DevFilterUtils::GetInstance()->AddDriverNameToList(Service);

Leave:
	return RetVal;
}

NTSTATUS
DevFilterUtils::GetDriversNamesByClassGuid(
	_In_ PWSTR ClassGuid
)
{
	NTSTATUS status;
	HANDLE hEnum;
	UserKernelUtilsLib::IRegUtils* regUtils;

	status = STATUS_UNSUCCESSFUL;
	hEnum = NULL;
	regUtils = NULL;

	regUtils = UserKernelUtilsLib::IRegUtils::GetInstance();

	if (!regUtils)
	{
		goto Leave;
	}

	hEnum = regUtils->RegOpenCreateKey(
		RTL_REGISTRY_ABSOLUTE,
		L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Enum",
		NULL,
		TRUE,
		KEY_READ,
		0
	);

	if (!hEnum)
	{
		goto Leave;
	}

	status = regUtils->RegEnumKeys(
		hEnum,
		GetDriversNamesByClassGuidRegEnumCallback,
		ClassGuid
	);

Leave:
	if (hEnum)
	{
		ZwClose(hEnum);
	}

	return status;
}

PWCHAR
DevFilterUtils::GetNextDriverNameInternal()
{
	PWCHAR RetVal =  NULL;
	DriverNameListElement* Element;

	Element = NULL;

	if (!m_DriverNamesList)
	{
		goto Leave;
	}

	Element = (DriverNameListElement*)m_DriverNamesList->PopItemHead();

	if (!Element)
	{
		goto Leave;
	}
	m_DriverNamesList->PushItemTail(Element);



	RetVal = Element->DriverName;

Leave:
	return RetVal;
}

PWCHAR
DevFilterUtils::GetFirstDriverName()
{
	m_FirstDriverName = GetNextDriverNameInternal();

	return m_FirstDriverName;

}

PWCHAR
DevFilterUtils::GetNextDriverName()
{
	PWCHAR Tmp = NULL;

	if (!m_FirstDriverName)
	{
		return NULL;
	}

	Tmp = GetNextDriverNameInternal();

	if (_wcsicmp(Tmp, m_FirstDriverName) == 0)
	{
		return NULL;
	}

	return Tmp;
}
