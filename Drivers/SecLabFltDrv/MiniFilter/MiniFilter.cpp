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

#include "MiniFilter.h"
#include <DriverEntryLib.h>

NTSTATUS FltGetFileName(
	_In_ PFLT_CALLBACK_DATA Data,
	_Out_ PUNICODE_STRING FileName
);

NTSTATUS GetSidStringFromToken(
	_In_ PACCESS_TOKEN Token,
	_Out_ PUNICODE_STRING SidString
);

extern "C" extern PULONG InitSafeBootMode;

FLT_OPERATION_REGISTRATION MiniFilter::CallBacks[] =
{
	{ IRP_MJ_CREATE, 0, MiniFilter::FilterPreCreate, MiniFilter::FilterPostCreate },
// 	{ IRP_MJ_WRITE, FLTFL_OPERATION_REGISTRATION_SKIP_PAGING_IO, MiniFilter::FilterPreWrite, NULL },
// 	{ IRP_MJ_CLEANUP, FLTFL_OPERATION_REGISTRATION_SKIP_PAGING_IO, MiniFilter::FilterPreCleanup, MiniFilter::FilterPostCleanup },
// 	{ IRP_MJ_FILE_SYSTEM_CONTROL, 0, MiniFilter::FilterSysCtrl, NULL },
// 	{ IRP_MJ_SET_INFORMATION, 0, MiniFilter::FilterSetInfo, NULL },


	{ IRP_MJ_OPERATION_END, 0, NULL, NULL },
};


MiniFilter::MiniFilter()
{
}

MiniFilter::~MiniFilter()
{
}

MiniFilter* MiniFilter::GetInstance()
{
	static MiniFilter Singletone;
	return &Singletone;
}

static
NTSTATUS
FLTAPI
InstanceSetupCallback(
	_In_ PCFLT_RELATED_OBJECTS    FltObjects,
	_In_ FLT_INSTANCE_SETUP_FLAGS Flags,
	_In_ DEVICE_TYPE              VolumeDeviceType,
	_In_ FLT_FILESYSTEM_TYPE      VolumeFilesystemType
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

	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(VolumeFilesystemType);

	/* Actually only disk volumes are of interest*/
	if (VolumeDeviceType == FILE_DEVICE_NETWORK_FILE_SYSTEM)
	{
		return STATUS_FLT_DO_NOT_ATTACH;
	}

	// this can be of interest to stop "ninja copy" of files or to check for ransom
	if (VolumeFilesystemType == FLT_FSTYPE_RAW)
	{
		return STATUS_FLT_DO_NOT_ATTACH;
	}

	return STATUS_SUCCESS;
}

NTSTATUS MiniFilter::Start()
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};
	PROC_ENTRY;

	NTSTATUS status;

	status = IBasicMiniFilter::GetInstance()->Start(
		CallBacks,
		NULL,
		InstanceSetupCallback,
		DriverEntryLib::G_DriverObject
	);
	if (!NT_SUCCESS(status))
	{
		goto Leave;
	}

	status = IBasicMiniFilter::GetInstance()->StartFiltering();

	// init filter context class here

Leave:
	return status;
}

NTSTATUS MiniFilter::Stop()
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};
	PROC_ENTRY;

	return IBasicMiniFilter::GetInstance()->Stop();
}


FLT_PREOP_CALLBACK_STATUS FLTAPI MiniFilter::FilterPreCreate(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PVOID*
)
{
	UNREFERENCED_PARAMETER(FltObjects);
	FLT_PREOP_CALLBACK_STATUS RetVal = FLT_PREOP_SUCCESS_WITH_CALLBACK;

	if (*InitSafeBootMode > 0)
	{
		RetVal = FLT_PREOP_SUCCESS_NO_CALLBACK;
		goto Leave;
	}

	if (Data->RequestorMode == KernelMode)
	{
		RetVal = FLT_PREOP_SUCCESS_NO_CALLBACK;
		goto Leave;
	}

	//TODO add a check if it is our service to avoid callback


Leave:
	return RetVal;
}

FLT_POSTOP_CALLBACK_STATUS
FLTAPI
MiniFilter::FilterPostCreate(
	PFLT_CALLBACK_DATA       Data,
	PCFLT_RELATED_OBJECTS    FltObjects,
	PVOID                    CompletionContext,
	FLT_POST_OPERATION_FLAGS Flags
)
{
	UNREFERENCED_PARAMETER(CompletionContext);

	NTSTATUS status;
	BOOLEAN isdir = FALSE;
	FILE_STANDARD_INFORMATION FileInformation;
	UNICODE_STRING FileName;
	FLT_POSTOP_CALLBACK_STATUS RetVal = FLT_POSTOP_FINISHED_PROCESSING;
	UNICODE_STRING OriginalUserSid;

	OriginalUserSid.Buffer = NULL;
	FileName.Buffer = NULL;

	if (Flags & FLTFL_POST_OPERATION_DRAINING)
	{
		return RetVal;
	}


	if (0
		|| !NT_SUCCESS(Data->IoStatus.Status)
		|| (STATUS_REPARSE == Data->IoStatus.Status)
		)
	{
		return RetVal;
	}

	if (KernelMode == Data->RequestorMode)
	{
		return RetVal;
	}

	if (FltObjects->FileObject == NULL)
	{
		ESL_DBG_OUT(DBG_INFO, "FltObjects->FileObject == NULL aborting)");
		goto Leave;
	}

	if (FltObjects->Instance == NULL)
	{
		ESL_DBG_OUT(DBG_INFO, "FltObjects->Instance == NULL aborting)");
		return RetVal;
	}

	FltGetFileName(
		Data,
		&FileName
	);
	if (!FileName.Buffer)
	{
		ESL_DBG_OUT(DBG_ERROR, "Failed to get filename");
		goto Leave;
	}

	if (FILE_OPENED != Data->IoStatus.Information)
	{
		ESL_DBG_OUT(DBG_INFO, "FileName %wZ flag FILE_OPENED not set, (may be created) ", &FileName);
		goto Leave;
	}

	/* If we are not connected then do nothing */

	/* If the originator is our service then do nothing */

	// TODO: DO NOT USE Data->Thread to identify the user (vulnerable to impersonation bypass).
	// Use the security Token (SubjectContext) instead.
	// Note: If the RPC server does not impersonate, the token will reflect SYSTEM; to capture the 
	// original user in those cases, an RPC hook (DLL Injection + Hooking) should be required in user mode.

	PACCESS_STATE AccessState = Data->Iopb->Parameters.Create.SecurityContext->AccessState;
	PSECURITY_SUBJECT_CONTEXT SubjectContext = &AccessState->SubjectSecurityContext;

	// Gets the effective token (impersonated or primary)
	PACCESS_TOKEN Token = SeQuerySubjectContextToken(SubjectContext);

	if(!NT_SUCCESS(GetSidStringFromToken(Token, &OriginalUserSid)))
	{
		OriginalUserSid.Buffer = NULL;
	}

	status = FltIsDirectory(
		FltObjects->FileObject,
		FltObjects->Instance,
		&isdir);
	if (NT_SUCCESS(status))
	{
		if (isdir)
		{
			//TODO a good place to add protection to our directories
			goto Leave;
		}
	}

	/* Called too frequent to put a trace here*/
	// 	ESL_DBG_OUT(DBG_INFO, ("Data->RequestorMode %d", Data->RequestorMode));
	// 	ESL_DBG_OUT(DBG_INFO, ("Data->Iopb.TargetFileObject %d", Data->Iopb->TargetFileObject));
	// 	ESL_DBG_OUT(DBG_INFO, ("Data->IoStatus.Information %d", Data->IoStatus.Information));


	// TODO use the FileInternInformation.IndexNumber.QuadPart to identify files that are protected. 
	// We can add the to a list of ID at DriverEntry
// 
// 	FILE_INTERNAL_INFORMATION FileInternInformation;
// 	FileInternInformation.IndexNumber.QuadPart = FILE_INVALID_FILE_ID;
// 
// 
// 	status = FltQueryInformationFile(
// 		FltObjects->Instance,
// 		FltObjects->FileObject,
// 		&FileInternInformation,
// 		sizeof(FILE_INTERNAL_INFORMATION),
// 		FileInternalInformation,
// 		NULL
// 	);
// 
// 	if (!NT_SUCCESS(status))
// 	{
// 		FileInternInformation.IndexNumber.QuadPart = FILE_INVALID_FILE_ID;
// 	}
// 
// 	if (IsProtectedFile(
// 		FileInternInformation.IndexNumber.QuadPart,
// 		Data,
// 		FltObjects
// 	))
// 	{
// 		return RetVal;
// 	}

	status = FltQueryInformationFile(
		FltObjects->Instance,
		FltObjects->FileObject,
		&FileInformation,
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation,
		NULL
	);
	if (!NT_SUCCESS(status))
	{
		ESL_DBG_OUT(DBG_ERROR, "FileStandardInformation file %wZ failed status = 0x%08X we don't have the Filesize...  aborting", &FileName, status);
		goto Leave;
	}


	if (0 == FileInformation.EndOfFile.QuadPart)
	{
		ESL_DBG_OUT(DBG_INFO, "file %wZ is zero sized ignoring", &FileName);
		goto Leave;
	}

	//TODO call user mode

Leave:

	if (FileName.Buffer) delete [] FileName.Buffer;
	if (OriginalUserSid.Buffer)
	{
		RtlFreeUnicodeString(&OriginalUserSid);
	}

	return RetVal;
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// some utils should move to a common lib

static
NTSTATUS
FilterGetFileNameInformation(
	PFLT_CALLBACK_DATA Data,
	PFLT_FILE_NAME_INFORMATION* nameInfo
)
{
	NTSTATUS status;

	status = FltGetFileNameInformation(
		Data,
		0
		| FLT_FILE_NAME_NORMALIZED
		| FLT_FILE_NAME_QUERY_DEFAULT
		,
		nameInfo);
	if (!NT_SUCCESS(status))
	{
		ESL_DBG_OUT(DBG_INFO, "FltGetFileNameInformation failed FLT_FILE_NAME_NORMALIZED 0x%08X", status);
		status = FltGetFileNameInformation(
			Data,
			0
			| FLT_FILE_NAME_OPENED
			| FLT_FILE_NAME_QUERY_DEFAULT
			,
			nameInfo);
		if (!NT_SUCCESS(status))
		{
			ESL_DBG_OUT(DBG_INFO, "FltGetFileNameInformation failed FLT_FILE_NAME_QUERY_DEFAULT 0x%08X", status);
		}
	}
	return status;
}

static
NTSTATUS
NTAPI
CaptureUnicodeString(
	PUNICODE_STRING DestinationString,
	PCUNICODE_STRING SourceString
)
{
	__try
	{
		DestinationString->Length = SourceString->Length;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return GetExceptionCode();
	}

	DestinationString->MaximumLength =
		DestinationString->Length + sizeof(UNICODE_NULL);
	DestinationString->Buffer = new('RTSU') WCHAR[DestinationString->MaximumLength];

	if (!DestinationString->Buffer)
		return STATUS_INSUFFICIENT_RESOURCES;

	__try
	{
		RtlCopyMemory(
			DestinationString->Buffer,
			SourceString->Buffer,
			DestinationString->Length);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		delete[]  DestinationString->Buffer;
		return GetExceptionCode();
	}

	DestinationString->Buffer[DestinationString->Length / sizeof(UNICODE_NULL)] = UNICODE_NULL;

	return STATUS_SUCCESS;
}

static WCHAR NoName[] = L"NoName";
static UNICODE_STRING UNoName = { sizeof(NoName), sizeof(NoName), NoName };

NTSTATUS FltGetFileName(
	_In_ PFLT_CALLBACK_DATA Data,
	_Out_ PUNICODE_STRING FileName
)
{
	NTSTATUS status;
	PFLT_FILE_NAME_INFORMATION nameInfo;

	if (!Data)
	{
		return  CaptureUnicodeString(FileName, &UNoName);
	}

	status = FilterGetFileNameInformation(
			Data,
			&nameInfo);

	if (NT_SUCCESS(status))
	{
		status = CaptureUnicodeString(FileName, &nameInfo->Name);
		FltReleaseFileNameInformation(nameInfo);
		return status;
	}

	return CaptureUnicodeString(FileName, &UNoName);
}

NTSTATUS GetSidStringFromToken(
	_In_ PACCESS_TOKEN Token,
	_Out_ PUNICODE_STRING SidString
)
{
	NTSTATUS Status;
	PTOKEN_USER UserToken = NULL;

	// 1. Query user information inside the token
	Status = SeQueryInformationToken(Token, TokenUser, (PVOID*)&UserToken);
	if (!NT_SUCCESS(Status)) {
		return Status;
	}

	// 2. Convert the numeric SID to characters (Text)
	Status = RtlConvertSidToUnicodeString(SidString, UserToken->User.Sid, TRUE);

	// Free the memory allocated by SeQueryInformationToken
	ExFreePool(UserToken);

	return Status;
}

static
NTSTATUS _FileHandle(
	PCFLT_RELATED_OBJECTS FltObjects,
	PUNICODE_STRING FileName,
	PHANDLE FileHandle
)
{
	NTSTATUS status = STATUS_SUCCESS;
	OBJECT_ATTRIBUTES objAttr = { 0 };
	IO_STATUS_BLOCK iosb = { 0 };

	InitializeObjectAttributes(&objAttr, FileName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = FltCreateFile(
		FltObjects->Filter,
		FltObjects->Instance,
		FileHandle,
		FILE_GENERIC_READ,
		&objAttr,
		&iosb,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_OPEN,
		FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_COMPLETE_IF_OPLOCKED,
		NULL,
		0,
		IO_IGNORE_SHARE_ACCESS_CHECK
	);

	return status;
}

NTSTATUS FltGetFileHandle(
	IN  PCFLT_RELATED_OBJECTS FltObjects,
	IN  PUNICODE_STRING FileName,
	IN  HANDLE TargetProcess,
	OUT PHANDLE FileHandle
)
{
	NTSTATUS status = STATUS_SUCCESS;
	HANDLE SystemProcessFileHandle = NULL;
	HANDLE OrigHandle = NULL;

	ObOpenObjectByPointer(
		PsInitialSystemProcess,
		OBJ_KERNEL_HANDLE,
		NULL,
		STANDARD_RIGHTS_READ,
		NULL,
		KernelMode,
		&SystemProcessFileHandle
	);

	status = _FileHandle(FltObjects, FileName, &OrigHandle);

	if (!NT_SUCCESS(status))
	{
		goto Leave;
	}

	status = ZwDuplicateObject(
		SystemProcessFileHandle,
		OrigHandle,
		TargetProcess,
		FileHandle,
		FILE_GENERIC_READ,
		0,
		DUPLICATE_SAME_ATTRIBUTES | DUPLICATE_SAME_ACCESS
	);

Leave:
	if (OrigHandle != NULL)
	{
		FltClose(OrigHandle);
	}
	if (SystemProcessFileHandle)
	{
		ZwClose(SystemProcessFileHandle);
	}

	return status;
}
