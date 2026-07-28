//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSAMPLEADDONINTERFACE_H__6E56B393_C50B_47E9_B220_D410F163A030__INCLUDED_)
#define AFX_CSAMPLEADDONINTERFACE_H__6E56B393_C50B_47E9_B220_D410F163A030__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

class CSampleAddOnInterface : public IAlibreAddOn
{
public:
	CSampleAddOnInterface();
	virtual ~CSampleAddOnInterface();

public:

	HRESULT _stdcall get_RootMenuItem ( long *pRootMenuID);

	HRESULT _stdcall HasSubMenus ( long menuID,  VARIANT_BOOL *pHasSubMenus);

	HRESULT _stdcall SubMenuItems ( long menuID,  SAFEARRAY **pSubMenuIDs);

	HRESULT _stdcall MenuItemText ( long menuID,  BSTR* pMenuDisplayText);

	HRESULT _stdcall HasPersistentDataToSave( BSTR sessionIdentifier,  VARIANT_BOOL *IsPopup);

	HRESULT _stdcall PopupMenu ( long menuID,  VARIANT_BOOL *IsPopup);

	HRESULT _stdcall MenuItemState ( long menuID,
										 BSTR sessionIdentifier,
										 enum ADDONMenuStates *pType);

	HRESULT _stdcall MenuItemToolTip ( long menuID,  BSTR *pToolTip);

	HRESULT _stdcall InvokeCommand ( long menuID,
									 BSTR sessionIdentifier,
									 IAlibreAddOnCommand **pCommand);

	HRESULT _stdcall LoadData ( struct IStream * ppCustomData,  BSTR sessionIdentifier);

	HRESULT _stdcall SaveData ( struct IStream * pCustomData,  BSTR sessionIdentifier);

	HRESULT _stdcall setIsAddOnLicensed ( VARIANT_BOOL isLicensed);

	HRESULT _stdcall MenuIcon(long, BSTR *);

	HRESULT _stdcall UseDedicatedRibbonTab(VARIANT_BOOL *);

	HRESULT _stdcall QueryInterface (REFIID riid, void **ppObj);
	ULONG _stdcall AddRef();
	ULONG _stdcall Release();

	long _stdcall GetTypeInfoCount(UINT FAR* pctinfo);
	long _stdcall GetTypeInfo(
		UINT iTInfo,
		LCID lcid,
		ITypeInfo FAR* FAR* ppTInfo);
	long _stdcall GetIDsOfNames(
		REFIID riid,
		OLECHAR FAR* FAR* rgszNames,
		UINT cNames,
		LCID lcid,
		DISPID FAR* rgDispId);

	long _stdcall Invoke(
		DISPID dispidMember,
		REFIID riid,
		LCID lcid,
		WORD wFlags,
		DISPPARAMS FAR* pDispParams,
		VARIANT FAR* pVarResult,
		EXCEPINFO FAR* pExcepInfo,
		UINT FAR* puArgErr);

private:
	long			m_nRefCount;
	ITypeInfo		*m_ptinfo;
	SAFEARRAY		*m_RootSubMenuIDs;

	void initializeMenus ();
};

#endif
