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

#include <ILinkListLibInt.h>

class DriverNameListElement : public UserKernelUtilsLib::ILinkListElement
{
public:
	WCHAR DriverName[128];
};

class DevFilterUtils
{
public:
	static DevFilterUtils* GetInstance();
	BOOL Start();
	void Stop();

	NTSTATUS
	GetDriversNamesByClassGuid(
		_In_ PWSTR ClassGuid
	);

	PWCHAR
	GetFirstDriverName();

	PWCHAR
	GetNextDriverName();

protected:

private:

	DevFilterUtils();
	virtual ~DevFilterUtils();

	static
		BOOL
		GetDriversNamesByClassGuidRegEnumCallback(
			_In_ HANDLE KeyHandle,
			_In_opt_ PVOID Context
		);

	BOOL GetServiceKey(
		_In_ HANDLE KeyHandle,
		_Out_writes_(ServiceLength) PWCHAR Service,
		_In_ DWORD ServiceLength
	);

	BOOL
		AddDriverNameToList(
			_In_ PWSTR DriverName
		);

	PWCHAR
	GetNextDriverNameInternal();

	PWCHAR m_FirstDriverName;
	UserKernelUtilsLib::ILinkListLib* m_DriverNamesList;
};
