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

#include "FileUtils.h"
using namespace UserKernelUtilsLib;

CFileUtils::CFileUtils()
{
}

CFileUtils::~CFileUtils()
{
}

IFileUtils *IFileUtils::GetInstance()
{
	static CFileUtils Singletone;
	return &Singletone;
}

BOOLEAN CFileUtils::OpenCreateFileCommon(
	PUNICODE_STRING FileName,
	PHANDLE hFile,
	ACCESS_MASK DesiredAccess,
	ULONG ShareAccess,
	PLARGE_INTEGER AllocationSize
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

	NTSTATUS status = STATUS_UNSUCCESSFUL;
	OBJECT_ATTRIBUTES ObAttr;
	BOOLEAN RetVal = FALSE;
	DWORD OA_Flags;

	OA_Flags = OBJ_CASE_INSENSITIVE
#ifdef _NTDDK_
		| OBJ_KERNEL_HANDLE
#endif
		;

	InitializeObjectAttributes(
		&ObAttr,
		FileName,
		OA_Flags
		,
		NULL,
		NULL
	);

	IO_STATUS_BLOCK iosb = { 0 };

	if (!AllocationSize)
	{
		status = ZwOpenFile(
			hFile,
			DesiredAccess,
			&ObAttr,
			&iosb,
			ShareAccess,
			0
#ifdef _NTDDK_
			| FILE_SYNCHRONOUS_IO_NONALERT
#endif
		);
	}
	else
	{
		status = ZwCreateFile(
			hFile,
			GENERIC_READ | GENERIC_WRITE,
			&ObAttr,
			&iosb,
			AllocationSize,
			0,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			FILE_SUPERSEDE,
			0
#ifdef _NTDDK_
			| FILE_SYNCHRONOUS_IO_NONALERT
#endif
			,
			NULL,
			0
		);
	}

	if (!NT_SUCCESS(status))
	{
		ESL_DBG_OUT(DBG_ERROR, "ZwOpenFile Failed for file %S filename length %d status 0x%08X", FileName->Buffer, (UINT)FileName->Length, status);
		goto Leave;
	}

	RetVal = TRUE;
Leave:
	return RetVal;
}

BOOLEAN CFileUtils::OpenFile(
	PUNICODE_STRING FileName,
	PHANDLE hFile,
	ACCESS_MASK DesiredAccess,
	ULONG ShareAccess
)
{
	return OpenCreateFileCommon(
		FileName,
		hFile,
		DesiredAccess,
		ShareAccess,
		NULL
	);
}

BOOLEAN CFileUtils::CreateFile(
	PUNICODE_STRING FileName,
	PHANDLE hFile,
	ACCESS_MASK DesiredAccess,
	ULONG ShareAccess,
	LARGE_INTEGER AllocationSize
)
{
	return OpenCreateFileCommon(
		FileName,
		hFile,
		DesiredAccess,
		ShareAccess,
		&AllocationSize
	);
}

BOOLEAN
CFileUtils::GetFileSize(
	HANDLE hFile,
	PLARGE_INTEGER FileSize
)
{
	NTSTATUS status;
	FILE_STANDARD_INFORMATION FileStandard;
	IO_STATUS_BLOCK IoStatusBlock = {0};

	if (!FileSize)
	{
		return FALSE;
	}

	status = ZwQueryInformationFile(hFile,
		&IoStatusBlock,
		&FileStandard,
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation);
	if (!NT_SUCCESS(status))
	{
		return FALSE;
	}

	*FileSize = FileStandard.EndOfFile;

	return TRUE;
}

BOOLEAN
CFileUtils::SetFileSize(
	HANDLE hFile,
	LARGE_INTEGER FileSize
)
{
	NTSTATUS status;
	FILE_END_OF_FILE_INFORMATION FileEndOfFile;
	IO_STATUS_BLOCK IoStatusBlock = {0};


	FileEndOfFile.EndOfFile = FileSize;

	status = ZwSetInformationFile(
		hFile,
		&IoStatusBlock,
		&FileEndOfFile,
		sizeof(FILE_END_OF_FILE_INFORMATION),
		FileEndOfFileInformation);
	if (!NT_SUCCESS(status))
	{
		return FALSE;
	}

	return TRUE;
}
