#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct _LPC_BASIC_MESSAGE32
	{
		PORT_MESSAGE32 MessageHeader;
		/*
		add here any member that will be used in all messages,
		but remember to add it to the LPC_BASIC_MESSAGE64
		*/
    }LPC_BASIC_MESSAGE32, * PLPC_BASIC_MESSAGE32;

	typedef struct _CONNECT_MESSAGE32
	{
		PORT_MESSAGE32 MessageHeader;
		DWORD ConnectionInformation;
	}CONNECT_MESSAGE32, *PCONNECT_MESSAGE32;

	typedef struct _LPC_BASIC_MESSAGE64
	{
		PORT_MESSAGE64 MessageHeader;
		/*
		add here any member that will be used in all messages,
		but remember to add it to the LPC_BASIC_MESSAGE32
		*/
	}LPC_BASIC_MESSAGE64, * PLPC_BASIC_MESSAGE64;

	typedef struct _CONNECT_MESSAGE64
	{
		PORT_MESSAGE64 MessageHeader;
		DWORD ConnectionInformation;
	}CONNECT_MESSAGE64, *PCONNECT_MESSAGE64;

	typedef struct _LPC_BASIC_MESSAGE *PLPC_BASIC_MESSAGE;
	typedef struct _CONNECT_MESSAGE *PCONNECT_MESSAGE;

#ifdef __cplusplus
}
#endif

