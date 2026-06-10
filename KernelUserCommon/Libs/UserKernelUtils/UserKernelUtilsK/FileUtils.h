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

#include <IFileUtils.h>

class CFileUtils : public UserKernelUtilsLib::IFileUtils
{
public:
	friend IFileUtils;

	virtual
	BOOLEAN OpenFile(
		PUNICODE_STRING FileName,
		PHANDLE hFile,
		ACCESS_MASK DesiredAccess,
		ULONG ShareAccess
	);

	virtual
	BOOLEAN CreateFile(
		PUNICODE_STRING FileName,
		PHANDLE hFile,
		ACCESS_MASK DesiredAccess,
		ULONG ShareAccess,
		LARGE_INTEGER AllocationSize
	);

	virtual
	BOOLEAN GetFileSize(
		HANDLE hFile,
		PLARGE_INTEGER FileSize
	);

	virtual
	BOOLEAN SetFileSize(
		HANDLE hFile,
		LARGE_INTEGER FileSize
	);

private:
	CFileUtils();
	virtual ~CFileUtils();

	BOOLEAN OpenCreateFileCommon(
		PUNICODE_STRING FileName,
		PHANDLE hFile,
		ACCESS_MASK DesiredAccess,
		ULONG ShareAccess,
		PLARGE_INTEGER AllocationSize
	);

};

