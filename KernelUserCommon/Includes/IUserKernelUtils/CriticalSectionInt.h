	/*****************************************************************************************
		 Note For kernel code:
		 When using CriticalSectionShared the thread CAN BE at DISPATCH_LEVEL or less
			In this case for threads at DISPATCH_LEVEL the call will be a blocking call
		 
		 But when using CriticalSectionExclusive the thread MUST BE at APC_LEVEL or less
		 In other words, CriticalSectionExclusive CAN'T be used for a thread that is running at a level greater than APC_LEVEL

		 Note For User mode:
		 The parameter CriticalSectionShared/CriticalSectionExclusive is ignored
	******************************************************************************************/
namespace UserKernelUtilsLib
{
	class CCriticalSection
	{
	public:
		enum class CCriticalSectionType : BYTE
		{
			CriticalSectionShared = 1,
			CriticalSectionExclusive
		};
		typedef class _CriticalSectionLock CriticalSectionLock, * pCriticalSectionLock;

		CCriticalSection(pCriticalSectionLock Lock, CCriticalSectionType LockType);
		~CCriticalSection();

		static NTSTATUS InitLockObject(pCriticalSectionLock* Lock);
		static NTSTATUS DeleteLockObject(pCriticalSectionLock Lock);

	private:
		// delete copy and move constructors and assign operators
		CCriticalSection(CCriticalSection const&);             // Copy construct
		CCriticalSection(CCriticalSection&&);                  // Move construct
		CCriticalSection& operator=(CCriticalSection const&);  // Copy assign
		CCriticalSection& operator=(CCriticalSection&&);       // Move assign

		pCriticalSectionLock m_Lock;
#ifdef _NTDDK_
		CCriticalSectionType m_LockType;
		KLOCK_QUEUE_HANDLE m_LocalData;
#endif
	};
}