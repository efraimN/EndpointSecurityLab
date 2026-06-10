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

#include "MapFile.h"
#include <IFileUtils.h>

using namespace UserKernelUtilsLib;

CMapFileImp::CMapFileImp()
{
}

CMapFileImp::~CMapFileImp()
{
}

IMapFileInt *IMapFileInt::GetInstance()
{
	static CMapFileImp Singletone;
	return &Singletone;
}

NTSTATUS CMapFileImp::CreateSection(
	OUT PHANDLE SectionHandle,
	IN HANDLE FileHandle,
	IN PVOID SecurityDescriptor,
	IN BOOLEAN Inheritable,
	IN DWORD flProtect,
	IN DWORD MaximumSizeHigh,
	IN DWORD MaximumSizeLow,
	IN PCWSTR SectionName
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
	ACCESS_MASK DesiredAccess;
	ULONG AllocationAttributes;
	ULONG SectionPageProtection;
	UNICODE_STRING SectionNameU;
	ULONG DefaultAttributes;
	OBJECT_ATTRIBUTES ObjectAttributes;
	LARGE_INTEGER SectionSize;
	BOOLEAN IsSize = FALSE;

	SectionSize.LowPart = 0;
	SectionSize.HighPart = 0;

	if (MaximumSizeLow || MaximumSizeHigh)
	{
		SectionSize.LowPart = MaximumSizeLow;
		SectionSize.HighPart = MaximumSizeHigh;
		IsSize = TRUE;
	}

	if (FileHandle == INVALID_HANDLE_VALUE)
	{
		/* It's not, we'll only go on if we have a size */
		FileHandle = NULL;
		if (!IsSize)
		{
			/* No size, so this isn't a valid non-mapped section */
			status = STATUS_INVALID_PARAMETER_2;
			goto Leave;
		}
	}

	DefaultAttributes = SEC_FILE | SEC_RESERVE | SEC_COMMIT | SEC_LARGE_PAGES
		| SEC_IMAGE | SEC_NOCACHE
		;

	/* Minimum Access rights that will always be requested */
	DesiredAccess = STANDARD_RIGHTS_REQUIRED | SECTION_QUERY | SECTION_MAP_READ;
	/* Get the AllocationAttributes part from the protect flags */
	AllocationAttributes = flProtect & DefaultAttributes;
	/* Get now the SectionPageProtection part from the protect flags*/
	SectionPageProtection = flProtect ^ AllocationAttributes;
	/* In case of no Section Attributes then...*/
	if (!AllocationAttributes) AllocationAttributes = SEC_COMMIT;

	if (SectionPageProtection == PAGE_READWRITE)
	{
		/* Give it */
		DesiredAccess |= SECTION_MAP_WRITE;
	}
	else if (SectionPageProtection == PAGE_EXECUTE_READWRITE)
	{
		/* Give it */
		DesiredAccess |= (SECTION_MAP_WRITE | SECTION_MAP_EXECUTE);
	}
	else if (SectionPageProtection == PAGE_EXECUTE_READ)
	{
		/* Give it */
		DesiredAccess |= SECTION_MAP_EXECUTE;
	}
	else if (SectionPageProtection == PAGE_EXECUTE_WRITECOPY)
	{
		/* Give it */
		DesiredAccess |= (SECTION_MAP_WRITE | SECTION_MAP_EXECUTE);
	}
	else if ((SectionPageProtection != PAGE_READONLY) && (flProtect != PAGE_WRITECOPY))
	{
		status = STATUS_INVALID_PARAMETER_5;
		goto Leave;
	}

	if (SectionName) RtlInitUnicodeString(&SectionNameU, SectionName);

	DWORD OA_Flags = OBJ_CASE_INSENSITIVE
#ifdef _NTDDK_
		| OBJ_KERNEL_HANDLE
#endif
		;

		if (Inheritable)
		{
			OA_Flags |= OBJ_INHERIT;
		}

	InitializeObjectAttributes(
		&ObjectAttributes,
		SectionName ? &SectionNameU : NULL,
		OA_Flags,
		NULL,
		SecurityDescriptor
	);

	/* Now create the actual section */
     status = ZwCreateSection(
		 SectionHandle,
		 DesiredAccess,
		 &ObjectAttributes,
		 &SectionSize,
		 SectionPageProtection,
		 AllocationAttributes,
		 FileHandle
	 );

 Leave:
	 return status;
}

/*
 if NumberOfBytesToMap is not NULL, will return the amount mapped
 if NumberOfBytesToMap is NULL, or *NumberOfBytesToMap is zero will map all the section/file
 if *NumberOfBytesToMap is different of zero will only map *NumberOfBytesToMap bytes
*/
NTSTATUS CMapFileImp::MapFile(
	_Out_ LPVOID *ViewBase,
	_In_ HANDLE SectionHandle,
	_In_ HANDLE ProcessHandle,
	_In_ DWORD DesiredAccess,
	_In_ DWORD FileOffsetHigh,
	_In_ DWORD FileOffsetLow,
	_Inout_opt_ SIZE_T *NumberOfBytesToMap
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
	LARGE_INTEGER SectionOffset;
     SIZE_T ViewSize;
     ULONG Protect;

     /* Convert the offset */
     SectionOffset.LowPart = FileOffsetLow;
     SectionOffset.HighPart = FileOffsetHigh;

     *ViewBase = NULL;
     ViewSize = NumberOfBytesToMap? *NumberOfBytesToMap:0;

     /* Convert flags to NT Protection Attributes */
     if (DesiredAccess & FILE_MAP_COPY)
     {
         Protect = (DesiredAccess & FILE_MAP_EXECUTE) ? 
					PAGE_EXECUTE_WRITECOPY : PAGE_WRITECOPY;
     }
     else if (DesiredAccess & FILE_MAP_WRITE)
     {
         Protect = (DesiredAccess & FILE_MAP_EXECUTE) ?
                    PAGE_EXECUTE_READWRITE : PAGE_READWRITE;
     }
     else if (DesiredAccess & FILE_MAP_READ)
     {
         Protect = (DesiredAccess & FILE_MAP_EXECUTE) ?
                    PAGE_EXECUTE_READ : PAGE_READONLY;
     }
     else
     {
         Protect = PAGE_NOACCESS;
     }

     /* Map the section */
     status = ZwMapViewOfSection(
		 SectionHandle,
		 ProcessHandle,
		 ViewBase,
		 0,
		 0,
		 &SectionOffset,
		 &ViewSize,
		 ViewUnmap,
		 0,
		 Protect
	 );
     if (!NT_SUCCESS(status))
     {
		 goto Leave;
     }

	 if (NumberOfBytesToMap)
	 {
		 *NumberOfBytesToMap = ViewSize;
	 }
Leave:
     return status;
}

BOOLEAN CMapFileImp::UnMapFile(
	_In_ PVOID BaseAddress,
	_In_ HANDLE ProcessHandle
	)
{
	NTSTATUS status;
	BOOLEAN RetVal = FALSE;

	/* Unmap the section */
	status = ZwUnmapViewOfSection(ProcessHandle, (PVOID)BaseAddress);
	if (!NT_SUCCESS(status))
	{
		/* Check if the pages were protected */
// 		if (status != STATUS_INVALID_PAGE_PROTECTION)
// 		{
			goto Leave;
// 		}
		/* Flush the region if it was a "secure memory cache" */
// 		if (!RtlFlushSecureMemoryCache((PVOID)BaseAddress, 0))
// 		{
// 			goto Leave;
// 		}
// 		/* Now try to unmap again */
// 		status = ZwUnmapViewOfSection(NtCurrentProcess(), (PVOID)BaseAddress);
// 		if (!NT_SUCCESS(status))
// 		{
// 			goto Leave;
// 		}
	}


	/* Otherwise, return success */
	RetVal = TRUE;

Leave:
	return RetVal;
}

BOOLEAN CMapFileImp::MapFileIntoMem(
	_In_ PUNICODE_STRING FullFilePath,
	_Out_ PVOID* DataMemPtr,
	_Out_ ULONG* DataLength,
	_In_ BOOL ReadOnly
)
{
	return MapFileIntoMemEx(
		FullFilePath,
		DataMemPtr,
		DataLength,
		ReadOnly,
		FALSE,
		NULL
	);
}

BOOLEAN CMapFileImp::MapFileIntoMem(
	_In_ HANDLE FileHandle,
	_Out_ PVOID* DataMemPtr,
	_Out_ ULONG* DataLength,
	_In_ BOOL ReadOnly
)
{
	return MapFileIntoMemEx(
		FileHandle,
		DataMemPtr,
		DataLength,
		ReadOnly,
		FALSE,
		NULL
	);
}

BOOLEAN CMapFileImp::MapFileIntoMemEx(
	_In_ PUNICODE_STRING FullFilePath,
	_Out_ PVOID* DataMemPtr,
	_Out_ ULONG* DataLength,
	_In_ BOOL ReadOnly,
	_In_ BOOL Image,
	_Inout_ PVOID* SectionHandle
)
{
	BOOLEAN RetVal = FALSE;
	HANDLE FileHandle = NULL;

	ACCESS_MASK DesiredAccess = GENERIC_READ;
	ULONG ShareAccess = FILE_SHARE_READ;

	if (!ReadOnly)
	{
		DesiredAccess |= GENERIC_WRITE;
		ShareAccess |= FILE_SHARE_WRITE;
	}

	if (!UserKernelUtilsLib::IFileUtils::GetInstance()->OpenFile(
		FullFilePath,
		&FileHandle,
		DesiredAccess,
		ShareAccess
	))
	{
		goto Leave;
	}

	RetVal = MapFileIntoMemEx
	(
		FileHandle,
		DataMemPtr,
		DataLength,
		ReadOnly,
		Image,
		SectionHandle
	);

Leave:
	if (FileHandle)
	{
		ZwClose(FileHandle);
	}
	return RetVal;
}

BOOLEAN CMapFileImp::MapFileIntoMemEx(
	_In_ HANDLE FileHandle,
	_Out_ PVOID* DataMemPtr,
	_Out_ ULONG* DataLength,
	_In_ BOOL ReadOnly,
	_In_ BOOL Image,
	_Inout_ PVOID* SectionHandle
)
{
	BOOLEAN RetVal = FALSE;
	NTSTATUS status;

	PVOID DataMemPtrp = NULL;
	HANDLE SectHandle = NULL;
	LARGE_INTEGER FileSize;

	DWORD flProtect;
	DWORD DesiredAccess;
	SIZE_T ViewSize = 0;

	flProtect = ReadOnly ? PAGE_READONLY : PAGE_READWRITE;
	DesiredAccess = ReadOnly ? FILE_MAP_READ : FILE_MAP_READ | FILE_MAP_WRITE;

	if (Image)
	{
		flProtect = SEC_IMAGE | PAGE_READONLY;
	}

	if (!UserKernelUtilsLib::IFileUtils::GetInstance()->GetFileSize(
		FileHandle,
		&FileSize))
	{
		goto Leave;
	}

	if (Image)
	{
		FileSize.HighPart = 0;
		FileSize.LowPart = 0;
	}

	status = UserKernelUtilsLib::IMapFileInt::GetInstance()->CreateSection(
		&SectHandle,
		FileHandle,
		NULL,
		FALSE,
		flProtect,
		FileSize.HighPart,
		FileSize.LowPart,
		NULL
	);
	if (!NT_SUCCESS(status))
	{
		goto Leave;
	}

	status = UserKernelUtilsLib::IMapFileInt::GetInstance()->MapFile(
		&DataMemPtrp,
		SectHandle,
		ZwCurrentProcess(),
		DesiredAccess,
		0,
		0,
		&ViewSize
	);
	if (!NT_SUCCESS(status))
	{
		goto Leave;
	}

	*DataMemPtr = DataMemPtrp;
	*DataLength = (ULONG)ViewSize;
	if (FileSize.LowPart)
	{
		*DataLength = FileSize.LowPart;
	}

	RetVal = TRUE;
Leave:
	if (SectHandle)
	{
		if (SectionHandle)
		{
			*SectionHandle = SectHandle;
		}
		else
		{
			ZwClose(SectHandle);
		}
	}

	return RetVal;
}

PCHAR CMapFileImp::CopyFileToMemory(
	PUNICODE_STRING FullFilePath,
	size_t* Filelength
)
{
	PCHAR RetVal = NULL;

	PVOID DataMemPtrp = NULL;
	ULONG FileSize;

	if (!MapFileIntoMem(FullFilePath, &DataMemPtrp, &FileSize, TRUE))
	{
		ESL_DBG_OUT(DBG_ERROR, "Failed to map file %S", FullFilePath->Buffer);
		DataMemPtrp = NULL;
		goto Leave;
	}

	RetVal =
#ifdef _NTDDK_
		new('oCfM') char[FileSize];
#else
		new(std::nothrow) char[FileSize];
#endif

	if (!RetVal)
	{
		ESL_DBG_OUT(DBG_ERROR, "Failed to allocate memory for file data file %S", FullFilePath->Buffer);
		goto Leave;
	}

	RtlCopyMemory(RetVal, DataMemPtrp, FileSize);
	if (Filelength)
	{
		*Filelength = FileSize;
	}


Leave:

	if (DataMemPtrp)
	{
		UserKernelUtilsLib::IMapFileInt::GetInstance()->UnMapFile(DataMemPtrp, ZwCurrentProcess());
	}

	return RetVal;
}
