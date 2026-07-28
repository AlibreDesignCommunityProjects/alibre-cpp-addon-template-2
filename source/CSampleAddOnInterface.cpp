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

HRESULT _stdcall CSampleAddOnInterface::get_RootMenuItem ( long *pRootMenuID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pRootMenuID = 	nROOT_MENU_ID;
	return S_OK;
}

HRESULT _stdcall CSampleAddOnInterface::HasSubMenus ( long menuID,
													 VARIANT_BOOL *pHasSubMenus)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pHasSubMenus = (nROOT_MENU_ID == menuID) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT _stdcall CSampleAddOnInterface::SubMenuItems ( long menuID,
													  SAFEARRAY **pSubMenuIDs)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pSubMenuIDs = NULL;

	if (nROOT_MENU_ID == menuID)
	{
		SafeArrayCopy (m_RootSubMenuIDs, pSubMenuIDs);
	}

	return S_OK;
}

HRESULT _stdcall CSampleAddOnInterface::MenuItemText ( long menuID,  BSTR* pMenuDisplayText)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

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

HRESULT _stdcall CSampleAddOnInterface::MenuItemState ( long menuID,
													   BSTR sessionIdentifier,
													   enum ADDONMenuStates *pType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pType = ADDONMenuStates_ADDON_MENU_ENABLED;

	return S_OK;
}

HRESULT _stdcall CSampleAddOnInterface::MenuItemToolTip ( long menuID,
														 BSTR *pToolTip)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	return S_OK;
}

HRESULT _stdcall CSampleAddOnInterface::PopupMenu ( long menuID,
												   VARIANT_BOOL *IsPopup)
{
	return S_OK;
}

HRESULT _stdcall CSampleAddOnInterface::HasPersistentDataToSave( BSTR sessionIdentifier,
																 VARIANT_BOOL *pHasDataToSave)
{
	*pHasDataToSave = VARIANT_FALSE;
	return S_OK;
}

HRESULT _stdcall CSampleAddOnInterface::setIsAddOnLicensed ( VARIANT_BOOL isLicensed)
{
	return S_OK;
}

HRESULT _stdcall CSampleAddOnInterface::InvokeCommand ( long menuID,
												   BSTR sessionIdentifier,
												   IAlibreAddOnCommand **pCommand)
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

HRESULT _stdcall CSampleAddOnInterface::SaveData ( struct IStream * pCustomData,
											 BSTR sessionIdentifier)
{
	return S_OK;
}

HRESULT _stdcall CSampleAddOnInterface::LoadData ( struct IStream * ppCustomData,
											 BSTR sessionIdentifier)
{
	return S_OK;
}

HRESULT _stdcall CSampleAddOnInterface::MenuIcon(long id, BSTR * pMenuIconPath)
{
	*pMenuIconPath = NULL;
	return S_OK;
}

HRESULT _stdcall CSampleAddOnInterface::UseDedicatedRibbonTab(VARIANT_BOOL * pFlag)
{
	*pFlag = VARIANT_FALSE;
	return S_OK;
}

void CSampleAddOnInterface::initializeMenus()
{
	int *pRootMenus = new int[nMAIN_MENUS_COUNT];

	pRootMenus[0] = nPOST_RENDER_MENU_ID;
	pRootMenus[1] = nOVERRIDE_RENDER_MENU_ID;

	getSafeArrayFromArray<int> (pRootMenus, nMAIN_MENUS_COUNT, VT_INT, &m_RootSubMenuIDs);

	delete[] pRootMenus;
}

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
	return E_NOTIMPL;
}

long _stdcall  CSampleAddOnInterface::GetTypeInfo(
	UINT iTInfo,
	LCID lcid,
	ITypeInfo FAR* FAR* ppTInfo)
{
	*ppTInfo = NULL;

	if (iTInfo != 0)
		return ResultFromScode(DISP_E_BADINDEX);

	return E_NOTIMPL;
}

long _stdcall CSampleAddOnInterface::GetTypeInfoCount(UINT FAR* pctinfo)
{
	*pctinfo = 0;
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
	return E_NOTIMPL;
}
