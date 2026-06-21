#pragma once


#define SEC_LAB_SERVER_PORT_NAME L"\\Lpc_SEC_LAB_Port"
#define USER_BUFFERS_REQUEST_COUNT 10

#include <LpcLibCommonInc.h>

enum KernelMessagesToUser
{
	GetUsbBlocStatusMessage = 1,
	GetOpenFileVeredictMessage,
};


typedef struct _Get_USB_Block_Status
{
	WCHAR HardwareId[128];
	BOOL ShouldBlock; // TRUE Blocked
}Get_USB_Block_Status, *PGet_USB_Block_Status;

typedef struct _Get_Open_File_Veredict
{
	WCHAR SidString[128];
	ULONG64 FileHandle; // didn't used HANDLE coz HANDLE size varies between 32-64
	BOOL ShouldBlock; // TRUE Blocked
}Get_Open_File_Veredict, *PGet_Open_File_Veredict;

typedef struct _User_Message
{
	KernelMessagesToUser MesageType;
	BOOL Result;
	union
	{
		Get_USB_Block_Status GetUsbBlocStatus;
		Get_Open_File_Veredict GetOpenFileVeredict;
	} Messages;
} User_Message, *PUser_Message;

typedef struct _MessagesToUser 
{
	LPC_BASIC_MESSAGE64 BasicMessage;
	User_Message UserMessage;
}MessagesToUser, *PMessagesToUser;

