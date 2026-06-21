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

#include <IBasicMiniFilterLib.h>

class MiniFilter
{
public:

	static MiniFilter* GetInstance();

	NTSTATUS Start();

	NTSTATUS Stop();

private:

	// delete copy and move constructors and assign operators
	MiniFilter(MiniFilter const&);             // Copy construct
	MiniFilter(MiniFilter&&);                  // Move construct
	MiniFilter& operator=(MiniFilter const&);  // Copy assign
	MiniFilter& operator=(MiniFilter&&);       // Move assign
	MiniFilter();
	~MiniFilter();

	static
	FLT_POSTOP_CALLBACK_STATUS
	FLTAPI
	FilterPostCreate(
		PFLT_CALLBACK_DATA       Data,
		PCFLT_RELATED_OBJECTS    FltObjects,
		PVOID                    CompletionContext,
		FLT_POST_OPERATION_FLAGS Flags
		);

	FLT_POSTOP_CALLBACK_STATUS
	FLTAPI
	_FilterPostCreate(
		PFLT_CALLBACK_DATA       Data,
		PCFLT_RELATED_OBJECTS    FltObjects,
		PVOID                    CompletionContext,
		FLT_POST_OPERATION_FLAGS Flags
	);


	static
	FLT_PREOP_CALLBACK_STATUS
	FLTAPI 
	FilterPreCreate(
		PFLT_CALLBACK_DATA Data,
		PCFLT_RELATED_OBJECTS FltObjects,
		PVOID*
	);

	BOOL ShouldStopRead(
		PCFLT_RELATED_OBJECTS    FltObjects,
		PUNICODE_STRING FileName,
		PUNICODE_STRING OriginalUserSid
	);

	static
	FLT_OPERATION_REGISTRATION CallBacks[];

	IBasicMiniFilter* m_BasicMiniFilter;
};
