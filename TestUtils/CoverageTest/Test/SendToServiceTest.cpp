/*
Copyright(c) 2026 - present, Ephraim Neuberger.All rights reserved.

The code and materials provided by Ephraim Neuberger are for non - commercial testing and evaluation purposes only.
Ephraim Neuberger reserves all rights not expressly granted.

Any permitted copy must retain this copyright notice and disclaimer.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL EPHRAIM NEUBERGER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include <WppIncludes.h>

#include <ISendToService.h>

NTSTATUS GetProcessHandle(
	IN  HANDLE ProcessId,
	OUT PHANDLE ProcessHandle
);

NTSTATUS GetFileHandle(
	IN  PUNICODE_STRING FileName,
	IN  HANDLE TargetProcess,
	OUT PHANDLE FileHandle
);

VOID ConVert2DosDev(PCHAR InputFile, PCHAR* Converted2DevPath);

/*!
Usage:
CoverageTest SendToServiceUsbMessage VidPid

Example:
CoverageTest SendToServiceUsbMessage VID_045E&PID_0745



*/
int TEST_FUNCTION SendToServiceUsbMessage(char* VidPid)
{
	int RetVal = -1;
	MessagesToUser Message;
	PGet_USB_Block_Status pGetUsbBlockStatus;
	ISendToService* Client;
	WCHAR wVidPid[128];

	RtlZeroMemory(&Message, sizeof(Message));

	pGetUsbBlockStatus = &Message.UserMessage.Messages.GetUsbBlocStatus;

	Client = ISendToService::GetInstance();
	if (!Client->Start())
	{
		printf("SendToService Start Failed");
		goto Leave;
	}

	printf("SendToService started");

	wsprintfW(wVidPid, L"%S", VidPid);
	wcscpy(pGetUsbBlockStatus->HardwareId, wVidPid);

	if(!Client->SendMessage(
		GetUsbBlocStatusMessage,
		&Message,
		TRUE
	))
	{
		printf("SendToService SendMessage Failed");
		goto Leave;
	}


	printf("SendToService SendMessage Done;");

	RetVal = 0;
Leave:
	Client->Stop();
	if (!RetVal)
	{
		printf("TEST SUCCESS\n");
	}
	else
	{
		printf("TEST FAILED\n");
	}

	return RetVal;
}

/*!
Usage:
CoverageTest SendToServiceOpenFileMessage VidPid

Example:
CoverageTest SendToServiceUsbMessage VID_045E&PID_0745



*/
int TEST_FUNCTION SendToServiceOpenFileMessage(char* FullFileName)
{
	int RetVal = -1;
	MessagesToUser Message;
	PGet_Open_File_Veredict pGetOpenFileVeredict;
	ISendToService* Client;
	WCHAR vSid[128] = L"S-1-5-21-XXXX-XXXX-XXXX-500";
	ULONG_PTR  ServerPid;
	HANDLE ServerProcHandle = NULL;
	HANDLE FileHandle = NULL;
	PCHAR Converted2DevPath = NULL;
	WCHAR wFullFileName[256];
	UNICODE_STRING uFullFileName;

	RtlZeroMemory(&Message, sizeof(Message));

	pGetOpenFileVeredict = &Message.UserMessage.Messages.GetOpenFileVeredict;

	Client = ISendToService::GetInstance();
	if (!Client->Start())
	{
		printf("SendToService Start Failed\n");
		goto Leave;
	}
	
	printf("SendToService started\n");

	ServerPid = Client->GetServerPid();

	if (!NT_SUCCESS(GetProcessHandle((HANDLE)ServerPid, &ServerProcHandle)))
	{
		printf("GetProcessHandle Failed for processes pid %lld \n", ServerPid);
		goto Leave;
	}


	ConVert2DosDev(FullFileName, &Converted2DevPath);
	if (!Converted2DevPath)
	{
		printf("ConVert2DosDev Failed for file %s \n", FullFileName);
		goto Leave;
	}

	wsprintf(wFullFileName, L"%S", Converted2DevPath);

	RtlInitUnicodeString(&uFullFileName, wFullFileName);

	if (!NT_SUCCESS(GetFileHandle(
		&uFullFileName,
		ServerProcHandle,
		&FileHandle
	)))
	{
		printf("GetFileHandle Failed for file %s \n", FullFileName);
		goto Leave;
	}

	pGetOpenFileVeredict->FileHandle = (ULONG64)FileHandle;
	wcscpy(pGetOpenFileVeredict->SidString, vSid);

	if (!Client->SendMessage(
		GetOpenFileVeredictMessage,
		&Message,
		TRUE
	))
	{
		printf("SendToService SendMessage Failed\n");
		goto Leave;
	}
	
	printf("SendToService SendMessage Done\n");


	RetVal = 0;
Leave:
	Client->Stop();
	if (ServerProcHandle)
	{
		ZwClose(ServerProcHandle);
	}
	if (!RetVal)
	{
		printf("TEST SUCCESS\n");
	}
	else
	{
		printf("TEST FAILED\n");
	}

	if (Converted2DevPath)
	{
		delete[] Converted2DevPath;
	}
	return RetVal;
}


//////////////////////////////////////////////////////////////////////////
// Following some utils, that should eventually go to the utilslib

static
NTSTATUS _FileHandle(
	PUNICODE_STRING FileName,
	PHANDLE FileHandle
)
{
	OBJECT_ATTRIBUTES objAttr = { 0 };
	IO_STATUS_BLOCK iosb = { 0 };

	InitializeObjectAttributes(
		&objAttr,
		FileName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL
	);

	return ZwOpenFile(
		FileHandle,
		FILE_GENERIC_READ,
		&objAttr,
		&iosb,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_COMPLETE_IF_OPLOCKED
	);
}

NTSTATUS GetFileHandle(
	IN  PUNICODE_STRING FileName,
	IN  HANDLE TargetProcess,
	OUT PHANDLE FileHandle
)
{
	NTSTATUS status = STATUS_SUCCESS;
	HANDLE OrigHandle = NULL;

	status = _FileHandle(FileName, &OrigHandle);

	if (!NT_SUCCESS(status))
	{
		goto Leave;
	}

	status = ZwDuplicateObject(
		ZwCurrentProcess(),
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
		ZwClose(OrigHandle);
	}

	return status;
}


NTSTATUS GetProcessHandle(
	IN  HANDLE ProcessId,
	OUT PHANDLE ProcessHandle
)
{
	OBJECT_ATTRIBUTES objAttr = { 0 };
	CLIENT_ID clientId = { 0 };

	InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);

	clientId.UniqueProcess = ProcessId;
	clientId.UniqueThread = NULL;

	return ZwOpenProcess(
		ProcessHandle,
		PROCESS_DUP_HANDLE,
		&objAttr,
		&clientId
	);
}

