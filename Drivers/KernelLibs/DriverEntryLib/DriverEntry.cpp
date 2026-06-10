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

#include "DriverObject.h"
#include <DriverEntryLib.h>

#include <KernelUtilsLib.h>

static
void DriverExit(IN PDRIVER_OBJECT DriverObject);
static
void DriverExitInternal(IN PDRIVER_OBJECT DriverObject);

PDRIVER_OBJECT DriverEntryLib::G_DriverObject;

static
DriverEntryLib::pfnStopDebuggerTracer s_StopDebuggerTracer;

NTSTATUS
DriverEntryLib::Main(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath,
	IN decltype(IDriverObject::GetInstance)* DriverObjectInstance,
	pfnStartDebuggerTracer StartDebuggerTracer,
	pfnStopDebuggerTracer StopDebuggerTracer
)
{
	NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;
	{
#if 0
		KdRefreshDebuggerNotPresent();
		if (KD_DEBUGGER_ENABLED && !KD_DEBUGGER_NOT_PRESENT)
		{
			[]() {
				// if a kernel debugger is attached the following will break into the debugger.
				__try
				{
					KdPrint((EXE_FILE_NAME)); KdPrint((" : "));  KdPrint((" ")); KdPrint(("This breakpoint is executed because a debugger is attached. you can safelly ignore it\n"));
					KdBreakPoint();
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
				}
			}();
		}
#endif
		G_DriverObject = DriverObject;
		s_StopDebuggerTracer = StopDebuggerTracer;

		DriverObject->DriverUnload = DriverExit;
		ExInitializeDriverRuntime(DrvRtPoolNxOptIn);

		StartDebuggerTracer(DriverObject, RegistryPath);

		auto ProcEntry = [](BOOL start)
		{
			if (start)
				ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
			else
				ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

		};
		PROC_ENTRY;

		ESL_DBG_OUT(DBG_INFO, "Driver Name %s Driver Build Date: %s Time: %s Version %s", ORIGINALFILENAME, __DATE__, __TIME__, VER_FILEVERSION_STR);

		CrtInit();
		
		status = DriverObjectImp::GetInstance()->DriverObjectMain(RegistryPath, DriverObjectInstance());
		if (!NT_SUCCESS(status))
		{
			goto Leave;
		}

		status = STATUS_SUCCESS;
	Leave:
		if (!NT_SUCCESS(status))
		{
			DriverExitInternal(DriverObject);
		}
	}
	if (!NT_SUCCESS(status))
	{
		s_StopDebuggerTracer(DriverObject);
	}
	return status;
};

static
void DriverExitInternal(IN PDRIVER_OBJECT DriverObject)
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};
	PROC_ENTRY;

	UNREFERENCED_PARAMETER(DriverObject);
	ESL_DBG_OUT(DBG_INFO, "DriverExit: Going to call CDriverObject::GetInstance()->DriverExit()");


	DriverObjectImp::GetInstance()->DriverExit();

	CrtExit();
	ESL_DBG_OUT(DBG_INFO, "------- " ORIGINALFILENAME " Driver Exit Finished-------- - ");
}

static
void DriverExit(IN PDRIVER_OBJECT DriverObject)
{

	{
		auto ProcEntry = [](BOOL start)
		{
			if (start)
				ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
			else
				ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

		};
		PROC_ENTRY;

		DriverExitInternal(DriverObject);
	}

	s_StopDebuggerTracer(DriverObject);
}
