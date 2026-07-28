#if !defined(AFX_STDAFX_H__AC28B15C_A45C_41A5_AD1B_48306C64D8DB__INCLUDED_)
#define AFX_STDAFX_H__AC28B15C_A45C_41A5_AD1B_48306C64D8DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#define VC_EXTRALEAN

#include <afxwin.h>
#include <afxext.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>
#include <afxodlgs.h>
#include <afxdisp.h>
#endif

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>
#endif

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>
#endif

#include <afxdtctl.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif

#pragma warning( disable : 4786 )

#include "objidl.h"
#include <math.h>
#include <afxtempl.h>
#include <afxcoll.h>

#import "AlibreX_64.tlb" rename("SendMessage", "AlibreSendMessage")
using namespace AlibreX;
#import "AlibreAddOn_64.tlb" raw_interfaces_only exclude("IStream")
using namespace AlibreAddOn;

#endif
