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

#include <IMapFile.h>

class CMapFileImp : public UserKernelUtilsLib::IMapFileInt
{
public:
	friend IMapFileInt;

	virtual NTSTATUS  CreateSection(
		OUT PHANDLE SectionHandle,
		IN HANDLE FileHandle,
		IN PVOID SecurityDescriptor,
		IN BOOLEAN Inheritable,
		IN DWORD flProtect,
		IN DWORD MaximumSizeHigh,
		IN DWORD MaximumSizeLow,
		IN PCWSTR SectionName
	);

	/*
	 if NumberOfBytesToMap is not NULL, will return the amount mapped
	 if NumberOfBytesToMap is NULL, or *NumberOfBytesToMap is zero will map all the section/file
	 if *NumberOfBytesToMap is different of zero will only map *NumberOfBytesToMap bytes
	*/
	virtual NTSTATUS MapFile(
		_Out_ LPVOID *ViewBase,
		_In_ HANDLE SectionHandle,
		_In_ HANDLE ProcessHandle,
		_In_ DWORD DesiredAccess,
		_In_ DWORD FileOffsetHigh,
		_In_ DWORD FileOffsetLow,
		_Inout_opt_ SIZE_T *NumberOfBytesToMap
	);

	virtual BOOLEAN UnMapFile(
		_In_ PVOID BaseAddress,
		_In_ HANDLE ProcessHandle
	);

	virtual
	BOOLEAN
	MapFileIntoMem(
		_In_ PUNICODE_STRING FullFilePath,
		_Out_ PVOID* DataMemPtr,
		_Out_ ULONG* DataLength,
		_In_ BOOL ReadOnly
	);

	virtual
	BOOLEAN
	MapFileIntoMem(
		_In_ HANDLE FileHandle,
		_Out_ PVOID* DataMemPtr,
		_Out_ ULONG* DataLength,
		_In_ BOOL ReadOnly
	);

	virtual
	BOOLEAN
	MapFileIntoMemEx(
		_In_ HANDLE FileHandle,
		_Out_ PVOID* DataMemPtr,
		_Out_ ULONG* DataLength,
		_In_ BOOL ReadOnly,
		_In_ BOOL Image,
		_Inout_ PVOID* SectionHandle
	);

	virtual
	BOOLEAN
	MapFileIntoMemEx(
		_In_ PUNICODE_STRING FullFilePath,
		_Out_ PVOID* DataMemPtr,
		_Out_ ULONG* DataLength,
		_In_ BOOL ReadOnly,
		_In_ BOOL Image,
		_Inout_ PVOID* SectionHandle
	);

	/*If returned a value, the buffer must be freed: "delete[] Buff;" */
	virtual
	PCHAR
	CopyFileToMemory(
		PUNICODE_STRING FullFilePath,
		size_t* Filelength = 0
	);


private:
	CMapFileImp();
	virtual~CMapFileImp();

};

