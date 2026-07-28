#include "stdafx.h"
#include "ADSampleAddOnDX.h"
#include "CSampleAddOnInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CADSampleAddOnDXApp, CWinApp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CADSampleAddOnDXApp::CADSampleAddOnDXApp()
{
}

/////////////////////////////////////////////////////////////////////////////

CADSampleAddOnDXApp theApp;

APICLIENTAPP_API void AddOnLoad (HWND windowHandle,
				VOID *pAutomationHook,
				VOID *reserved)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (NULL != pAutomationHook)
	{
		IAutomationHook*	pHook = (IAutomationHook *) pAutomationHook;
		theApp.m_pRoot = pHook->GetRoot ();
	}
	theApp.m_windowHandle = windowHandle;
}

APICLIENTAPP_API void AddOnUnload (HWND windowHandle,
	BOOL forceUnload,
	BOOL *cancel,
	VOID *reserved1,
	VOID *reserved2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	theApp.m_pAddOnInterface = NULL;
	theApp.m_pRoot = NULL;
}

APICLIENTAPP_API IUnknown* GetAddOnInterface ()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	IUnknownPtr result = NULL;
	CSampleAddOnInterface* addOnInterface = new CSampleAddOnInterface ();

	if (theApp.m_pAddOnInterface == NULL)
		theApp.m_pAddOnInterface = addOnInterface;

	result = (IUnknown *)addOnInterface;

	return result;
}

APICLIENTAPP_API void AddOnInvoke (HWND windowHandle,
				  VOID *pAutomationHook,
				  LPCSTR sessionName,
				  BOOL isLicensed,
				  VOID *reserved1,
				  VOID *reserved2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
}
