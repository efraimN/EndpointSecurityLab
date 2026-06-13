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

#include "SecLabDlpClassicDriverObject.h"
#include "SecLabDlpClassic.h"

BOOLEAN G_ShouldStopWpp = TRUE;

static
VOID
__cdecl
StartDebuggerTracer(
	PDRIVER_OBJECT DriverObject,
	PUNICODE_STRING RegistryPath
)
{
	WPP_INIT_TRACING(DriverObject, RegistryPath);
}

VOID
__cdecl
StopDebuggerTracer(
	PDRIVER_OBJECT DriverObject
)
{
	if (G_ShouldStopWpp)
	{
		WPP_CLEANUP(DriverObject);
	}
}

extern "C"
NTSTATUS
DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath
)
{
	return DriverEntryLib::Main(
		DriverObject, 
		RegistryPath, 
		ORIGINALFILENAME,
		VER_FILEVERSION_STR,
		CMyDriverObject::GetInstance,
		StartDebuggerTracer,
		StopDebuggerTracer
	);
}

