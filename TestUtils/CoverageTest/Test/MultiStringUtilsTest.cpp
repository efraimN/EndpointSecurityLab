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

#include <IMultiStringUtils.h>

using namespace UserKernelUtilsLib;

WCHAR MultiStrTest[] = { L"string22" L"\0" L"string4444" L"\0" L"string555555" L"\0" L"string666666" L"\0"};

static
BOOL PrintMtStringCallBack(PWCHAR Data, UINT DataLength, PVOID /*ConText*/)
{
	printf("Data is %S, DataLength is %d\n", Data, DataLength);
	return TRUE;
}

BOOL PrintMtString(PWCHAR MtString)
{
	UINT Tmp;
	printf("\n");
	if (!IMultiStringUtils::ParseMultiString(MtString, PrintMtStringCallBack, NULL))
	{
		printf("Failed DeleteStringFromMultiString\n");
		return FALSE;
	}

	Tmp = IMultiStringUtils::GetMultiStringSize(MtString);
	if (!Tmp)
	{
		printf("Failed GetMultiStringSize\n");
		return FALSE;
	}
	printf("GetMultiStringSize returned %d\n", Tmp);

	return TRUE;
}

/*!
Usage:
Tester TestMultiStringUtilsLib

Do not use

for debug purposes

*/
int TEST_FUNCTION TestMultiStringUtilsLib()
{
	int RetVal = -1;
	PWCHAR MultiStringCopy = 0;

	if (!IMultiStringUtils::CheckMultiString(
		MultiStrTest,
		ARRAYSIZE(MultiStrTest)
	))
	{
		printf("Failed CheckMultiString\n");
		goto Leave;
	}
	printf("Success on CheckMultiString\n");

	if (!PrintMtString(MultiStrTest))
	{
		printf("Failed ParseMultiString\n");
		goto Leave;
	}

	printf("\nTesting CopyDeleteStringFromMultiString removing string22 that is at the start\n");
	if (!IMultiStringUtils::CopyDeleteStringFromMultiString(MultiStrTest, &MultiStringCopy, L"string22"))
	{
		printf("Failed CopyDeleteStringFromMultiString\n");
		goto Leave;
	}
	printf("Success on CopyDeleteStringFromMultiString for string22\n");

	if (!PrintMtString(MultiStringCopy))
	{
		printf("Failed PrintMtString\n");
		goto Leave;
	}

	delete[] MultiStringCopy;
	MultiStringCopy = NULL;

	printf("\nTesting CopyDeleteStringFromMultiString removing string666666 that is at the end\n");
	if (!IMultiStringUtils::CopyDeleteStringFromMultiString(MultiStrTest, &MultiStringCopy, L"string666666"))
	{
		printf("Failed CopyDeleteStringFromMultiString\n");
		goto Leave;
	}
	printf("Success on CopyDeleteStringFromMultiString for string666666\n");

	if (!PrintMtString(MultiStringCopy))
	{
		printf("Failed PrintMtString\n");
		goto Leave;
	}

	delete[] MultiStringCopy;
	MultiStringCopy = NULL;

	printf("\nTesting CopyDeleteStringFromMultiString removing string4444\n");
	if (!IMultiStringUtils::CopyDeleteStringFromMultiString(MultiStrTest, &MultiStringCopy, L"string4444"))
	{
		printf("Failed CopyDeleteStringFromMultiString\n");
		goto Leave;
	}
	printf("Success on CopyDeleteStringFromMultiString for string4444\n");

	if (!PrintMtString(MultiStringCopy))
	{
		printf("Failed PrintMtString\n");
		goto Leave;
	}

	delete[] MultiStringCopy;
	MultiStringCopy = NULL;

	printf("\nTesting CopyDeleteStringFromMultiString removing abc12345tre that does not exist should return false\n");
	if (IMultiStringUtils::CopyDeleteStringFromMultiString(MultiStrTest, &MultiStringCopy, L"abc12345tre"))
	{
		printf("Failed CopyDeleteStringFromMultiString\n");
		goto Leave;
	}
	printf("Success on CopyDeleteStringFromMultiString for a nonexistent string\n");

	printf("\nTesting CopyDeleteStringFromMultiString a null string should return a copy\n");
	if (!IMultiStringUtils::CopyDeleteStringFromMultiString(MultiStrTest, &MultiStringCopy, NULL))
	{
		printf("Failed CopyDeleteStringFromMultiString\n");
		goto Leave;
	}
	printf("Success on CopyDeleteStringFromMultiString for NULL\n");

	if (!PrintMtString(MultiStringCopy))
	{
		printf("Failed PrintMtString\n");
		goto Leave;
	}

	delete[] MultiStringCopy;
	MultiStringCopy = NULL;

	printf("\nTesting CopyAddStringToMultiString adding stringAbc123xyz at the start\n");
	if (!IMultiStringUtils::CopyAddStringToMultiString(MultiStrTest, &MultiStringCopy, L"stringAbc123xyz", NULL))
	{
		printf("Failed CopyAddStringToMultiString\n");
		goto Leave;
	}
	printf("Success on CopyDeleteStringFromMultiString for adding stringAbc123xyz at the start\n");

	if (!PrintMtString(MultiStringCopy))
	{
		printf("Failed PrintMtString\n");
		goto Leave;
	}

	delete[] MultiStringCopy;
	MultiStringCopy = NULL;

	printf("\nTesting CopyAddStringToMultiString adding stringAbc123xyz at the end\n");
	if (!IMultiStringUtils::CopyAddStringToMultiString(MultiStrTest, &MultiStringCopy, L"stringAbc123xyz", (PWCHAR)-1))
	{
		printf("Failed CopyAddStringToMultiString\n");
		goto Leave;
	}
	printf("Success on CopyDeleteStringFromMultiString for adding stringAbc123xyz at the end\n");

	if (!PrintMtString(MultiStringCopy))
	{
		printf("Failed PrintMtString\n");
		goto Leave;
	}

	delete[] MultiStringCopy;
	MultiStringCopy = NULL;

	printf("\nTesting CopyAddStringToMultiString adding strintes1234 after string22\n");
	if (!IMultiStringUtils::CopyAddStringToMultiString(MultiStrTest, &MultiStringCopy, L"strintes1234", L"string22"))
	{
		printf("Failed CopyAddStringToMultiString\n");
		goto Leave;
	}
	printf("Success on CopyDeleteStringFromMultiString for adding strintes1234 after string22\n");

	if (!PrintMtString(MultiStringCopy))
	{
		printf("Failed PrintMtString\n");
		goto Leave;
	}

	delete[] MultiStringCopy;
	MultiStringCopy = NULL;

	printf("\nTesting CopyAddStringToMultiString removing string666666 from end and added after string22\n");
	if (!IMultiStringUtils::CopyAddStringToMultiString(MultiStrTest, &MultiStringCopy, L"string666666", L"string22"))
	{
		printf("Failed CopyAddStringToMultiString\n");
		goto Leave;
	}
	printf("Success on CopyAddStringToMultiString for removing string666666 from end and added after string22\n");

	if (!PrintMtString(MultiStringCopy))
	{
		printf("Failed PrintMtString\n");
		goto Leave;
	}

	delete[] MultiStringCopy;
	MultiStringCopy = NULL;

	printf("\nTesting CopyAddStringToMultiString removing string666666 from end and added at start\n");
	if (!IMultiStringUtils::CopyAddStringToMultiString(MultiStrTest, &MultiStringCopy, L"string666666", NULL))
	{
		printf("Failed CopyAddStringToMultiString\n");
		goto Leave;
	}
	printf("Success on CopyDeleteStringFromMultiString for removing string666666 from end and added at start\n");

	if (!PrintMtString(MultiStringCopy))
	{
		printf("Failed PrintMtString\n");
		goto Leave;
	}

	delete[] MultiStringCopy;
	MultiStringCopy = NULL;

	printf("\nTesting CopyAddStringToMultiString removing string666666 from end and added at the end\n");
	if (!IMultiStringUtils::CopyAddStringToMultiString(MultiStrTest, &MultiStringCopy, L"string666666", (PWCHAR)-1))
	{
		printf("Failed CopyAddStringToMultiString\n");
		goto Leave;
	}
	printf("Success on CopyDeleteStringFromMultiString for removing string666666 from end and added at the end\n");

	if (!PrintMtString(MultiStringCopy))
	{
		printf("Failed PrintMtString\n");
		goto Leave;
	}

	delete[] MultiStringCopy;
	MultiStringCopy = NULL;

	printf("\nTesting CopyAddStringToMultiString removing string22 and add it at the end\n");
	if (!IMultiStringUtils::CopyAddStringToMultiString(MultiStrTest, &MultiStringCopy, L"string22", (PWCHAR)-1))
	{
		printf("Failed CopyAddStringToMultiString\n");
		goto Leave;
	}
	printf("Success on CopyDeleteStringFromMultiString for removing string22 and add it at the end\n");

	if (!PrintMtString(MultiStringCopy))
	{
		printf("Failed PrintMtString\n");
		goto Leave;
	}

	delete[] MultiStringCopy;
	MultiStringCopy = NULL;

	printf("\nTesting CopyAddStringToMultiString removing string22 and add it after string555555\n");
	if (!IMultiStringUtils::CopyAddStringToMultiString(MultiStrTest, &MultiStringCopy, L"string22", L"string555555"))
	{
		printf("Failed CopyAddStringToMultiString\n");
		goto Leave;
	}
	printf("Success on CopyDeleteStringFromMultiString for removing string22 and add it after string555555\n");

	if (!PrintMtString(MultiStringCopy))
	{
		printf("Failed PrintMtString\n");
		goto Leave;
	}

	delete[] MultiStringCopy;
	MultiStringCopy = NULL;

	printf("\nTesting CopyAddStringToMultiString adding stringenddd will fail coz the after string does not exist\n");
	if (IMultiStringUtils::CopyAddStringToMultiString(MultiStrTest, &MultiStringCopy, L"stringenddd", L"dgcr"))
	{
		printf("Failed CopyAddStringToMultiString\n");
		goto Leave;
	}
	printf("Success on CopyDeleteStringFromMultiString for failing adding a string coz the after string does not exist\n");

	printf("\nTesting CopyAddStringToMultiString adding string555555 will fail coz the after string does not exist\n");
	if (IMultiStringUtils::CopyAddStringToMultiString(MultiStrTest, &MultiStringCopy, L"string555555", L"dgcr"))
	{
		printf("Failed CopyAddStringToMultiString\n");
		goto Leave;
	}
	printf("Success on CopyDeleteStringFromMultiString for failing adding a string coz the after string does not exist\n");

	printf("*********   TEST IS SUCCESS **********\n");

	RetVal = 0;

Leave:
	if (RetVal)
	{
		printf("*********   TEST FAIL **********\n");
	}
	if (MultiStringCopy)
	{
		delete[] MultiStringCopy;
	}

	return RetVal;
}
