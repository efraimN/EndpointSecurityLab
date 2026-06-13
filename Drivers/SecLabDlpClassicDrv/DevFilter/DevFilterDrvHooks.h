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
class DevFilterDrvHooksManager;

class DevFilterDrvHooks
{
public:
	friend DevFilterDrvHooksManager;
	
	BOOL
	Start();
	
	void
	Stop();
	
	DevFilterDrvHooks();
	virtual ~DevFilterDrvHooks();
protected:

private:
	NTSTATUS
	_MajorFunctionDispatcher(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

	static
	NTSTATUS
	MajorFunctionDispatcher(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

	PDRIVER_OBJECT  m_DriverObject;
	WCHAR m_DriverName[50];
	PDRIVER_DISPATCH m_MajorFunctionDispatcher;
};

//////////////////////////////////////////////////////////////////////////

class DevFilterDrvHooksManager
{
public:
	static
	DevFilterDrvHooksManager* GetInstance();

	BOOL
	Init(
		PWCHAR* ClassGuidArray
	);

	BOOL
	AddMajorFunctionDispatcher(
		_In_ PWSTR DriverName,
		_In_ PDRIVER_DISPATCH MajorFunctionDispatcher
	);

	void
	Start();

	void
	Stop();

	DevFilterDrvHooks*
	FindByDeviceObject(
		_In_ PDEVICE_OBJECT DeviceObject
	);

protected:

private:
	DevFilterDrvHooksManager();
	virtual ~DevFilterDrvHooksManager();

	DevFilterDrvHooksManager(const DevFilterDrvHooksManager& other);
	DevFilterDrvHooksManager& operator=(const DevFilterDrvHooksManager& other);

	DevFilterDrvHooks** m_DevFilterDrvHooksArray;
	DWORD m_NumberOfHooks;
};