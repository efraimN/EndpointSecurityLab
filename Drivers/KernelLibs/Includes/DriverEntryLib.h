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

#pragma once

// Each driver must implement this class
class IDriverObject
{
public:

	static
	IDriverObject *IDriverObject::GetInstance();

	virtual
	NTSTATUS
	MajorFunctionDispatcher(
		IN PDEVICE_OBJECT DeviceObject,
		IN PIRP Irp
	) = 0;

	virtual
	NTSTATUS DoStartStop(
		BOOL Start_Stop
	) = 0;

protected:
	IDriverObject() {};
	virtual ~IDriverObject() {};
};

// Implement a class from BasicDriverDevice if a device is needed
class BasicDriverDevice
{
public:

	static
	void* __cdecl operator new(size_t nSize, PDRIVER_OBJECT DriverObject, BOOLEAN Exclusive = FALSE, wchar_t* pDeviceName = NULL, DEVICE_TYPE Type = FILE_DEVICE_UNKNOWN);
	void __cdecl operator delete(void* p);
	BasicDriverDevice(UINT32 Signature);
	virtual ~BasicDriverDevice();

	virtual
	NTSTATUS
	MajorFunctionDispatcher(
		IN PIRP Irp
	) = 0;

	UINT32 m_Signature;
	PDEVICE_OBJECT m_pDeviceObject;      // Ptr to the device object

protected:
};

// This is the driver entry code
namespace DriverEntryLib
{
	// To be implemented in the driver code. will be called by the lib
	typedef
	VOID
	(__cdecl *pfnStartDebuggerTracer)(
		PDRIVER_OBJECT DriverObject,
		PUNICODE_STRING RegistryPath
	);

	// To be implemented in the driver code. will be called by the lib
	typedef
	VOID
	(__cdecl *pfnStopDebuggerTracer)(
		PDRIVER_OBJECT DriverObject
	);

	// To be called from the driver code. It is implemented on the lib
	NTSTATUS
	Main(
		IN PDRIVER_OBJECT DriverObject,
		IN PUNICODE_STRING RegistryPath,
		IN PCHAR DriverFileName,
		IN PCHAR DriverFileVersion,
		// a pointer to the driver implemented IDriverObject::GetInstance function
		IN decltype(IDriverObject::GetInstance)* DriverObjectInstance,
		pfnStartDebuggerTracer StartDebuggerTracer,
		pfnStopDebuggerTracer StopDebuggerTracer
	);

	extern PDRIVER_OBJECT G_DriverObject;
};
