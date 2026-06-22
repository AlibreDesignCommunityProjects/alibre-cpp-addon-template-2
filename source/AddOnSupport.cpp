#include "stdafx.h"
#include "ADSampleAddOnDX.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

template<typename T>
HRESULT getSafeArrayFromArray (/*IN*/ T* pBuffer,
							   /*IN*/ long size,
							   /*IN*/ enum VARENUM VT_Size,
							   /*OUT*/SAFEARRAY** ppNums)
{
	HRESULT		hr = S_OK;
	T			*pSafeArrayData;

	// initialize the output parameters
	*ppNums = NULL;

	// check the input parameters
	if ((NULL == pBuffer) || (size <= 0))
	{
		return E_INVALIDARG;
	}

	// Wrap the buffer of int's in a SAFEARRAY
	// Create a SAFEARRAY of Int's and allocate the required amount of memory. 

	if ((*ppNums = SafeArrayCreateVector (VT_Size, 1, size)) == NULL)
	{
		_ASSERT (FALSE);
		return E_OUTOFMEMORY;
	}

	// Gain access to memory allocated for the SAFEARRAY.  This increments the lock count.
	if ((hr = SafeArrayAccessData (*ppNums, (void **)&pSafeArrayData)) != S_OK)
	{
		_ASSERT (FALSE);
		return hr;
	}

	for (int i = 0; i < size; i++)
	{
		pSafeArrayData[i] = pBuffer[i];
	}

	// Decrement the lock count on the SAFEARRAY.
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
