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

#include <DriverEntryLib.h>
#include "SecLabFltDrvDriverObject.h"

#include <ISendToService.h>

extern "C" PULONG InitSafeBootMode;

IDriverObject* IDriverObject::GetInstance()
{
	static CMyDriverObject SingleTone;
	return &SingleTone;
}

NTSTATUS CMyDriverObject::DoStartStop(BOOL Start_Stop)
{
	NTSTATUS RetVal = STATUS_INSUFFICIENT_RESOURCES;

	auto Start = [&]()->BOOL
	{
		RetVal = FALSE;
		/* Add initialization for each module used */

		if (!ISendToService::GetInstance()->Start())
		{
			goto Leave;
		}

		RetVal = TRUE;
	Leave:
		return RetVal;
	};

	auto Stop = [&]()
	{
		ISendToService::GetInstance()->Stop();
	};

	if (*InitSafeBootMode > 0)
	{
		return STATUS_SUCCESS;
	}

	if (Start_Stop)
	{
		if (!Start())
		{
			Stop();
			goto Leave;
		}
	}
	else
	{
		Stop();
	}

	RetVal = STATUS_SUCCESS;
Leave:
	return RetVal;
	 
}

NTSTATUS
CMyDriverObject::MajorFunctionDispatcher(
	IN PDEVICE_OBJECT /*DeviceObject*/,
	IN PIRP Irp)
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};
	PROC_ENTRY;


	ESL_DBG_OUT(DBG_ERROR, ("Returning STATUS_INVALID_DEVICE_REQUEST the DeviceObject is invalid"));
	Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_INVALID_DEVICE_REQUEST;
}

