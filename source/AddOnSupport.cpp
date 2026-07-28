#include "stdafx.h"
#include "ADSampleAddOnDX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

template<typename T>
HRESULT getSafeArrayFromArray ( T* pBuffer,
							    long size,
							    enum VARENUM VT_Size,
							   SAFEARRAY** ppNums)
{
	HRESULT		hr = S_OK;
	T			*pSafeArrayData;

	*ppNums = NULL;

	if ((NULL == pBuffer) || (size <= 0))
	{
		return E_INVALIDARG;
	}

	if ((*ppNums = SafeArrayCreateVector (VT_Size, 1, size)) == NULL)
	{
		_ASSERT (FALSE);
		return E_OUTOFMEMORY;
	}

	if ((hr = SafeArrayAccessData (*ppNums, (void **)&pSafeArrayData)) != S_OK)
	{
		_ASSERT (FALSE);
		return hr;
	}

	for (int i = 0; i < size; i++)
	{
		pSafeArrayData[i] = pBuffer[i];
	}

	if ((hr = SafeArrayUnaccessData (*ppNums)) != S_OK)
	{
		_ASSERT (FALSE);
		return hr;
	}

	return hr;
}

template HRESULT getSafeArrayFromArray<float>(float *, long, enum VARENUM, SAFEARRAY**);
template HRESULT getSafeArrayFromArray<int>(int *, long, enum VARENUM, SAFEARRAY**);
template HRESULT getSafeArrayFromArray<double>(double *, long, enum VARENUM, SAFEARRAY**);
