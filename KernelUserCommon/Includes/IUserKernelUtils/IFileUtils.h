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

namespace UserKernelUtilsLib
{
	class IFileUtils
	{
	public:
		static IFileUtils* GetInstance();

		/* In kernel The handle returned is a kernel Handle */
		virtual
		BOOLEAN OpenFile(
			PUNICODE_STRING FileName,
			PHANDLE hFile,
			ACCESS_MASK DesiredAccess,
			ULONG ShareAccess
		) = 0;

		/* In kernel The handle returned is a kernel Handle */
		virtual
		BOOLEAN CreateFile(
			PUNICODE_STRING FileName,
			PHANDLE hFile,
			ACCESS_MASK DesiredAccess,
			ULONG ShareAccess,
			LARGE_INTEGER AllocationSize
		) = 0;

		virtual
		BOOLEAN GetFileSize(
			HANDLE hFile,
			PLARGE_INTEGER FileSize
		) = 0;

		virtual
		BOOLEAN SetFileSize(
			HANDLE hFile,
			LARGE_INTEGER FileSize
		) = 0;

	protected:
		IFileUtils() NOEXCEPT {};
		virtual ~IFileUtils() {};

	private:
		IFileUtils(const IFileUtils& other);
		IFileUtils& operator=(const IFileUtils& other);
	};
}
