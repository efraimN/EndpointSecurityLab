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
	class IRegUtils
	{
	public:
		static IRegUtils* GetInstance();

		virtual NTSTATUS RegDeleteAllKeys(HANDLE ParentKey, DWORD DesiredAccess = 0) = 0;

		virtual
		NTSTATUS
		RegSetValue(
			IN     ULONG           RelativeTo,
			IN     PWSTR           KeyNamePath,
			IN     PWSTR           ParameterName,
			IN OUT PVOID           ParameterValue,
			IN OUT ULONG           ParameterLength,
			IN     ULONG		   ValueType
		) = 0;

		virtual
		NTSTATUS
		RegGetValue(
			IN     ULONG           RelativeTo,
			IN     PWSTR           KeyNamePath,
			IN     PWSTR           ParameterName,
			IN OUT PVOID           ParameterValue,
			IN OUT PDWORD          ParameterLength
		) = 0;

		virtual
		BOOL
		RegDeletekey(PWCHAR Path) = 0;

		virtual
		HANDLE
		RegOpenCreateKey(
			_In_  ULONG RelativeTo,
			_In_  PWSTR Path,
			_In_  PWSTR SubPath,
			BOOLEAN Open_Create,
			ACCESS_MASK DesiredAccess,
			ULONG CreateOptions
		) = 0;

		virtual
		HANDLE
		RegOpenCreateKeyStatus(
			_In_  ULONG RelativeTo,
			_In_  PWSTR Path,
			_In_  PWSTR SubPath,
			BOOLEAN Open_Create,
			ACCESS_MASK DesiredAccess,
			ULONG CreateOptions,
			NTSTATUS *Status
		) = 0;

		/* 
		   If the key does not exist will create it, and add the AddString as a multi-string
		   If After is NULL will add at the start
		   If After is -1 will add at the end
		   else will add after the "After" string
		   if the "After" string is not found, will return error
		   if AddString string exist will remove it before adding it
		*/
		virtual
		NTSTATUS
		AddMultiStringToKey(
			_In_ ULONG RelativeTo,
			_In_ PWSTR KeyNamePath,
			_In_ PWSTR ValueName,
			_In_ PWSTR After,
			_In_ PWSTR AddString
		) = 0;

		// if the DelString does not exist will return error
		virtual
		NTSTATUS
		RemoveMultiStringFromKey(
			_In_ ULONG RelativeTo,
			_In_ PWSTR KeyNamePath,
			_In_ PWSTR ValueName,
			_In_ PWSTR DelString
		) = 0;

		virtual
		NTSTATUS
		RemoveMultiStringFromKeyDeleteIfEmpty(
			_In_ ULONG RelativeTo,
			_In_ PWSTR KeyNamePath,
			_In_ PWSTR ValueName,
			_In_ PWSTR DelString
		) = 0;

		typedef
		BOOL
		(*PREG_ENUM_KEYS_CALLBACK)(
			_In_ HANDLE KeyHandle,
			_In_opt_ PVOID Context
			);

	protected:
		IRegUtils()NOEXCEPT {};
		virtual ~IRegUtils() {};

	private:
		IRegUtils(const IRegUtils& other);
		IRegUtils& operator=(const IRegUtils& other);
	};
}
