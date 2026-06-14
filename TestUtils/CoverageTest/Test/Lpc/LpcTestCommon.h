#pragma once

#include <LpcLibCommonInc.h>

#define LPC_TEST_PORT_NAME L"\\LpcTestPort"

typedef struct _LPC_TEST_MESSAGE64
{
	LPC_BASIC_MESSAGE64 BasicMessage;
	WCHAR Text[64];

} LPC_TEST_MESSAGE64, * PLPC_TEST_MESSAGE64;

