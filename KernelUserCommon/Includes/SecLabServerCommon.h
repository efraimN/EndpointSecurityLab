#pragma once


#define SEC_LAB_SERVER_PORT_NAME L"\\Lpc_SEC_LAB_Port"

enum KernelMessagesToUser
{
	SecLabTset = 1,
	GetLpcInBlacList,

};


typedef struct _LPC_SEC_LAB_SERVER_MESSAGE64
{
	WCHAR Text[64];

} LPC_SEC_LAB_SERVER_MESSAGE64, * PLPC_SEC_LAB_SERVER_MESSAGE64;

typedef struct _Get_LPC_Block_Status
{
	WCHAR HardwareId[128];
	BOOL ShouldBlock; // TRUE Blocked
}Get_LPC_Block_Status, *PGet_LPC_Block_Status;





typedef struct _MessagesToUser 
{
	LPC_BASIC_MESSAGE64 BasicMessage;
	KernelMessagesToUser MesageType;
	union 
	{
		LPC_SEC_LAB_SERVER_MESSAGE64 LPCSecLabServerMessage;
		Get_LPC_Block_Status GetLPCBlocStatus;
	}Messages;
}MessagesToUser, *PMessagesToUser;

#define SIZE_OF_LCP_MESSAGE sizeof(MessagesToUser)