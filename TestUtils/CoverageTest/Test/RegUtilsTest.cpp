#include "Precompiled.h"
#include <WppIncludes.h>

#include <IRegUtils.h>

using namespace UserKernelUtilsLib;

/*!
Usage:
Tester TestAddMultiStringToKey

Do not use

for debug purposes

*/
int TEST_FUNCTION TestAddMultiStringToKey()
{
	NTSTATUS status;
	IRegUtilsInt* pRegUtilsInt;
	pRegUtilsInt = IRegUtilsInt::GetInstance();

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
