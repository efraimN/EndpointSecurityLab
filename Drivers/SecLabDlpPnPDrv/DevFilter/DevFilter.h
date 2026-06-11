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

#include <DriverEntryLib.h>

#define MY_FILTER_DEVICES_SIGNATURE 0x4BADCAFE
class DevFilter : public BasicDriverDevice
{
public:

	// we will fail the add device if this device is not listed
	static
	NTSTATUS
	AddDevice(
		IN PDRIVER_OBJECT,
		IN PDEVICE_OBJECT   PDO
	);

	virtual NTSTATUS MajorFunctionDispatcher(IN PIRP Irp);

	DevFilter(UINT32 Signature, PWCHAR HardwareID, PDEVICE_OBJECT PDO);
	virtual ~DevFilter();

	void* __cdecl operator new(size_t nSize, PDRIVER_OBJECT DriverObject, DEVICE_TYPE Type);
	void __cdecl operator delete(void* p);


protected:

private:

	static
	PWCHAR
	GetDeviceInformation(
		PDEVICE_OBJECT PDO,
		DEVICE_REGISTRY_PROPERTY Property,
		PULONG PropertyStringSize
	);

	static
	BOOL
	AttachToDevice(
		IN PDEVICE_OBJECT PDO,
		PWCHAR HardwareID,
		BOOL LearnMode
	);

	virtual
	NTSTATUS
	Default_IRP_Dispatcher(
		IN PIRP Irp
	);

	static
	NTSTATUS
	StartDeviceCompletition(
		IN PDEVICE_OBJECT DeviceObject,
		IN PIRP Irp,
		IN PVOID Context
	);

	virtual
	NTSTATUS
	PnPDispatch(
		IN PIRP Irp,
		IN UCHAR MinorFunction
	);

	PWCHAR m_HardwareID;
	BOOL m_Blocked;
	BOOL m_LearnMode;

	//
	// "THE PDO"  (ejected by the root bus or ACPI)
	//
	PDEVICE_OBJECT  m_PDO;

	//
	// The top of the stack before this filter was added.  AKA the location
	// to which all IRPS should be directed.
	//
	PDEVICE_OBJECT  m_TopOfStackBeforeUs; // i.e. next lower driver
};
