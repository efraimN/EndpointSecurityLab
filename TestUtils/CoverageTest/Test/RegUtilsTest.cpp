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

#include <IRegUtils.h>

using namespace UserKernelUtilsLib;

/*!
Usage:
CoverageTest TestAddMultiStringToKey

Do not use

for debug purposes

*/
int TEST_FUNCTION TestAddMultiStringToKey()
{
	NTSTATUS status;
	IRegUtils* pRegUtilsInt;
	pRegUtilsInt = IRegUtils::GetInstance();

	status = pRegUtilsInt->AddMultiStringToKey(
		RTL_REGISTRY_ABSOLUTE,
		L"\\REGISTRY\\MACHINE\\SYSTEM\\ControlSet001\\services\\APDDRVX",
		L"test",
		NULL,
		L"testqweerty"
	);

	status = pRegUtilsInt->RemoveMultiStringFromKey(
		RTL_REGISTRY_ABSOLUTE,
		L"\\REGISTRY\\MACHINE\\SYSTEM\\ControlSet001\\services\\APDDRVX",
		L"test",
		L"testqweerty"
	);

	status = pRegUtilsInt->AddMultiStringToKey(
		RTL_REGISTRY_ABSOLUTE,
		L"\\REGISTRY\\MACHINE\\SYSTEM\\ControlSet001\\services\\APDDRVX",
		L"test",
		NULL,
		L"testqweerty"
	);

	status = pRegUtilsInt->AddMultiStringToKey(
		RTL_REGISTRY_ABSOLUTE,
		L"\\REGISTRY\\MACHINE\\SYSTEM\\ControlSet001\\services\\APDDRVX",
		L"test",
		NULL,
		L"testqweerty1"
	);

	status = pRegUtilsInt->AddMultiStringToKey(
		RTL_REGISTRY_ABSOLUTE,
		L"\\REGISTRY\\MACHINE\\SYSTEM\\ControlSet001\\services\\APDDRVX",
		L"test",
		NULL,
		L"1234"
	);

	status = pRegUtilsInt->RemoveMultiStringFromKey(
		RTL_REGISTRY_ABSOLUTE,
		L"\\REGISTRY\\MACHINE\\SYSTEM\\ControlSet001\\services\\APDDRVX",
		L"test",
		L"testqweerty1"
	);

	return 0;

}
