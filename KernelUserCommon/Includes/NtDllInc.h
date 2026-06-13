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

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct _CLIENT_ID_64
	{
		DWORD64 UniqueProcess;
		DWORD64 UniqueThread;
	}CLIENT_ID_64, * PCLIENT_ID_64;

	typedef struct _CLIENT_ID_32
	{
		DWORD UniqueProcess;
		DWORD UniqueThread;
	}CLIENT_ID_32, * PCLIENT_ID_32;

#ifndef _NTDDK_

#define NtCurrentProcess() ( (HANDLE)(LONG_PTR) -1 )  
#define ZwCurrentProcess() NtCurrentProcess()         

typedef struct _CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID, * PCLIENT_ID;

typedef struct _KEY_BASIC_INFORMATION {
	LARGE_INTEGER LastWriteTime;
	ULONG   TitleIndex;
	ULONG   NameLength;
	WCHAR   Name[1];            // Variable length string
} KEY_BASIC_INFORMATION, * PKEY_BASIC_INFORMATION;

typedef enum _KEY_INFORMATION_CLASS {
	KeyBasicInformation,
	KeyNodeInformation,
	KeyFullInformation,
	KeyNameInformation,
	KeyCachedInformation,
	KeyFlagsInformation,
	KeyVirtualizationInformation,
	KeyHandleTagsInformation,
	KeyTrustInformation,
	KeyLayerInformation,
	MaxKeyInfoClass  // MaxKeyInfoClass should always be the last enum
} KEY_INFORMATION_CLASS;

typedef enum _NTFILE_INFORMATION_CLASS {
	_FileDirectoryInformation = 1,
	FileFullDirectoryInformation,   // 2
	FileBothDirectoryInformation,   // 3
	FileBasicInformation,           // 4
	FileStandardInformation,        // 5
	FileInternalInformation,        // 6
	FileEaInformation,              // 7
	FileAccessInformation,          // 8
	FileNameInformation,            // 9
	FileRenameInformation,          // 10
	FileLinkInformation,            // 11
	FileNamesInformation,           // 12
	FileDispositionInformation,     // 13
	FilePositionInformation,        // 14
	FileFullEaInformation,          // 15
	FileModeInformation,            // 16
	FileAlignmentInformation,       // 17
	FileAllInformation,             // 18
	FileAllocationInformation,      // 19
	FileEndOfFileInformation,       // 20
	FileAlternateNameInformation,   // 21
	FileStreamInformation,          // 22
	FilePipeInformation,            // 23
	FilePipeLocalInformation,       // 24
	FilePipeRemoteInformation,      // 25
	FileMailslotQueryInformation,   // 26
	FileMailslotSetInformation,     // 27
	FileCompressionInformation,     // 28
	FileObjectIdInformation,        // 29
	FileCompletionInformation,      // 30
	FileMoveClusterInformation,     // 31
	FileQuotaInformation,           // 32
	FileReparsePointInformation,    // 33
	FileNetworkOpenInformation,     // 34
	FileAttributeTagInformation,    // 35
	FileTrackingInformation,        // 36
	FileIdBothDirectoryInformation, // 37
	FileIdFullDirectoryInformation, // 38
	FileValidDataLengthInformation, // 39
	FileShortNameInformation,       // 40
	FileIoCompletionNotificationInformation, // 41
	FileIoStatusBlockRangeInformation,       // 42
	FileIoPriorityHintInformation,           // 43
	FileSfioReserveInformation,              // 44
	FileSfioVolumeInformation,               // 45
	FileHardLinkInformation,                 // 46
	FileProcessIdsUsingFileInformation,      // 47
	FileNormalizedNameInformation,           // 48
	FileNetworkPhysicalNameInformation,      // 49
	FileIdGlobalTxDirectoryInformation,      // 50
	FileIsRemoteDeviceInformation,           // 51
	FileUnusedInformation,                   // 52
	FileNumaNodeInformation,                 // 53
	FileStandardLinkInformation,             // 54
	FileRemoteProtocolInformation,           // 55

	//
	//  These are special versions of these operations (defined earlier)
	//  which can be used by kernel mode drivers only to bypass security
	//  access checks for Rename and HardLink operations.  These operations
	//  are only recognized by the IOManager, a file system should never
	//  receive these.
	//
	FileRenameInformationBypassAccessCheck,  // 56
	FileLinkInformationBypassAccessCheck,    // 57
	FileVolumeNameInformation,               // 58
	FileIdInformation,                       // 59
	FileIdExtdDirectoryInformation,          // 60
	FileReplaceCompletionInformation,        // 61
	FileHardLinkFullIdInformation,           // 62
	FileMaximumInformation
} NTFILE_INFORMATION_CLASS, * PNTFILE_INFORMATION_CLASS;

typedef enum _SECTION_INHERIT {
	ViewShare = 1,
	ViewUnmap = 2
} SECTION_INHERIT;

typedef struct _FILE_STANDARD_INFORMATION {
	LARGE_INTEGER AllocationSize;
	LARGE_INTEGER EndOfFile;
	ULONG NumberOfLinks;
	BOOLEAN DeletePending;
	BOOLEAN Directory;
} FILE_STANDARD_INFORMATION, * PFILE_STANDARD_INFORMATION;

typedef struct _FILE_POSITION_INFORMATION {
	LARGE_INTEGER CurrentByteOffset;
} FILE_POSITION_INFORMATION, * PFILE_POSITION_INFORMATION;

typedef struct _FILE_END_OF_FILE_INFORMATION {
	LARGE_INTEGER EndOfFile;
} FILE_END_OF_FILE_INFORMATION, * PFILE_END_OF_FILE_INFORMATION;


#endif // NOT _NTDK_

#ifdef _NTDDK_
typedef void* LPVOID;
typedef BYTE* PBYTE;
typedef DWORD* PDWORD;
#define SECTION_QUERY                0x0001
#define SECTION_MAP_WRITE            0x0002
#define SECTION_MAP_READ             0x0004
#define SECTION_MAP_EXECUTE          0x0008
#define SECTION_EXTEND_SIZE          0x0010
#define SECTION_MAP_EXECUTE_EXPLICIT 0x0020 // not included in SECTION_ALL_ACCESS

#define SECTION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SECTION_QUERY|\
								SECTION_MAP_WRITE |      \
								SECTION_MAP_READ |       \
								SECTION_MAP_EXECUTE |    \
								SECTION_EXTEND_SIZE)

#define SESSION_QUERY_ACCESS  0x0001
#define SESSION_MODIFY_ACCESS 0x0002

#define SESSION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED |  \
								SESSION_QUERY_ACCESS |             \
								SESSION_MODIFY_ACCESS)

#define PAGE_NOACCESS          0x01     
#define PAGE_READONLY          0x02     
#define PAGE_READWRITE         0x04     
#define PAGE_WRITECOPY         0x08     
#pragma region Desktop Family           
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) 
#define PAGE_EXECUTE           0x10     
#define PAGE_EXECUTE_READ      0x20     
#define PAGE_EXECUTE_READWRITE 0x40     
#define PAGE_EXECUTE_WRITECOPY 0x80     
#endif /* WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) */ 
#pragma endregion                       
#define PAGE_GUARD            0x100     
#define PAGE_NOCACHE          0x200     
#define PAGE_WRITECOMBINE     0x400     
#define PAGE_REVERT_TO_FILE_MAP     0x80000000     
#define MEM_COMMIT                  0x1000      
#define MEM_RESERVE                 0x2000      
#define MEM_DECOMMIT                0x4000      
#define MEM_RELEASE                 0x8000      
#define MEM_FREE                    0x10000     
#define MEM_PRIVATE                 0x20000     
#define MEM_MAPPED                  0x40000     
#define MEM_RESET                   0x80000     
#define MEM_TOP_DOWN                0x100000    
#define MEM_WRITE_WATCH             0x200000    
#define MEM_PHYSICAL                0x400000    
#define MEM_ROTATE                  0x800000    
#define MEM_DIFFERENT_IMAGE_BASE_OK 0x800000    
#define MEM_RESET_UNDO              0x1000000   
#define MEM_LARGE_PAGES             0x20000000  
#define MEM_4MB_PAGES               0x80000000  
#define SEC_FILE           0x800000     
#define SEC_IMAGE         0x1000000     
#define SEC_PROTECTED_IMAGE  0x2000000  
#define SEC_RESERVE       0x4000000     
#define SEC_COMMIT        0x8000000     
#define SEC_NOCACHE      0x10000000     
#define SEC_WRITECOMBINE 0x40000000     
#define SEC_LARGE_PAGES  0x80000000     
#define SEC_IMAGE_NO_EXECUTE (SEC_IMAGE | SEC_NOCACHE)     
#define MEM_IMAGE         SEC_IMAGE     
#define WRITE_WATCH_FLAG_RESET  0x01     
#define MEM_UNMAP_WITH_TRANSIENT_BOOST  0x01     

#define FILE_MAP_WRITE            SECTION_MAP_WRITE
#define FILE_MAP_READ             SECTION_MAP_READ
#define FILE_MAP_COPY       0x00000001
#define FILE_MAP_EXECUTE    SECTION_MAP_EXECUTE_EXPLICIT    // not included in FILE_MAP_ALL_ACCESS

#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)



#endif


//LPC
	//
	// LPC Message Types
	//

#define MAX_LPC_DATA 0x130    // Maximum number of bytes that can be copied through LPC

typedef enum _LPC_TYPE
{
	LPC_NEW_MESSAGE,
	LPC_REQUEST,
	LPC_REPLY,
	LPC_DATAGRAM,
	LPC_LOST_REPLY,
	LPC_PORT_CLOSED,
	LPC_CLIENT_DIED,
	LPC_EXCEPTION,
	LPC_DEBUG_EVENT,
	LPC_ERROR_EVENT,
	LPC_CONNECTION_REQUEST,
	LPC_CONNECTION_REFUSED,
	LPC_MAXIMUM
} LPC_TYPE;

typedef struct _PORT_VIEW64
{
	ULONG Length;
	ULONGLONG SectionHandle;
	ULONG SectionOffset;
	ULONGLONG ViewSize;
	ULONGLONG ViewBase;
	ULONGLONG ViewRemoteBase;
} PORT_VIEW64, * PPORT_VIEW64;

typedef struct _PORT_VIEW32
{
	ULONG Length;
	HANDLE SectionHandle;
	ULONG SectionOffset;
	SIZE_T ViewSize;
	PVOID ViewBase;
	PVOID ViewRemoteBase;
} PORT_VIEW32, * PPORT_VIEW32;

typedef struct _PORT_VIEW* PPORT_VIEW;

typedef struct _REMOTE_PORT_VIEW64
{
	ULONG Length;
	ULONGLONG ViewSize;
	ULONGLONG ViewBase;
} REMOTE_PORT_VIEW64, * PREMOTE_PORT_VIEW64;

typedef struct _REMOTE_PORT_VIEW32
{
	ULONG Length;
	SIZE_T ViewSize;
	PVOID ViewBase;
} REMOTE_PORT_VIEW32, * PREMOTE_PORT_VIEW32;

typedef struct _REMOTE_PORT_VIEW* PREMOTE_PORT_VIEW;

typedef struct _PORT_MESSAGE64
{
	USHORT DataSize;
	USHORT MessageSize;
	USHORT MessageType;
	SHORT VirtualRangesOffset;
	CLIENT_ID_64 ClientId;
	ULONG MessageId;
	ULONGLONG SectionSize;
} PORT_MESSAGE64, * PPORT_MESSAGE64;

typedef struct _PORT_MESSAGE32
{
	USHORT DataSize;
	USHORT MessageSize;
	USHORT MessageType;
	USHORT VirtualRangesOffset;
	CLIENT_ID ClientId;
	ULONG MessageId;
	ULONG SectionSize;
} PORT_MESSAGE32, * PPORT_MESSAGE32;

typedef struct _PORT_MESSAGE* PPORT_MESSAGE;
// End Lpc


#ifdef NTZW
#undef NTZW
#endif

#define NTZW(a) Nt##a

#include <NtDllFuncInc.h>

#undef NTZW
#define NTZW(a) Zw##a
#include <NtDllFuncInc.h>
#undef NTZW



#ifdef __cplusplus
}
#endif
