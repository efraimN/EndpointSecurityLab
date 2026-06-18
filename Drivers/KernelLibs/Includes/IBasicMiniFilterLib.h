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

class IBasicMiniFilter
{
public:

	static IBasicMiniFilter* GetInstance();

	virtual
	NTSTATUS
	Start(
		const FLT_OPERATION_REGISTRATION callbacks[],
		const FLT_CONTEXT_REGISTRATION ContextRegistration[],
		PFLT_INSTANCE_SETUP_CALLBACK instanceSetupCallback,
		PDRIVER_OBJECT DriverObject
	) = 0;

	virtual
	NTSTATUS StartFiltering() = 0;

	virtual
	NTSTATUS Stop() = 0;


	virtual
	PFLT_FILTER  GetFilterHandle() = 0;


protected:
	IBasicMiniFilter() {};
	~IBasicMiniFilter() {};
private:
	// delete copy and move constructors and assign operators
	IBasicMiniFilter(IBasicMiniFilter const&);             // Copy construct
	IBasicMiniFilter(IBasicMiniFilter&&);                  // Move construct
	IBasicMiniFilter& operator=(IBasicMiniFilter const&);  // Copy assign
	IBasicMiniFilter& operator=(IBasicMiniFilter&&);       // Move assign
};

