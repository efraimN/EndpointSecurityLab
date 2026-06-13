#pragma once

namespace UserKernelUtilsLib
{
	/* Do not use virtual members on this class to avoid Vtable; Warning DO NOT ADD virtual even to the destructor*/
	class ILinkListElement
	{
	public:
// 		ILinkListElement(const ILinkListElement& other) = delete;
// 		CIinkListElement& operator=(const ILinkListElement& other) = delete;
		LIST_ENTRY m_ListElement;

		ILinkListElement();
		~ILinkListElement();
	protected:

	private:
		ILinkListElement(const ILinkListElement& other);
		ILinkListElement& operator=(const ILinkListElement& other);
	};

	class ILinkListLib
	{
	public:
// 		ILinkListLibInt(const ILinkListLibInt& other) = delete;
// 		ILinkListLibInt& operator=(const ILinkListLibInt& other) = delete;
		typedef BOOLEAN(_cdecl* pfnFoundElement)(ILinkListElement* Element, PVOID FindContext);
		typedef BOOLEAN(_cdecl* pfnFoundElementEx)(ILinkListElement* Element, DWORD NumberOfElements, PVOID FindContext);

		static
		ILinkListLib* GetNewInstance();

		static
		VOID FreeInstance(ILinkListLib* Interface);

		virtual
		BOOLEAN Start() = 0;

		virtual
		VOID Stop() = 0;


		virtual
		VOID PushItemHead(ILinkListElement* Element) = 0;

		virtual
		ILinkListElement* PopItemHead() = 0;

		virtual
		ILinkListElement* GetElement(pfnFoundElement FoundElementCallback, PVOID FindContext, BOOLEAN Removal) = 0;

		virtual
		ILinkListElement* GetElementEx(pfnFoundElementEx FoundElementCallback, PVOID FindContext, BOOLEAN Removal) = 0;

		virtual
		VOID PushItemTail(ILinkListElement* Element) = 0;

		virtual
		ILinkListElement* PopItemTail() = 0;

	protected:

		ILinkListLib() {};
		virtual~ILinkListLib() {};

	private:
		ILinkListLib(const ILinkListLib& other);
		ILinkListLib& operator=(const ILinkListLib& other);
	};
}
