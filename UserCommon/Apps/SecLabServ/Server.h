#pragma once

#include <ILpcLibServ.h>
#include <SecLabServerCommon.h>

class MyServerCallbacks : public ILpcServReceiverHandler
{
public:
	virtual BOOLEAN AcceptConnect(PLPC_BASIC_MESSAGE PortMessage);
	virtual VOID HandleDataRequest(PLPC_BASIC_MESSAGE PortMessage);
};

