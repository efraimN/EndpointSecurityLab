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

#pragma once

#include <IRegUtils.h>

class CRegUtils : public UserKernelUtilsLib::IRegUtils
{
public:
	static CRegUtils *GetInstance();

	virtual
	NTSTATUS
	RegDeleteAllKeys(HANDLE ParentKey, DWORD DesiredAccess = 0);

	virtual
	NTSTATUS
	RegSetValue(
		IN     ULONG           RelativeTo,
		IN     PWSTR           KeyNamePath,
		IN     PWSTR           ParameterName,
		IN OUT PVOID           ParameterValue,
		IN OUT ULONG           ParameterLength,
		IN     ULONG		   ValueType
	);

	virtual
	NTSTATUS
	RegGetValue(
		IN     ULONG           RelativeTo,
		IN     PWSTR           KeyNamePath,
		IN     PWSTR           ParameterName,
		IN OUT PVOID           ParameterValue,
		IN OUT PDWORD          ParameterLength
	);

	virtual
	BOOL
	RegDeletekey(PWCHAR Path);

	virtual
	HANDLE
	RegOpenCreateKey(
		_In_  ULONG RelativeTo,
		_In_  PWSTR Path,
		_In_  PWSTR SubPath,
		BOOLEAN Open_Create,
		ACCESS_MASK DesiredAccess,
		ULONG CreateOptions
	);
	
	virtual
	HANDLE
	RegOpenCreateKeyStatus(
		_In_  ULONG RelativeTo,
		_In_  PWSTR Path,
		_In_  PWSTR SubPath,
		BOOLEAN Open_Create,
		ACCESS_MASK DesiredAccess,
		ULONG CreateOptions,
		NTSTATUS *Status
	);

	virtual
	NTSTATUS
	AddMultiStringToKey(
		_In_ ULONG RelativeTo,
		_In_ PWSTR KeyNamePath,
		_In_ PWSTR ValueName,
		_In_ PWSTR After,
		_In_ PWSTR Value
	);

	virtual
	NTSTATUS
	RemoveMultiStringFromKey(
		_In_ ULONG RelativeTo,
		_In_ PWSTR KeyNamePath,
		_In_ PWSTR ValueName,
		_In_ PWSTR DelString
	);

	virtual
	NTSTATUS
	RemoveMultiStringFromKeyDeleteIfEmpty(
		_In_ ULONG RelativeTo,
		_In_ PWSTR KeyNamePath,
		_In_ PWSTR ValueName,
		_In_ PWSTR DelString
	);

	virtual
	NTSTATUS
	RegEnumKeys(
		_In_ HANDLE RootKey,
		_In_ PREG_ENUM_KEYS_CALLBACK Callback,
		_In_opt_ PVOID Context
	);

private:
	CRegUtils();
	virtual ~CRegUtils();

	NTSTATUS RegDeleteAllSubKeys(HANDLE	ParentKey, DWORD DesiredAccess);

	virtual
	NTSTATUS
	RemoveMultiStringFromKeyEx(
	ULONG RelativeTo,
	PWSTR KeyNamePath,
	PWSTR ValueName,
	PWSTR DelString,
	BOOL DeleteOnEmpty
	);
};

