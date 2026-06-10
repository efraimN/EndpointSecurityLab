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

#include "RegUtils.h"
using namespace UserKernelUtilsLib;

#include <IMultiStringUtils.h>

CRegUtils::CRegUtils()
{
}

CRegUtils::~CRegUtils()
{
}

CRegUtils *CRegUtils::GetInstance()
{
	static CRegUtils s_CRegUtils;
	return &s_CRegUtils;
}

IRegUtilsInt *IRegUtilsInt::GetInstance()
{
	return CRegUtils::GetInstance();
}

#define DEVREG_MAX_REGISTRY_KEY		((ULONG)(-1))
NTSTATUS CRegUtils::RegDeleteAllSubKeys(HANDLE	ParentKey, DWORD DesiredAccess)
{
// 	auto ProcEntry = [](BOOL start)
// 	{
// 		if (start)
// 			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
// 		else
// 			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");
// 
// 	};
// 	PROC_ENTRY;

	NTSTATUS	status;
	PKEY_BASIC_INFORMATION	keyInfo;
	ULONG	outLength;
	ULONG	idxKey;
	OBJECT_ATTRIBUTES	objectAttributes;
	UNICODE_STRING	objectName;
	HANDLE	childKey;

#ifdef _NTDDK_
	keyInfo = (PKEY_BASIC_INFORMATION) new('1234') char[512];
#else
	keyInfo = (PKEY_BASIC_INFORMATION) new(std::nothrow) char[512];
#endif //_NTDDK_
	if (!keyInfo)
	{
		ESL_DBG_OUT(DBG_INFO, "new(KEY_BASIC_INFORMATION) failed.");
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	status = STATUS_SUCCESS;
	for (idxKey = 0; idxKey < DEVREG_MAX_REGISTRY_KEY; idxKey++)
	{
		status = ZwEnumerateKey(
			ParentKey,
			idxKey,
			KeyBasicInformation,
			keyInfo,
			512,
			&outLength
		);

		if (status == STATUS_NO_MORE_ENTRIES)
		{
			ESL_DBG_OUT(DBG_INFO, ("No more entries."));
			status = STATUS_SUCCESS;
			break;
		}
		if (status != STATUS_SUCCESS)
		{
			//ASSERT(status != STATUS_BUFFER_OVERFLOW && status != STATUS_BUFFER_TOO_SMALL);
			ESL_DBG_OUT(DBG_INFO, "ZwEnumerateKey() failed. NTSTATUS:%08lx", status);
			delete[](char*)keyInfo;
			keyInfo = NULL;
			return STATUS_SUCCESS;
		}


		//
		// Open a sub key
		//
		objectName.Length = objectName.MaximumLength = (USHORT)keyInfo->NameLength;
		objectName.Buffer = keyInfo->Name;
		InitializeObjectAttributes(&objectAttributes,
			&objectName,
			OBJ_KERNEL_HANDLE,
			ParentKey,
			NULL
		);
		status = ZwOpenKey(
			&childKey,
			KEY_ALL_ACCESS | DesiredAccess,
			&objectAttributes
		);
		if (!NT_SUCCESS(status))
		{
			ESL_DBG_OUT(DBG_INFO, "ZwOpenKey() failed. NTSTATUS:%08lx", status);
			continue;
		}

		//
		// Delete all subkeys
		//
		status = RegDeleteAllSubKeys(childKey , DesiredAccess); // recursive
		if (!NT_SUCCESS(status))
		{
			ESL_DBG_OUT(DBG_INFO, "Recursive DeleteAllSubKeys() failed. NTSTATUS:%08lx", status);
			ZwClose(childKey);
			continue;
		}

		status = ZwDeleteKey(childKey);

		if (!NT_SUCCESS(status))
		{
			ESL_DBG_OUT(DBG_INFO, "ZwDeleteKey() failed. NTSTATUS:%08lx", status);
		}

		ZwClose(childKey);

		//
		// One key was deleted, decrement key index.
		//

		idxKey--;

	}

	delete[]((PBYTE)keyInfo);
	return STATUS_SUCCESS;
}


NTSTATUS CRegUtils::RegDeleteAllKeys(HANDLE ParentKey, DWORD DesiredAccess /* = 0 */ )
{
// 	auto ProcEntry = [](BOOL start)
// 	{
// 		if (start)
// 			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
// 		else
// 			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");
// 
// 	};
// 	PROC_ENTRY;
// 
	NTSTATUS	status;

	status = RegDeleteAllSubKeys(ParentKey, DesiredAccess);
	if (!NT_SUCCESS(status))
	{
		ESL_DBG_OUT(DBG_ERROR, "RegDeleteAllSubKeys Failed status %lu", status);
		return status;
	}
	status = ZwDeleteKey(ParentKey);

	return status;
}

NTSTATUS
CRegUtils::RegSetValue(
	IN     ULONG           RelativeTo,
	IN     PWSTR           KeyNamePath,
	IN     PWSTR           ParameterName,
	IN OUT PVOID           ParameterValue,
	IN OUT ULONG           ParameterLength,
	IN     ULONG		   ValueType
)
{
// 	auto ProcEntry = [](BOOL start)
// 	{
// 		if (start)
// 			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
// 		else
// 			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");
// 
// 	};
// 	PROC_ENTRY;

	NTSTATUS    status;

	status = RtlWriteRegistryValue(RelativeTo, KeyNamePath, ParameterName, ValueType, ParameterValue, ParameterLength);
	if (!NT_SUCCESS(status))
	{
		ESL_DBG_OUT(DBG_ERROR, "******Error calling RtlWriteRegistryValue(): 0x%X KeyNamePath %S ParameterName %S ParameterLength %d ParameterValue %S",
			status,
			KeyNamePath,
			ParameterName,
			ParameterLength,
			(PWSTR)ParameterValue
		);
		goto Leave;
	}

Leave:
	return status;
}

static
NTSTATUS
NTAPI
RegGetKeyValueQueryCallbackRoutine(
	_In_      PWSTR ValueName,
	_In_      ULONG ValueType,
	_In_      PVOID ValueData,
	_In_      ULONG ValueLength,
	_In_opt_  PVOID Context,
	_In_opt_  PVOID EntryContext
)
{
// 	auto ProcEntry = [](BOOL start)
// 	{
// 		if (start)
// 			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
// 		else
// 			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");
// 
// 	};
// 	PROC_ENTRY;

	UNREFERENCED_PARAMETER(ValueName);
	UNREFERENCED_PARAMETER(ValueType);

// 	ESL_DBG_OUT(DBG_INFO, "ValueName is %S", ValueName);
// 	ESL_DBG_OUT(DBG_INFO, "ValueType is %d", ValueType);
// 	ESL_DBG_OUT(DBG_INFO, "ValueData is %S", (LPCWSTR)ValueData);
// 	ESL_DBG_OUT(DBG_INFO, "ValueLength is %d", ValueLength);
// 	ESL_DBG_OUT(DBG_INFO, "Context is %d", *(DWORD*)Context);
// 	ESL_DBG_OUT(DBG_INFO, "EntryContext is %p", EntryContext);

	if ((*(DWORD*)Context) >= ValueLength)
	{
		if (EntryContext)
		{
			memcpy(EntryContext, ValueData, ValueLength);
		}
	}

	(*(DWORD*)Context) = ValueLength;

	return STATUS_SUCCESS;
}

NTSTATUS
CRegUtils::RegGetValue(
	IN     ULONG           RelativeTo,
	IN     PWSTR           KeyNamePath,
	IN     PWSTR           ParameterName,
	IN OUT PVOID           ParameterValue,
	IN OUT PDWORD          ParameterLength
)
{
// 	auto ProcEntry = [](BOOL start)
// 	{
// 		if (start)
// 			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
// 		else
// 			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");
// 
// 	};
// 	PROC_ENTRY;

	NTSTATUS                 status;
	RTL_QUERY_REGISTRY_TABLE queryTable[2];

// 	ESL_DBG_OUT(DBG_INFO, "KeyNamePath =  %S ParameterName = %S", KeyNamePath, ParameterName);
	RtlZeroMemory(queryTable, sizeof(queryTable));

	queryTable[0].Flags = RTL_QUERY_REGISTRY_REQUIRED | RTL_QUERY_REGISTRY_NOEXPAND;
	queryTable[0].Name = ParameterName;
	queryTable[0].EntryContext = ParameterValue;
	queryTable[0].DefaultType = REG_NONE;

	queryTable[0].QueryRoutine = RegGetKeyValueQueryCallbackRoutine;

	status = RtlQueryRegistryValues(RelativeTo, KeyNamePath, queryTable, ParameterLength, NULL);

	if (!NT_SUCCESS(status))
	{
		ESL_DBG_OUT(DBG_ERROR, "RtlQueryRegistryValues Failed status =  0x%08X", status);
	}
// 	else
// 	{
// 		ESL_DBG_OUT(DBG_INFO, "Return status =  0x%08X", status);
// 	}
	return status;
}

HANDLE
CRegUtils::RegOpenCreateKeyStatus(
	_In_  ULONG RelativeTo,
	_In_  PWSTR Path,
	_In_  PWSTR SubPath,
	BOOLEAN Open_Create,
	ACCESS_MASK DesiredAccess,
	ULONG CreateOptions,
	NTSTATUS* Status
)
{
// 	auto ProcEntry = [](BOOL start)
// 	{
// 		if (start)
// 			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
// 		else
// 			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");
// 
// 	};
// 	PROC_ENTRY;

	NTSTATUS status;
	OBJECT_ATTRIBUTES ObjectAttributes;
	WCHAR* RootKey = NULL;
	UNICODE_STRING UniRootKey;
	HANDLE RetVal = NULL;
	HANDLE Handle = NULL;
	ULONG Disposition;
	ULONG Attributes;

	switch (RelativeTo)
	{
	case RTL_REGISTRY_HANDLE:
		Handle = (HANDLE)Path;
		RootKey = SubPath;
		break;
	case RTL_REGISTRY_ABSOLUTE:
		RootKey = Path;
		break;

	default:
		return FALSE;
		break;
	}

	Attributes = 0
		| OBJ_CASE_INSENSITIVE
#ifdef _NTDDK_
		| OBJ_KERNEL_HANDLE
#endif
		;

	RtlInitUnicodeString(&UniRootKey, RootKey);

	InitializeObjectAttributes(
		&ObjectAttributes,
		&UniRootKey,
		Attributes,
		Handle,
		NULL);

	if (Open_Create)
	{
		status = ZwOpenKey(&RetVal, DesiredAccess, &ObjectAttributes);
	}
	else
	{
		status = ZwCreateKey(&RetVal, DesiredAccess, &ObjectAttributes,
			0, NULL, CreateOptions, &Disposition);
	}
	if (Status)
	{
		*Status = status;
	}
	if (!NT_SUCCESS(status))
	{
		ESL_DBG_OUT(DBG_INFO, "%s Error : 0x%08X", Open_Create ? "ZwOpenKey" : "ZwCreateKey", status);
		return FALSE;
	}

	return RetVal;
}

HANDLE
CRegUtils::RegOpenCreateKey(
	_In_  ULONG RelativeTo,
	_In_  PWSTR Path,
	_In_  PWSTR SubPath,
	BOOLEAN Open_Create,
	ACCESS_MASK DesiredAccess,
	ULONG CreateOptions
)
{
	return RegOpenCreateKeyStatus(
		RelativeTo,
		Path,
		SubPath,
		Open_Create,
		DesiredAccess,
		CreateOptions,
		NULL
	);
}

BOOL
CRegUtils::RegDeletekey(PWCHAR Path)
{
// 	auto ProcEntry = [](BOOL start)
// 	{
// 		if (start)
// 			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
// 		else
// 			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");
// 
// 	};
// 	PROC_ENTRY;

	HANDLE hkey;
	BOOL RetVal = FALSE;

	hkey = RegOpenCreateKey(
		RTL_REGISTRY_ABSOLUTE,
		Path,
		NULL,
		TRUE,// open
		KEY_ALL_ACCESS,//DELETE
		NULL// don't care
	);

	if (!hkey)
	{
		ESL_DBG_OUT(DBG_INFO, "RtlRegOpenCreateKey failed key for %S", Path);
		goto Leave;
	}

	if (!NT_SUCCESS(ZwDeleteKey(hkey)))
	{
		ESL_DBG_OUT(DBG_INFO, "ZwDeleteKey failed key for %S", Path);
	}

	RetVal = TRUE;

Leave:
	if (hkey)
	{
		ZwClose(hkey);
	}
	return RetVal;
}

NTSTATUS
CRegUtils::AddMultiStringToKey(
	_In_ ULONG RelativeTo,
	_In_ PWSTR KeyNamePath,
	_In_ PWSTR ValueName,
	_In_ PWSTR Where,
	_In_ PWSTR Value
)
{
	NTSTATUS status;
	PVOID ParameterValue = NULL;
	DWORD ParameterLength;
	PWCHAR MultiStringCopy = NULL;
	DWORD DestLength;

	ParameterLength = 0;
	status = this->RegGetValue(
		RelativeTo,
		KeyNamePath,
		ValueName,
		ParameterValue,
		&ParameterLength
	);

	if (NT_SUCCESS(status))
	{
		if (ParameterLength)
		{
			ParameterValue =
#ifndef _NTDDK_
			new 
#else
			new ('tLuM')
#endif
			BYTE[ParameterLength];

			if (!ParameterValue)
			{
				goto Leave;
			}
		}
		status = this->RegGetValue(
			RelativeTo,
			KeyNamePath,
			ValueName,
			ParameterValue,
			&ParameterLength
		);
	}
	else
	{
		ParameterLength = (DWORD)wcslen(Value)+2;
		ParameterLength *= 2;
		ParameterValue = 
#ifndef _NTDDK_
			new
#else
			new ('tLuM')
#endif
		BYTE[ParameterLength];
		if (!ParameterValue)
		{
			goto Leave;
		}
		RtlZeroMemory(ParameterValue, ParameterLength);
		RtlCopyMemory(ParameterValue, Value, ParameterLength-2);
		status = this->RegSetValue(
			RelativeTo,
			KeyNamePath,
			ValueName,
			ParameterValue,
			ParameterLength,
			REG_MULTI_SZ
		);
		goto Leave;
	}

	status = STATUS_UNSUCCESSFUL;

	if (!IMultiStringUtils::CheckMultiString((PWCHAR)ParameterValue, ParameterLength/2))
	{
		goto Leave;
	}

	if (!IMultiStringUtils::CopyAddStringToMultiString(
		(PWCHAR)ParameterValue,
		&MultiStringCopy,
		Value,
		Where
	))
	{
		goto Leave;
	}

	DestLength = IMultiStringUtils::GetMultiStringSize(MultiStringCopy)*2;
	status = this->RegSetValue(
		RelativeTo,
		KeyNamePath,
		ValueName,
		MultiStringCopy,
		DestLength,
		REG_MULTI_SZ
		);

Leave:
	if (ParameterValue)
	{
		delete[]ParameterValue;
	}
	if (MultiStringCopy)
	{
		delete[]MultiStringCopy;
	}
	return status;
}

NTSTATUS
CRegUtils::RemoveMultiStringFromKeyEx(
	ULONG RelativeTo,
	PWSTR KeyNamePath,
	PWSTR ValueName,
	PWSTR DelString,
	BOOL DeleteOnEmpty
)
{
	NTSTATUS status;
	PVOID ParameterValue = NULL;
	DWORD ParameterLength;
	PWCHAR MultiStringCopy = NULL;
	DWORD DestLength;

	ParameterLength = 0;
	status = this->RegGetValue(
		RelativeTo,
		KeyNamePath,
		ValueName,
		ParameterValue,
		&ParameterLength
	);
	if (!NT_SUCCESS(status))
	{
		goto Leave;
	}
	status = STATUS_UNSUCCESSFUL;
	if (!ParameterLength)
	{
		goto Leave;
	}
	ParameterValue = 
#ifndef _NTDDK_
		new
#else
		new ('LumR')
#endif
		BYTE[ParameterLength];
	if (!ParameterValue)
	{
		goto Leave;
	}
	status = this->RegGetValue(
		RelativeTo,
		KeyNamePath,
		ValueName,
		ParameterValue,
		&ParameterLength
	);
	if (!NT_SUCCESS(status))
	{
		goto Leave;
	}

	status = STATUS_UNSUCCESSFUL;
	if (!IMultiStringUtils::CheckMultiString((PWCHAR)ParameterValue, ParameterLength/2))
	{
		goto Leave;
	}

	if (!IMultiStringUtils::CopyDeleteStringFromMultiString(
		(PWCHAR)ParameterValue,
		&MultiStringCopy,
		DelString
	))
	{
		goto Leave;
	}

	DestLength = IMultiStringUtils::GetMultiStringSize(MultiStringCopy) * 2;
	if (DestLength == 4)
	{
		if (MultiStringCopy[0]==0)
		{
			DestLength = 2;
		}
	}
	if (DestLength == 2)
	{
		if (DeleteOnEmpty)
		{
			HANDLE hReg;
			UNICODE_STRING UValueName;

			hReg = RegOpenCreateKey(
				RelativeTo,
				KeyNamePath,
				NULL,
				TRUE,
				KEY_ALL_ACCESS,
				NULL
			);
			if (!hReg)
			{
				status = STATUS_UNSUCCESSFUL;
				goto Leave;
			}

			UValueName.Buffer = ValueName;
			UValueName.Length = (USHORT)wcslen(ValueName)*2;
			UValueName.MaximumLength = UValueName.Length + 2;

			status = ZwDeleteValueKey(
				hReg,
				&UValueName
			);
			ZwClose(hReg);
		}
		else
		{
			status = this->RegSetValue(
				RelativeTo,
				KeyNamePath,
				ValueName,
				MultiStringCopy,
				DestLength,
				REG_MULTI_SZ
			);
		}
	}
	else
	{
		status = this->RegSetValue(
			RelativeTo,
			KeyNamePath,
			ValueName,
			MultiStringCopy,
			DestLength,
			REG_MULTI_SZ
		);
	}

Leave:
	if (ParameterValue)
	{
		delete[]ParameterValue;
	}
	if (MultiStringCopy)
	{
		delete[]MultiStringCopy;
	}
	return status;
}

NTSTATUS
CRegUtils::RemoveMultiStringFromKey(
	_In_ ULONG RelativeTo,
	_In_ PWSTR KeyNamePath,
	_In_ PWSTR ValueName,
	_In_ PWSTR DelString
)
{
	return
		RemoveMultiStringFromKeyEx(
			RelativeTo,
			KeyNamePath,
			ValueName,
			DelString,
			FALSE
		);
}

NTSTATUS
CRegUtils::RemoveMultiStringFromKeyDeleteIfEmpty(
	_In_ ULONG RelativeTo,
	_In_ PWSTR KeyNamePath,
	_In_ PWSTR ValueName,
	_In_ PWSTR DelString
)
{
	return
		RemoveMultiStringFromKeyEx(
			RelativeTo,
			KeyNamePath,
			ValueName,
			DelString,
			TRUE
		);
}
