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

#include "DevFilter.h"

#include <IRegUtils.h>

#include "DevFilterUtils.h"

static PWCHAR g_InstallerClassArray[] =
{
		L"{36fc9e60-c465-11cf-8056-444553540000}",
		NULL,
};


DevFilter::DevFilter()
{
}

DevFilter::~DevFilter()
{
}

DevFilter* DevFilter::GetInstance()
{
	static DevFilter SingleTone;
	return &SingleTone;
}

BOOL
DevFilter::Start(
)
{
	BOOL RetVal = FALSE;

	if(!DevFilterDrvHooksManager::GetInstance()->Init(g_InstallerClassArray))
	{
		goto Leave;
	}
	/*
	* if required call here 
	DevFilterDrvHooksManager::GetInstance()->AddMajorFunctionDispatcher(
		_In_ PWSTR DriverName,
		_In_ PDRIVER_DISPATCH MajorFunctionDispatcher
	);

	*/
	DevFilterDrvHooksManager::GetInstance()->Start();
	RetVal = TRUE;
Leave:
	return RetVal;
}

void DevFilter::Stop()
{
	DevFilterDrvHooksManager::GetInstance()->Stop();
	return;
}
