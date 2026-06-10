#pragma once

// If this function returns false the ParseMultiString will return false
typedef
BOOL (*MtStrCallBack)(PWCHAR Data, UINT DataLength, PVOID ConText);

namespace UserKernelUtilsLib
{
	namespace IMultiStringUtils
	{
		// If the MultiStringMaxLen param is greater than the memory allocated for the MultiString the behavior is undefined
		BOOL CheckMultiString(PWCHAR MultiString, UINT MultiStringMaxLen);

		// all the following functions assume that the MultiString is properly formed
		// all the following functions will return a proper Multi-String

		// to get the size of the string (not the size of the buffer) GetMultiStringSize can be used
		UINT GetMultiStringSize(PWCHAR MultiString);

		BOOL ParseMultiString(PWCHAR MultiString, MtStrCallBack CallBack, PVOID Context);

		// will return a copy of the original MultiString without the DelString
		// if the DelString does not exist will return FALSE
		// if the DelString is null will return a copy of the original MultiString
		BOOL CopyDeleteStringFromMultiString(PWCHAR MultiString, PWCHAR* MultiStringCopy, PWCHAR DelString);

		/* If After is NULL will add at the start
		   If After is -1 will add at the end
		   else will add after the "After" string
		   if the "After" string is not found, will return FALSE
		   if string exist will remove it before adding it
		*/
		BOOL CopyAddStringToMultiString(PWCHAR MultiString, PWCHAR* MultiStringCopy, PWCHAR AddString, PWCHAR After);
	}
}
