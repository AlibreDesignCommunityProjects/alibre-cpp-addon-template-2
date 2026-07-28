#if !defined(AFX_ADSAMPLEADDONDX_H__3DBA9460_715C_44C2_A200_9962B4B9C179__INCLUDED_)
#define AFX_ADSAMPLEADDONDX_H__3DBA9460_715C_44C2_A200_9962B4B9C179__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////

class CADSampleAddOnDXApp : public CWinApp
{
public:
	CADSampleAddOnDXApp();

	DECLARE_MESSAGE_MAP()

public:

	IADRootPtr					m_pRoot;
	IUnknownPtr					m_pAddOnInterface;
	HWND						m_windowHandle;
};

#ifdef APICLIENTAPP_EXPORTS
#define APICLIENTAPP_API __declspec(dllexport)
#else
#define APICLIENTAPP_API __declspec(dllimport)
#endif

extern "C"
{
APICLIENTAPP_API void AddOnLoad (HWND windowHandle,
				VOID *pAutomationHook,
				VOID *reserved);

APICLIENTAPP_API void AddOnInvoke (HWND windowHandle,
				  VOID *pAutomationHook,
				  LPCSTR sessionName,
				  BOOL isLicensed,
				  VOID *reserved1,
				  VOID *reserved2);

APICLIENTAPP_API void AddOnUnload (HWND windowHandle,
				  BOOL forceUnload,
				  BOOL *cancel,
				  VOID *reserved1,
				  VOID *reserved2);

APICLIENTAPP_API IUnknown* GetAddOnInterface ();
}

/////////////////////////////////////////////////////////////////////////////

#endif
