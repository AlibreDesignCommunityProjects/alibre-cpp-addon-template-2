// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__AC28B15C_A45C_41A5_AD1B_48306C64D8DB__INCLUDED_)
#define AFX_STDAFX_H__AC28B15C_A45C_41A5_AD1B_48306C64D8DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#pragma warning( disable : 4786 )

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#include "objidl.h"
#include <math.h>
#include <afxtempl.h>
#include <afxcoll.h>

// Edit the folder path to the type library files below based on where Alibre Design is installed on your computer
#import "C:\Program Files\Alibre Design 29.0.0.29060\Program\AlibreX_64.tlb" rename("SendMessage", "AlibreSendMessage")
using namespace AlibreX;
#import "C:\Program Files\Alibre Design 29.0.0.29060\Program\AlibreAddOn_64.tlb" raw_interfaces_only exclude("IStream")
using namespace AlibreAddOn;


#endif // !defined(AFX_STDAFX_H__AC28B15C_A45C_41A5_AD1B_48306C64D8DB__INCLUDED_)
