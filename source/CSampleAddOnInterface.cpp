// CSampleAddOnInterface.cpp: implementation of the CSampleAddOnInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ADSampleAddOnDX.h"
#include "CSampleAddOnInterface.h"
#include "AddOnSupport.h"
#include "DrawTriangleCommand.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern int m_nGlobalDrawCounter;

CSampleAddOnInterface::CSampleAddOnInterface()
{

	initializeMenus();
	m_nRefCount = 0;
	m_nGlobalDrawCounter = 0;
	m_ptinfo = NULL;

}

CSampleAddOnInterface::~CSampleAddOnInterface()
{

}

//
// IAlibreAddOn interface methods implementation
//
HRESULT _stdcall CSampleAddOnInterface::get_RootMenuItem (/*[out,retval]*/ long *pRootMenuID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// Return the ID of addon's root menu

	*pRootMenuID = 	nROOT_MENU_ID;
	return S_OK;
}


HRESULT _stdcall CSampleAddOnInterface::HasSubMenus (/*[in]*/ long menuID, 
													/*[out,retval]*/ VARIANT_BOOL *pHasSubMenus)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// Our addon has a root menu that has a sub-menu containing just two commands
	*pHasSubMenus = (nROOT_MENU_ID == menuID) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}


HRESULT _stdcall CSampleAddOnInterface::SubMenuItems (/*[in]*/ long menuID, 
													 /*[out,retval]*/ SAFEARRAY **pSubMenuIDs)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// Initialize the out-parameter so we never marshal an uninitialized SAFEARRAY**
	*pSubMenuIDs = NULL;

	// Our addon has a root menu that has a sub-menu containing just two commands
	if (nROOT_MENU_ID == menuID)
	{
		SafeArrayCopy (m_RootSubMenuIDs, pSubMenuIDs);
	}

	return S_OK;
}


HRESULT _stdcall CSampleAddOnInterface::MenuItemText (/*[in]*/ long menuID, /*[out,retval]*/ BSTR* pMenuDisplayText)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// Return the text for the menu that addon wants to show up on Alibre's menu
	if(menuID == nROOT_MENU_ID)
	{
		*pMenuDisplayText = _bstr_t (cStrROOT_MENU);
	}
	else if (menuID == nPOST_RENDER_MENU_ID)
	{
		*pMenuDisplayText = _bstr_t (cStrPOST_RENDER_MENU);
	}	
	else if(menuID == nOVERRIDE_RENDER_MENU_ID)
	{
		*pMenuDisplayText = _bstr_t (cStrOVERRIDE_RENDER_MENU);
	}    

	return S_OK;
}


HRESULT _stdcall CSampleAddOnInterface::MenuItemState (/*[in]*/ long menuID, 
													  /*[in]*/ BSTR sessionIdentifier, 
													  /*[out, retval]*/ enum ADDONMenuStates *pType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// This simple addon wants to keeps all its commands always enabled.
	*pType = ADDONMenuStates_ADDON_MENU_ENABLED;

	return S_OK;
}

HRESULT _stdcall CSampleAddOnInterface::MenuItemToolTip (/*[in]*/ long menuID, 
														/*[out, retval]*/ BSTR *pToolTip)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// This simple addon does not show menu tool tip.
	return S_OK;
}

HRESULT _stdcall CSampleAddOnInterface::PopupMenu (/*[in]*/ long menuID, 
												  /*[out,retval]*/ VARIANT_BOOL *IsPopup)
{
	// Deprecated method. Just return S_OK
	return S_OK;
}

HRESULT _stdcall CSampleAddOnInterface::HasPersistentDataToSave(/*[in]*/ BSTR sessionIdentifier,
																/*[retval][out]*/ VARIANT_BOOL *pHasDataToSave)
{
	// This addon does not save any data into Alibre's file
	*pHasDataToSave = VARIANT_FALSE;
	return S_OK;
}


HRESULT _stdcall CSampleAddOnInterface::setIsAddOnLicensed (/*[in]*/ VARIANT_BOOL isLicensed)
{
	// This is relevant only if addon licensing is part of Alibre's license.
	return S_OK;
}

HRESULT _stdcall CSampleAddOnInterface::InvokeCommand (/*[in]*/ long menuID, 
												  /*[in]*/ BSTR sessionIdentifier, 
												  /*[out, retval]*/ IAlibreAddOnCommand **pCommand)
{

/** 
	Both addon commands implemented here are for rendering to Alibre's graphics window; See
	how they call Begin3DDisplay in OnRender/On3DRender function in DrawTriangleCommand

	POST_RENDER command renders its Triangle after Alibre finishes its display
	
	OVERRIDE_RENDER command renders its Triangle after suppressing (overriding) Alibre's
	display

*/

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	try
	{

		CDrawTriangleCommand*	pDrawTriangleCommand = NULL;
		
		if (nPOST_RENDER_MENU_ID == menuID)
		{
			pDrawTriangleCommand = new CDrawTriangleCommand(VARIANT_FALSE, VARIANT_FALSE);
		}
		else if (nOVERRIDE_RENDER_MENU_ID == menuID)
		{
			pDrawTriangleCommand = new CDrawTriangleCommand(VARIANT_TRUE, VARIANT_TRUE);
			
		}

		if (pDrawTriangleCommand)
		{
			pDrawTriangleCommand->QueryInterface(__uuidof (IAlibreAddOnCommand), (void **) pCommand);
		}
	}
	catch (...)
	{
		AfxMessageBox ("Exception caught in CSampleAddOnInterface::InvokeCommand"); 
	}
		
	return S_OK;
}


HRESULT _stdcall CSampleAddOnInterface::SaveData (/*[in]*/ struct IStream * pCustomData, 
											/*[in]*/ BSTR sessionIdentifier)
{
	// This add-on does not save any persistent data
	return S_OK;
}


HRESULT _stdcall CSampleAddOnInterface::LoadData (/*[in]*/ struct IStream * ppCustomData, 
											/*[in]*/ BSTR sessionIdentifier)
{
	// No persistent data to load
	return S_OK;
}

HRESULT _stdcall CSampleAddOnInterface::MenuIcon(long id, BSTR * pMenuIconPath)
{
	// This addon is not providing any icon to be displayed next to its menu commands
	*pMenuIconPath = NULL;
	return S_OK;
}

HRESULT _stdcall CSampleAddOnInterface::UseDedicatedRibbonTab(VARIANT_BOOL * pFlag)
{
	// This addon does not have a dedicated tab on Alibre's Ribbon
	*pFlag = VARIANT_FALSE;
	return S_OK;
}

// Internal class instance methods
void CSampleAddOnInterface::initializeMenus()
{

	// Build Root Menus' Array
	int *pRootMenus = new int[nMAIN_MENUS_COUNT];

	pRootMenus[0] = nPOST_RENDER_MENU_ID;
	pRootMenus[1] = nOVERRIDE_RENDER_MENU_ID;

	getSafeArrayFromArray<int> (pRootMenus, nMAIN_MENUS_COUNT, VT_INT, &m_RootSubMenuIDs);

	delete[] pRootMenus;
}


//
// Below, we implement the standard COM interfaces (IUnknown, IDispatch) 
//
HRESULT _stdcall CSampleAddOnInterface::QueryInterface(REFIID riid, void **ppObj)
{
	if (riid == IID_IUnknown)
	{
		*ppObj = static_cast <IUnknown *> (this);
		AddRef();
		return S_OK;
	}

	if (riid == __uuidof(IAlibreAddOn))
	{
		*ppObj = static_cast <IAlibreAddOn *>(this);
		AddRef();
		return S_OK;

	}

	//If control reaches here then, let the client 
	//know that we do not satisfy the required interface.

	*ppObj = NULL;
	return E_NOINTERFACE;

}


ULONG _stdcall CSampleAddOnInterface::AddRef()
{
	long nRefCount = 0;
	nRefCount = InterlockedIncrement (&m_nRefCount);
	return nRefCount;
}


ULONG _stdcall CSampleAddOnInterface::Release()

{
	long nRefCount = 0;
	nRefCount = InterlockedDecrement (&m_nRefCount);
	if (nRefCount == 0) delete this;
	return nRefCount;

}


long _stdcall  CSampleAddOnInterface::GetIDsOfNames(
	REFIID riid,
	OLECHAR FAR* FAR* rgszNames,
	UINT cNames,
	LCID lcid,
	DISPID FAR* rgDispId)
{
	// Original logic:
	// if (!m_ptinfo) return E_FAIL; // m_ptinfo must be valid
	// return DispGetIDsOfNames(m_ptinfo, rgszNames, cNames, rgDispId);
	return E_NOTIMPL; // Placeholder: m_ptinfo setup is required.
}

long _stdcall  CSampleAddOnInterface::GetTypeInfo(
	UINT iTInfo,
	LCID lcid,
	ITypeInfo FAR* FAR* ppTInfo)
{
	*ppTInfo = NULL;

	if (iTInfo != 0)
		return ResultFromScode(DISP_E_BADINDEX);

	// Original logic:
	// if (!m_ptinfo) return E_FAIL; // m_ptinfo must be valid
	// m_ptinfo->AddRef();
	// *ppTInfo = m_ptinfo;
	// return NOERROR;
	return E_NOTIMPL; // Placeholder: m_ptinfo setup is required.
}


long _stdcall CSampleAddOnInterface::GetTypeInfoCount(UINT FAR* pctinfo)
{
	*pctinfo = 0; // No type info available (m_ptinfo is not set up)
	return S_OK;
}


long _stdcall  CSampleAddOnInterface::Invoke(
	DISPID dispidMember,
	REFIID riid,
	LCID lcid,
	WORD wFlags,
	DISPPARAMS FAR* pDispParams,
	VARIANT FAR* pVarResult,
	EXCEPINFO FAR* pExcepInfo,
	UINT FAR* puArgErr)
{
	// Original logic:
	// if (!m_ptinfo) return E_FAIL; // m_ptinfo must be valid
	// return DispInvoke(this, m_ptinfo, dispidMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
	return E_NOTIMPL; // Placeholder: m_ptinfo setup is required.
}





