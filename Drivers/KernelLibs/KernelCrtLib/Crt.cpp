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

#include "Crt.h"

#pragma section(".CRT$XCA",long,read)
#pragma section(".CRT$XCZ",long,read)
#define _CRTALLOC(x) __declspec(allocate(x))


typedef void(__cdecl *PVOIDFUNC)();

typedef struct _ATEXIT_LIST_ENTRY {
	LIST_ENTRY link;
	PVOIDFUNC func;
} ATEXIT_LIST_ENTRY, *PATEXIT_LIST_ENTRY;


_CRTALLOC(".CRT$XCA") PVOIDFUNC __xc_a[] = { NULL };
_CRTALLOC(".CRT$XCZ") PVOIDFUNC __xc_z[] = { NULL };

KSPIN_LOCK G_Exitlock;      // spin lock to protect atexit list
LIST_ENTRY exitlist;      // anchor of atexit list


int __cdecl atexit(PVOIDFUNC func)
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};

	PROC_ENTRY;

	PATEXIT_LIST_ENTRY p = (PATEXIT_LIST_ENTRY)ExAllocatePoolWithTag(NonPagedPool, sizeof(ATEXIT_LIST_ENTRY), '01YH');
	if (!p)
	{
		return 0;
	}

	p->func = func;
	ExInterlockedInsertHeadList(&exitlist, &p->link, &G_Exitlock);

	return 1;
}

void CrtExit()
{
	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};

	PROC_ENTRY;

	PATEXIT_LIST_ENTRY p;

	p = (PATEXIT_LIST_ENTRY)ExInterlockedRemoveHeadList(&exitlist, &G_Exitlock);
	while (p)
	{
		(*p->func)();
		ExFreePool(p);
		p = (PATEXIT_LIST_ENTRY)ExInterlockedRemoveHeadList(&exitlist, &G_Exitlock);
	}
}

void CrtInit()
{

	auto ProcEntry = [](BOOL start)
	{
		if (start)
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, ">>> Start file %!FILE! line: %!LINE!");
		else
			ESL_DBG_OUT(DBG_TRACE_FUNCTIONS, "<<< Finish file %!FILE! line: %!LINE!");

	};

	PROC_ENTRY;

	InitializeListHead(&exitlist);
	KeInitializeSpinLock(&G_Exitlock);

	for (PVOIDFUNC* p = __xc_a; p < __xc_z; ++p) if (*p != NULL)(**p)();
}

// void * __cdecl operator new(size_t nSize)
// {
// 	void* retval;
// 	retval = ExAllocatePoolWithTag(NonPagedPool, nSize, 'ARFE'); // EFRA(per)
// 	if (retval)
// 	{
// 		RtlZeroMemory(retval, nSize);
// 	}
// 	return retval;
// }

void * __cdecl operator new(size_t nSize, ULONG Tag)
{
	void* retval;
	retval = ExAllocatePoolWithTag(NonPagedPool, nSize, Tag);
	if (retval)
	{
		RtlZeroMemory(retval, nSize);
	}
	return retval;
}

void __cdecl operator delete(void * p)
{
	if (p) ExFreePool(p);
}

