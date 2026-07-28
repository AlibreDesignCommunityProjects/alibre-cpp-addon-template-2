//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ADSampleAddOnDX.h"
#include "DrawTriangleCommand.h"
#include <string>
#include "AddOnSupport.h"
#include <shlwapi.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

int	m_nGlobalDrawCounter;
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

bool DrawTriangle(IADAddOnCanvasDisplay* pCanvasDisplay, LPWSTR pImageFilePath)
{
	float vertices[] = { -5.0f, 0.0f, 0.0f,  5.0f, 0.0f, 0.0f,  0.0f, 5.0f, 0.0f };
	float normals[] = { 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f };
	int indices[] = { 3, 0, 1, 2 };
	float color[] = { 0.0f, 0.5f, 0.0f };

	float vertexUVs[] = { 0.0f,0.0f,    1.0f,1.0f,    0.5f,1.0f };

	float fltOffset = m_nGlobalDrawCounter * 5.0f;

	double transform[] = { 1.0, 0.0, 0.0,
						   0.0, 1.0, 0.0,
						   0.0, 0.0, 1.0,
						   fltOffset, 0.0, 0.0 };

	SAFEARRAY* saVertices = NULL;
	SAFEARRAY* saNormals = NULL;
	SAFEARRAY* saIndices = NULL;
	SAFEARRAY* saRGB = NULL;
	SAFEARRAY* saTransform = NULL;
	SAFEARRAY* saVertexUVparams = NULL;

	if (getSafeArrayFromArray<float>(vertices, (long)9, VT_R4, &saVertices) != S_OK)
		return false;
	if (getSafeArrayFromArray<float>(normals, (long)9, VT_R4, &saNormals) != S_OK)
	{
		SafeArrayDestroy(saVertices);
		return false;
	}
	if (getSafeArrayFromArray<int>(indices, (long)4, VT_I4, &saIndices) != S_OK)
	{
		SafeArrayDestroy(saVertices);
		SafeArrayDestroy(saNormals);
		return false;
	}
	if (getSafeArrayFromArray<float>(color, (long)3, VT_R4, &saRGB) != S_OK)
	{
		SafeArrayDestroy(saVertices);
		SafeArrayDestroy(saNormals);
		SafeArrayDestroy(saIndices);
		return false;
	}
	if (getSafeArrayFromArray<double>(transform, (long)12, VT_R8, &saTransform) != S_OK)
	{
		SafeArrayDestroy(saVertices);
		SafeArrayDestroy(saNormals);
		SafeArrayDestroy(saIndices);
		SafeArrayDestroy(saRGB);
		return false;
	}

	LONG64 result;
	LONG64 mySegment;
	BSTR mySegmentName = SysAllocString(L"My Triangle");
	pCanvasDisplay->AddSubSegment(NULL, mySegmentName, &mySegment);
	pCanvasDisplay->SetSegmentTransform(mySegment, VARIANT_TRUE, &saTransform);
	pCanvasDisplay->SetSegmentColor(mySegment, 0, 255, 0, 255);

	if (getSafeArrayFromArray<float>(vertexUVs, (long)6, VT_R4, &saVertexUVparams) != S_OK)
	{
		SafeArrayDestroy(saVertices);
		SafeArrayDestroy(saNormals);
		SafeArrayDestroy(saIndices);
		SafeArrayDestroy(saRGB);
		SafeArrayDestroy(saTransform);
		SysFreeString(mySegmentName);
		return false;
	}
	BSTR textureName = SysAllocString(L"Wood");
	BSTR imagePath = SysAllocString(pImageFilePath);
	pCanvasDisplay->DefineTexture(mySegment, textureName, ImageFormat_JPEG, imagePath);
	pCanvasDisplay->SetFaceTexture(mySegment, textureName);
	pCanvasDisplay->DrawTexturedMesh(mySegment, &saVertices, &saNormals, &saVertexUVparams, &saIndices, &result);
	SafeArrayDestroy(saVertexUVparams);
	SysFreeString(textureName);
	SysFreeString(imagePath);

	pCanvasDisplay->DrawPolyline(mySegment, &saVertices, &result);
	pCanvasDisplay->SetLineWeight(mySegment, 2.0f);
	pCanvasDisplay->ToggleForegroundRendering(mySegment, true);

	long radius = 15;
	pCanvasDisplay->DrawMarker(mySegment, vertices[0], vertices[1], vertices[2], radius, MarkerType::MarkerType_MARKER_RECT);
	pCanvasDisplay->DrawMarker(mySegment, vertices[3], vertices[4], vertices[5], radius, MarkerType::MarkerType_MARKER_RECT);

	SafeArrayDestroy(saVertices);
	SafeArrayDestroy(saNormals);
	SafeArrayDestroy(saIndices);
	SafeArrayDestroy(saRGB);
	SafeArrayDestroy(saTransform);
	SysFreeString(mySegmentName);

	return true;
}

CDrawTriangleCommand::CDrawTriangleCommand()
{
	m_nRefCount = 0;
	m_pCmdSite = NULL;
	m_bClearViewPort = VARIANT_FALSE;
	m_bOverrideRender = VARIANT_FALSE;
	m_bLegacyRenderer = VARIANT_TRUE;
	m_bIsOutOfDate = true;
}

CDrawTriangleCommand::CDrawTriangleCommand(VARIANT_BOOL bOverrideRender, VARIANT_BOOL bClearViewPort)
{
	m_nRefCount = 0;
	m_pCmdSite = NULL;
	m_bClearViewPort = bClearViewPort;
	m_bOverrideRender = bOverrideRender;
	m_bLegacyRenderer = VARIANT_TRUE;
	m_bIsOutOfDate = true;

	LPWSTR strDLLPath = new WCHAR[_MAX_PATH];
	if (strDLLPath)
	{
		::GetModuleFileNameW((HINSTANCE)&__ImageBase, strDLLPath, _MAX_PATH);
		::PathRemoveFileSpecW(strDLLPath);
		::PathCombineW(m_imageFilePath, strDLLPath, L"wood.jpg");
		delete[] strDLLPath;
	}
}

CDrawTriangleCommand::~CDrawTriangleCommand()
{
	if (m_pCmdSite)
	{
		m_pCmdSite->Release();
		m_pCmdSite = NULL;
	}
}

HRESULT _stdcall CDrawTriangleCommand::AddTab(VARIANT_BOOL* pAddTab) {  *pAddTab = VARIANT_FALSE; return S_OK; }
HRESULT _stdcall CDrawTriangleCommand::get_TabName(BSTR* pTabName) {  if (!pTabName) return E_POINTER; *pTabName = NULL; return S_OK; }
HRESULT _stdcall CDrawTriangleCommand::get_CommandSite(IADAddOnCommandSite** pSite) {  if (!pSite) return E_POINTER; *pSite = NULL; if (m_pCmdSite) return m_pCmdSite->QueryInterface(__uuidof(IADAddOnCommandSite), (void**)pSite); return E_UNEXPECTED; }
HRESULT _stdcall CDrawTriangleCommand::IsTwoWayToggle(VARIANT_BOOL* pIsTwoWayToggle) {  *pIsTwoWayToggle = VARIANT_FALSE; return S_OK; }
HRESULT _stdcall CDrawTriangleCommand::OnClick(long, long, enum ADDONMouseButtons, VARIANT_BOOL* pIsHandled) {  *pIsHandled = VARIANT_FALSE; return S_OK; }
HRESULT _stdcall CDrawTriangleCommand::OnComplete(void) {  if (m_bOverrideRender && m_pCmdSite) m_pCmdSite->Override3DRender(VARIANT_TRUE); m_bIsOutOfDate = true; if (m_pCmdSite) m_pCmdSite->InvalidateCanvas(); return S_OK; }
HRESULT _stdcall CDrawTriangleCommand::OnMouseDown(long, long, enum ADDONMouseButtons, VARIANT_BOOL* pIsHandled) {  *pIsHandled = VARIANT_FALSE; return S_OK; }
HRESULT _stdcall CDrawTriangleCommand::OnMouseMove(long, long, enum ADDONMouseButtons, VARIANT_BOOL* pIsHandled) {  *pIsHandled = VARIANT_FALSE; return S_OK; }
HRESULT _stdcall CDrawTriangleCommand::OnMouseUp(long, long, enum ADDONMouseButtons, VARIANT_BOOL* pIsHandled) {  *pIsHandled = VARIANT_FALSE; return S_OK; }
HRESULT _stdcall CDrawTriangleCommand::OnMouseWheel(double, VARIANT_BOOL* pIsHandled) {  *pIsHandled = VARIANT_FALSE; return S_OK; }
HRESULT _stdcall CDrawTriangleCommand::OnKeyDown(long, VARIANT_BOOL* pIsHandled) {  m_nGlobalDrawCounter++; m_bIsOutOfDate = true; if (m_pCmdSite) m_pCmdSite->UpdateCanvas(); *pIsHandled = VARIANT_TRUE; return S_OK; }
HRESULT _stdcall CDrawTriangleCommand::OnKeyUp(long, VARIANT_BOOL* pIsHandled) {  *pIsHandled = VARIANT_FALSE; return S_OK; }
HRESULT _stdcall CDrawTriangleCommand::OnEscape(VARIANT_BOOL* pIsHandled) {  if (m_pCmdSite) { m_pCmdSite->Override3DRender(VARIANT_FALSE); m_pCmdSite->InvalidateCanvas(); m_pCmdSite->Terminate(); } *pIsHandled = VARIANT_TRUE; return S_OK; }
HRESULT _stdcall CDrawTriangleCommand::OnDoubleClick(long, long, VARIANT_BOOL* pIsHandled) {  if (m_pCmdSite) { m_pCmdSite->Override3DRender(VARIANT_FALSE); m_pCmdSite->InvalidateCanvas(); m_pCmdSite->Terminate(); } *pIsHandled = VARIANT_TRUE; return S_OK; }
HRESULT _stdcall CDrawTriangleCommand::OnSelectionChange(void) {  return S_OK; }
HRESULT _stdcall CDrawTriangleCommand::OnTerminate(void) {  return S_OK; }

HRESULT _stdcall CDrawTriangleCommand::On3DRender(void)
{
	if (m_bLegacyRenderer == VARIANT_FALSE)
	{
		if (m_bIsOutOfDate)
		{
			if (!m_pCmdSite) return E_UNEXPECTED;
			IUnknown* pUnkDisplayContext = NULL;
			IADAddOnCanvasDisplay* pCanvasDisplay = NULL;
			m_pCmdSite->Begin3DDisplay(m_bClearViewPort, &pUnkDisplayContext);
			if (pUnkDisplayContext)
			{
				HRESULT hr = pUnkDisplayContext->QueryInterface(__uuidof(IADAddOnCanvasDisplay), (void**)&pCanvasDisplay);
				if (SUCCEEDED(hr) && pCanvasDisplay)
				{
					DrawTriangle(pCanvasDisplay, m_imageFilePath);
					pCanvasDisplay->Release();
				}
				pUnkDisplayContext->Release();
			}
			m_pCmdSite->End3DDisplay();
			m_bIsOutOfDate = false;
		}
	}
	return S_OK;
}

HRESULT _stdcall CDrawTriangleCommand::OnRender(long, long, long, long, long) {  return S_OK; }

HRESULT _stdcall CDrawTriangleCommand::putref_CommandSite(IADAddOnCommandSite* pSite)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	try
	{
		if (m_pCmdSite)
		{
			m_pCmdSite->Release();
			m_pCmdSite = NULL;
		}
		if (pSite)
		{
			m_pCmdSite = pSite;
			m_pCmdSite->AddRef();
		}
	}
	catch (...)
	{
		AfxMessageBox("Exception caught in CDrawTriangleCommand::putref_CommandSite");
	}
	return S_OK;
}

HRESULT _stdcall CDrawTriangleCommand::OnShowUI(__int64) {  return S_OK; }
HRESULT _stdcall CDrawTriangleCommand::get_Extents(SAFEARRAY** pResult) {  if (!pResult) return E_POINTER; *pResult = NULL; return S_FALSE; }

ULONG _stdcall CDrawTriangleCommand::AddRef() { return InterlockedIncrement(&m_nRefCount); }
HRESULT _stdcall CDrawTriangleCommand::QueryInterface(REFIID riid, void** ppObj)
{
	if (!ppObj) return E_POINTER;
	*ppObj = NULL;
	if (riid == IID_IUnknown) { *ppObj = static_cast<IUnknown*>(this); }
	else if (riid == __uuidof(IAlibreAddOnCommand)) { *ppObj = static_cast<IAlibreAddOnCommand*>(this); }
	else { return E_NOINTERFACE; }
	AddRef();
	return S_OK;
}
ULONG _stdcall CDrawTriangleCommand::Release() { long nRefCount = InterlockedDecrement(&m_nRefCount); if (nRefCount == 0) delete this; return nRefCount; }

long _stdcall CDrawTriangleCommand::GetTypeInfoCount(UINT FAR* pctinfo) { if (!pctinfo) return E_POINTER; *pctinfo = 1; return NOERROR; }
long _stdcall CDrawTriangleCommand::GetTypeInfo(UINT iTInfo, LCID, ITypeInfo FAR* FAR* ppTInfo)
{
	if (!ppTInfo) return E_POINTER; *ppTInfo = NULL;
	if (iTInfo != 0) return DISP_E_BADINDEX;
	return E_NOTIMPL;
}
long _stdcall CDrawTriangleCommand::GetIDsOfNames(REFIID, OLECHAR FAR* FAR*, UINT, LCID, DISPID FAR*)
{
	return E_NOTIMPL;
}
long _stdcall CDrawTriangleCommand::Invoke(DISPID, REFIID, LCID, WORD, DISPPARAMS FAR*, VARIANT FAR*, EXCEPINFO FAR*, UINT FAR*)
{
	return E_NOTIMPL;
}
