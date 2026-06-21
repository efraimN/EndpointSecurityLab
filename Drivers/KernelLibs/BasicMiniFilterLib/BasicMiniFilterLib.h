/*
Copyright(c) 2026 - present, Ephraim Neuberger.All rights reserved.

The code and materials provided by Ephraim Neuberger are for non - commercial testing and evaluation purposes only.
Ephraim Neuberger reserves all rights not expressly granted.

Any permitted copy must retain this copyright notice and disclaimer.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL EPHRAIM NEUBERGER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <IBasicMiniFilterLib.h>


class BasicMiniFilter : public IBasicMiniFilter
{
public:

	friend IBasicMiniFilter;

	virtual
	NTSTATUS
	Start(
		const FLT_OPERATION_REGISTRATION callbacks[],
		const FLT_CONTEXT_REGISTRATION ContextRegistration[],
		PFLT_INSTANCE_SETUP_CALLBACK instanceSetupCallback,
		PDRIVER_OBJECT DriverObject
	);

	virtual
	NTSTATUS StartFiltering();

	virtual
	NTSTATUS Stop();


	virtual
	PFLT_FILTER  GetFilterHandle();


private:

	// delete copy and move constructors and assign operators
	BasicMiniFilter(BasicMiniFilter const&);             // Copy construct
	BasicMiniFilter(BasicMiniFilter&&);                  // Move construct
	BasicMiniFilter& operator=(BasicMiniFilter const&);  // Copy assign
	BasicMiniFilter& operator=(BasicMiniFilter&&);       // Move assign
	BasicMiniFilter();
	~BasicMiniFilter();

	static
	NTSTATUS
	FLTAPI
	UnloadCallback(
		FLT_FILTER_UNLOAD_FLAGS Flags
	);

	PFLT_FILTER m_FilterHandle;

	PDRIVER_OBJECT m_DriverObject;
	PDRIVER_UNLOAD m_DriverUnload;
	BOOL m_Inited;
};
