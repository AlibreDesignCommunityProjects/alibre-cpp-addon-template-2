// CSampleAddOnInterface.h: interface for the CSampleAddOnInterface class.
// Every addon that is tightly integrated with AD should implement this
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSAMPLEADDONINTERFACE_H__6E56B393_C50B_47E9_B220_D410F163A030__INCLUDED_)
#define AFX_CSAMPLEADDONINTERFACE_H__6E56B393_C50B_47E9_B220_D410F163A030__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSampleAddOnInterface : public IAlibreAddOn   
{
public:
	CSampleAddOnInterface();
	virtual ~CSampleAddOnInterface();

public:

	// Methods from IAlibreAddOn
	HRESULT _stdcall get_RootMenuItem (/*[out,retval]*/ long *pRootMenuID);

	HRESULT _stdcall HasSubMenus (/*[in]*/ long menuID, /*[out,retval]*/ VARIANT_BOOL *pHasSubMenus);

	HRESULT _stdcall SubMenuItems (/*[in]*/ long menuID, /*[out,retval]*/ SAFEARRAY **pSubMenuIDs);

	HRESULT _stdcall MenuItemText (/*[in]*/ long menuID, /*[out,retval]*/ BSTR* pMenuDisplayText);

	HRESULT _stdcall HasPersistentDataToSave(/* [in] */ BSTR sessionIdentifier, /*[retval][out] */ VARIANT_BOOL *IsPopup);

	HRESULT _stdcall PopupMenu (/*[in]*/ long menuID, /*[out,retval]*/ VARIANT_BOOL *IsPopup);

	HRESULT _stdcall MenuItemState (/*[in]*/ long menuID, 
										/*[in] */ BSTR sessionIdentifier, 
										/*[out, retval]*/ enum ADDONMenuStates *pType);

	HRESULT _stdcall MenuItemToolTip (/*[in]*/ long menuID, /*[out, retval]*/ BSTR *pToolTip);

	HRESULT _stdcall InvokeCommand (/*[in]*/ long menuID, 
									/* [in] */ BSTR sessionIdentifier, 
									/*[out, retval]*/ IAlibreAddOnCommand **pCommand);

	HRESULT _stdcall LoadData (/*[in]*/ struct IStream * ppCustomData, /*[in]*/ BSTR sessionIdentifier);

	HRESULT _stdcall SaveData (/*[in]*/ struct IStream * pCustomData, /*[in]*/ BSTR sessionIdentifier);

	HRESULT _stdcall setIsAddOnLicensed (/*[in]*/ VARIANT_BOOL isLicensed);

	HRESULT _stdcall MenuIcon(long, BSTR *);

	HRESULT _stdcall UseDedicatedRibbonTab(VARIANT_BOOL *);


	// Here, we declare the standard COM interfaces that the ActiveX (COM) object should implement
	// IUnknown
	HRESULT _stdcall QueryInterface (REFIID riid, void **ppObj);
	ULONG _stdcall AddRef();
	ULONG _stdcall Release();

	// IDispatch
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

#endif // !defined(AFX_CSAMPLEADDONINTERFACE_H__6E56B393_C50B_47E9_B220_D410F163A030__INCLUDED_)

