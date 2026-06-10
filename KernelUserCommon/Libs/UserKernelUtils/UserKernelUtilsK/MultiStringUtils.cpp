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

#include "MultiStringUtils.h"

using namespace UserKernelUtilsLib;

static
BOOL ParseMultiStringInternal(PWCHAR MultiString, UINT MultiStringMaxLen, MtStrCallBack CallBack, PVOID Context)
{
	BOOL RetVal = FALSE;
	UINT TmpLength = 0;
	PWCHAR Tmp;
	UINT TotalLength = 0;

	Tmp = MultiString;
	for (;;)
	{
		TmpLength = (UINT)wcslen(Tmp)+1;
		TotalLength += TmpLength;
		if (MultiStringMaxLen)
		{
			if (TotalLength >= MultiStringMaxLen)
			{
				goto Leave;
			}
		}
		if (CallBack)
		{
			if (!CallBack(Tmp, TmpLength, Context))
			{
				goto Leave;;
			}
		}
		Tmp = &Tmp[TmpLength];
		if (*Tmp == 0)
		{
			break;
		}
	}
	RetVal = TRUE;
Leave:
	return RetVal;
}

static
BOOL GetMultiStringSizeCallBack(PWCHAR /*Data*/, UINT DataLength, PVOID ConText)
{
	PUINT Count = (PUINT)ConText;
	*Count += DataLength;
	return TRUE;
}

BOOL IMultiStringUtils::CheckMultiString(PWCHAR MultiString, UINT MultiStringMaxLen)
{
	UINT Count = 1;
	if (!ParseMultiStringInternal(MultiString, MultiStringMaxLen, GetMultiStringSizeCallBack, &Count))
	{
		return FALSE;
	}

	return (Count <= MultiStringMaxLen);
}

UINT IMultiStringUtils::GetMultiStringSize(PWCHAR MultiString)
{
	UINT RetVal = 0;
	UINT Count = 1;
	if (!ParseMultiString(MultiString, GetMultiStringSizeCallBack, &Count))
	{
		goto Leave;
	}

	RetVal = Count;
Leave:
	return RetVal;
}

BOOL IMultiStringUtils::ParseMultiString(PWCHAR MultiString, MtStrCallBack CallBack, PVOID Context)
{
	return ParseMultiStringInternal(MultiString, 0, CallBack, Context);
}

typedef struct _DeleteStringFromMultiStringCallBackData
{
	PWCHAR DelString;
	PWCHAR TmpTargetMtString;
	UINT TmpTargetMtStringLength;
	BOOL Found;

}DeleteStringFromMultiStringCallBackData, *PDeleteStringFromMultiStringCallBackData;
 
static
BOOL DeleteStringFromMultiStringCallBack(PWCHAR Data, UINT DataLength, PVOID ConText)
{
	BOOL RetVal = FALSE;
	PDeleteStringFromMultiStringCallBackData CallBackData = 
		(PDeleteStringFromMultiStringCallBackData)ConText;

	if (CallBackData->DelString)
	{
		if (_wcsicmp(Data, CallBackData->DelString) == 0)
		{
			RetVal = TRUE;
			CallBackData->Found = TRUE;
			goto Leave;
		}
	}

	RtlCopyMemory(CallBackData->TmpTargetMtString, Data, DataLength * 2);
	CallBackData->TmpTargetMtString = &CallBackData->TmpTargetMtString[DataLength];

	RetVal = TRUE;
Leave:
	CallBackData->TmpTargetMtString[0] = 0;
	return RetVal;
}

BOOL IMultiStringUtils::CopyDeleteStringFromMultiString(PWCHAR MultiString, PWCHAR* MultiStringCopy, PWCHAR DelString)
{
	BOOL RetVal = FALSE;
	UINT TmpSize;
	DeleteStringFromMultiStringCallBackData Data;
	PWCHAR TargetMtString;

	*MultiStringCopy = NULL;
	TmpSize = GetMultiStringSize(MultiString);
	Data.DelString = DelString;
	TargetMtString =
#ifdef _NTDDK_
		new ('tStM')
#else
		new
#endif
		WCHAR[TmpSize];

	if (!TargetMtString)
	{
		goto Leave;
	}
	RtlZeroMemory(TargetMtString, TmpSize*2);
	Data.TmpTargetMtString = TargetMtString;
	Data.Found = FALSE;
	Data.TmpTargetMtStringLength = TmpSize;
	if (!ParseMultiString(MultiString, DeleteStringFromMultiStringCallBack, &Data))
	{
		goto Leave;
	}

	if (DelString)
	{
		if (!Data.Found)
		{
			goto Leave;
		}
	}

	if (!CheckMultiString(
		TargetMtString,
		TmpSize
	))
	{
		goto Leave;
	}

	RetVal = TRUE;
	*MultiStringCopy = TargetMtString;

Leave:
	if (!RetVal)
	{
		if (TargetMtString)
		{
			delete[] TargetMtString;
		}
	}
	return RetVal;
}

typedef struct  _AddStringToMultiStringCallBackData
{
	PWCHAR AddString;
	PWCHAR After;
	PWCHAR TmpTargetMtString;
	BOOL Added;
}AddStringToMultiStringCallBackData, *PAddStringToMultiStringCallBackData;

static
BOOL AddStringToMultiStringCallBack(PWCHAR Data, UINT DataLength, PVOID ConText)
{
	PAddStringToMultiStringCallBackData CallBackData = (PAddStringToMultiStringCallBackData)ConText;
	UINT Tmp;
	RtlCopyMemory(CallBackData->TmpTargetMtString, Data, DataLength * 2);
	CallBackData->TmpTargetMtString = &CallBackData->TmpTargetMtString[DataLength];

	if (CallBackData->After && !CallBackData->Added)
	{
		if (_wcsicmp(Data, CallBackData->After) == 0)
		{
			Tmp = (UINT)wcslen(CallBackData->AddString) + 1;
			RtlCopyMemory(CallBackData->TmpTargetMtString, 
				CallBackData->AddString, 
				Tmp * 2);

			CallBackData->TmpTargetMtString = &CallBackData->TmpTargetMtString[Tmp];
			CallBackData->Added = TRUE;
		}
	}

	return TRUE;
}

/* If After is NULL will add at the start
   If After is -1 will add at the end
   else will add after the "After" string
   if the "After" string is not found, will return FALSE
   if string exist will remove it before adding it
*/
BOOL IMultiStringUtils::CopyAddStringToMultiString(PWCHAR MultiString, PWCHAR* MultiStringCopy, PWCHAR AddString, PWCHAR After)
{
	BOOL RetVal = FALSE;;
	PWCHAR MultiStringCopyTmp = NULL;
	UINT TmpSize;
	UINT StartCopyIndex;
	PWCHAR TargetMtString = NULL;
	AddStringToMultiStringCallBackData CallBackData;

	*MultiStringCopy = NULL;

	// First get a copy, but be sure the string to add is not there
	if (!CopyDeleteStringFromMultiString(
		MultiString,
		&MultiStringCopyTmp,
		AddString
	))
	{
		if (!CopyDeleteStringFromMultiString(
			MultiString,
			&MultiStringCopyTmp,
			NULL
		))
		{
			goto Leave;
		}
	}

	TmpSize = GetMultiStringSize(MultiStringCopyTmp);
	TmpSize += (UINT)wcslen(AddString)+1;

	TargetMtString =
#ifdef _NTDDK_
		new ('tStM')
#else
		new
#endif
		WCHAR[TmpSize];

	if (!TargetMtString)
	{
		goto Leave;
	}

	RtlZeroMemory(TargetMtString, TmpSize * 2);

	StartCopyIndex = 0;
	CallBackData.After = After;
	CallBackData.Added = FALSE;
	if (After == 0)
	{
		// ad at the start
		StartCopyIndex = (UINT)wcslen(AddString) + 1;
		RtlCopyMemory(TargetMtString, AddString, StartCopyIndex * 2);
		CallBackData.Added = TRUE;
	}
	if (After == (PWCHAR)-1)
	{
		// this way the callback wont use the after param
		CallBackData.After = 0;
		CallBackData.Added = TRUE;
	}

	CallBackData.AddString = AddString;
	CallBackData.TmpTargetMtString = &TargetMtString[StartCopyIndex];
	// Now parse the obtained MultiStringCopyTmp into the new TargetMtString adding the AddString
	if (!ParseMultiString(
		MultiStringCopyTmp,
		AddStringToMultiStringCallBack,
		&CallBackData
	))
	{
		goto Leave;
	}

	if (CallBackData.AddString)
	{
		if (!CallBackData.Added)
		{
			goto Leave;
		}
	}

	if (!CheckMultiString(
		TargetMtString,
		TmpSize
	))
	{
		goto Leave;
	}

	if (After == (PWCHAR)-1)
	{
		UINT TmpLength;
		TmpLength = GetMultiStringSize(TargetMtString);
		RtlCopyMemory(&TargetMtString[TmpLength-1], AddString, (wcslen(AddString)+1) * 2);
		if (!CheckMultiString(
			TargetMtString,
			TmpSize
		))
		{
			goto Leave;
		}

	}

	*MultiStringCopy = TargetMtString;
	RetVal = TRUE;
Leave:
	if (MultiStringCopyTmp)
	{
		delete[] MultiStringCopyTmp;
	}

	if (!RetVal)
	{
		if (TargetMtString)
		{
			delete[] TargetMtString;
		}
	}
	return RetVal;
}

