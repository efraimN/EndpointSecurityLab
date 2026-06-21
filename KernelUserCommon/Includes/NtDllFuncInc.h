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

#ifndef _NTDDK_

NTSTATUS
NTAPI
NTZW(Close)(
	_In_ HANDLE Handle
	);


NTSTATUS
NTAPI
NTZW(EnumerateKey)(
	_In_ HANDLE KeyHandle,
	_In_ ULONG Index,
	_In_ KEY_INFORMATION_CLASS KeyInformationClass,
	_Out_writes_bytes_opt_(Length) PVOID KeyInformation,
	_In_ ULONG Length,
	_Out_ PULONG ResultLength
	);

NTSTATUS
NTAPI
NTZW(OpenKey)(
	_Out_ PHANDLE KeyHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes
	);

NTSTATUS
NTAPI
NTZW(DeleteKey)(
	_In_ HANDLE KeyHandle
	);

NTSTATUS
NTAPI
NTZW(CreateKey)(
	_Out_ PHANDLE KeyHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_Reserved_ ULONG TitleIndex,
	_In_opt_ PUNICODE_STRING Class,
	_In_ ULONG CreateOptions,
	_Out_opt_ PULONG Disposition
	);

NTSTATUS
NTAPI
NTZW(DeleteValueKey)(
	_In_ HANDLE KeyHandle,
	_In_ PUNICODE_STRING ValueName
	);

NTSTATUS
NTAPI
NTZW(CreateSection)(
	_Out_ PHANDLE SectionHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_opt_ PLARGE_INTEGER MaximumSize,
	_In_ ULONG SectionPageProtection,
	_In_ ULONG AllocationAttributes,
	_In_opt_ HANDLE FileHandle
	);

NTSTATUS
NTAPI
NTZW(UnmapViewOfSection)(
	_In_ HANDLE ProcessHandle,
	_In_opt_ PVOID BaseAddress
	);

NTSTATUS
NTAPI
NTZW(MapViewOfSection)(
	_In_ HANDLE SectionHandle,
	_In_ HANDLE ProcessHandle,
	_Outptr_result_bytebuffer_(*ViewSize) PVOID* BaseAddress,
	_In_ ULONG_PTR ZeroBits,
	_In_ SIZE_T CommitSize,
	_Inout_opt_ PLARGE_INTEGER SectionOffset,
	_Inout_ PSIZE_T ViewSize,
	_In_ SECTION_INHERIT InheritDisposition,
	_In_ ULONG AllocationType,
	_In_ ULONG Win32Protect
	);

NTSTATUS
NTAPI
NTZW(OpenFile)(
	_Out_ PHANDLE FileHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ ULONG ShareAccess,
	_In_ ULONG OpenOptions
	);

NTSTATUS
NTAPI
NTZW(CreateFile)(
	_Out_ PHANDLE FileHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_opt_ PLARGE_INTEGER AllocationSize,
	_In_ ULONG FileAttributes,
	_In_ ULONG ShareAccess,
	_In_ ULONG CreateDisposition,
	_In_ ULONG CreateOptions,
	_In_reads_bytes_opt_(EaLength) PVOID EaBuffer,
	_In_ ULONG EaLength
	);

NTSTATUS
NTAPI
NTZW(ReadFile)(
	_In_ HANDLE FileHandle,
	_In_opt_ HANDLE Event,
	_In_opt_ PIO_APC_ROUTINE ApcRoutine,
	_In_opt_ PVOID ApcContext,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_Out_writes_bytes_(Length) PVOID Buffer,
	_In_ ULONG Length,
	_In_opt_ PLARGE_INTEGER ByteOffset,
	_In_opt_ PULONG Key
	);

NTSTATUS
NTAPI
NTZW(QueryInformationFile)(
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_Out_writes_bytes_(Length) PVOID FileInformation,
	_In_ ULONG Length,
	_In_ NTFILE_INFORMATION_CLASS FileInformationClass
	);

NTSTATUS
NTAPI
NTZW(DuplicateObject)(
	_In_ HANDLE SourceProcessHandle,
	_In_ HANDLE SourceHandle,
	_In_opt_ HANDLE TargetProcessHandle,
	_Out_opt_ PHANDLE TargetHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ ULONG HandleAttributes,
	_In_ ULONG Options
	);

NTSTATUS
NTAPI
NTZW(OpenProcess)(
	_Out_ PHANDLE ProcessHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_opt_ PCLIENT_ID ClientId
	);

NTSTATUS
NTAPI
NTZW(SetInformationFile)(
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_Out_writes_bytes_(Length) PVOID FileInformation,
	_In_ ULONG Length,
	_In_ NTFILE_INFORMATION_CLASS FileInformationClass
	);


NTSTATUS
NTAPI
NTZW(CreateEvent)(
	_Out_ PHANDLE EventHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_ EVENT_TYPE EventType,
	_In_ BOOLEAN InitialState
	);

NTSTATUS
NTAPI
NTZW(SetEvent)(
	_In_ HANDLE EventHandle,
	_Out_opt_ PLONG PreviousState
	);

NTSTATUS
NTAPI
NTZW(ClearEvent)(
	_In_ HANDLE EventHandle
	);

#endif // NOT _NTDDK_

// START LPC

NTSTATUS
NTAPI
NTZW(ReplyWaitReceivePortEx)(
	IN HANDLE PortHandle,
	OUT PVOID* PortContext OPTIONAL,
	IN PPORT_MESSAGE ReplyMessage OPTIONAL,
	OUT PPORT_MESSAGE ReceiveMessage,
	IN PLARGE_INTEGER Timeout OPTIONAL
	);

NTSTATUS
NTAPI
NTZW(ReplyPort)(
	IN HANDLE PortHandle,
	IN PPORT_MESSAGE LpcReply
	);

NTSTATUS
NTAPI
NTZW(ReplyWaitReplyPort)(
	IN HANDLE PortHandle,
	IN OUT PPORT_MESSAGE LpcReply
	);

NTSTATUS
NTAPI
NTZW(CreateWaitablePort)(
	PHANDLE PortHandle,
	POBJECT_ATTRIBUTES ObjectAttributes,
	ULONG MaxConnectInfoLength,
	ULONG MaxDataLength,
	ULONG NPMessageQueueSize OPTIONAL
	);

NTSTATUS
NTAPI
NTZW(CompleteConnectPort)(
	HANDLE PortHandle
	);

NTSTATUS
NTAPI
NTZW(AcceptConnectPort)(
	PHANDLE PortHandle,
	PVOID PortContext OPTIONAL,
	PPORT_MESSAGE ConnectionRequest,
	BOOLEAN AcceptConnection,
	PPORT_VIEW ServerView OPTIONAL,
	PREMOTE_PORT_VIEW ClientView OPTIONAL
	);

NTSTATUS
NTAPI
NTZW(RequestWaitReplyPort)(
	IN HANDLE PortHandle,
	IN PPORT_MESSAGE LpcRequest,
	OUT PPORT_MESSAGE LpcReply
	);

NTSTATUS
NTAPI
NTZW(RequestPort)(
	IN HANDLE PortHandle,
	IN PPORT_MESSAGE LpcRequest
	);

NTSTATUS
NTAPI
NTZW(ConnectPort)(
	PHANDLE PortHandle,
	PUNICODE_STRING PortName,
	PSECURITY_QUALITY_OF_SERVICE SecurityQos,
	PPORT_VIEW ClientView OPTIONAL,
	PREMOTE_PORT_VIEW ServerView OPTIONAL,
	PULONG MaxMessageLength OPTIONAL,
	PVOID ConnectionInformation OPTIONAL,
	PULONG ConnectionInformationLength OPTIONAL
	);

NTSTATUS
NTAPI
NTZW(ListenPort)(
	IN HANDLE PortHandle,
	OUT PPORT_MESSAGE Message
	);
// END LPC
