# alibre-cpp-addon-template-2 — Code Review (Correctness)

**Date:** 2026-06-20
**Scope:** Second-opinion review, code only (correctness bugs).

**Summary: 6 bugs — 1 High, 4 Medium, 1 Low**

## High

- **CSampleAddOnInterface.cpp:24-31 (constructor) / used at 291, 304-305, 329** — `m_ptinfo` (ITypeInfo*) is never initialized in the constructor and is never assigned anywhere, yet `GetIDsOfNames`, `GetTypeInfo`, and `Invoke` dereference/pass it (`m_ptinfo->AddRef()`, `DispGetIDsOfNames(m_ptinfo, ...)`, `DispInvoke(this, m_ptinfo, ...)`). It is a raw class member, so it holds an indeterminate value; any IDispatch call on the add-on interface dereferences garbage and crashes the host. (Note: the parallel CDrawTriangleCommand IDispatch methods were defensively stubbed to `E_NOTIMPL`, but the CSampleAddOnInterface ones were not.)

## Medium

- **CSampleAddOnInterface.cpp:225,232 — array/scalar `delete` mismatch.** `pRootMenus` is allocated with `new int[nMAIN_MENUS_COUNT]` but freed with scalar `delete pRootMenus;` instead of `delete[] pRootMenus;`. Mismatched new[]/delete is undefined behavior.

- **DrawTriangleCommand.cpp:119-138 (2-arg constructor) — `m_nGlobalDrawCounter = 0` resets the global offset on every command creation.** The triangle-offset feature increments this counter in `OnKeyDown` (line 164) and `DrawTriangle` reads it to offset each triangle (line 35). Resetting it in the constructor zeroes the accumulated offset every time a new command object is created, defeating the feature; the default constructor (line 108) does not reset it, so the behavior is also inconsistent.

- **DrawTriangleCommand.cpp:51-71 — SAFEARRAY/BSTR leaks on the error paths in `DrawTriangle`.** Each `if (getSafeArrayFromArray(...) != S_OK) return false;` returns without destroying the SAFEARRAYs already created earlier in the function; the failure at line 70-71 additionally leaks `saVertices..saTransform` and `mySegmentName` (created at line 64). The success path frees everything, but any failure leaks COM-allocated memory.

- **CSampleAddOnInterface.cpp:64-76 — `SubMenuItems` returns `S_OK` without setting `*pSubMenuIDs` for any non-root menu ID.** When `menuID != nROOT_MENU_ID` the out-param is left untouched and `S_OK` is returned, so the caller marshals an uninitialized/garbage SAFEARRAY**. The out-parameter should be initialized to NULL up front.

## Low

- **AddOnSupport.cpp:26,35 — `getSafeArrayFromArray` returns Win32 error codes (`ERROR_INVALID_PARAMETER`, `ERROR_NOT_ENOUGH_MEMORY`) typed as HRESULT.** These are not HRESULTs; callers only test `!= S_OK` so control flow happens to work, but propagating these values as an HRESULT (e.g., if ever returned to COM) misreports the error and `FAILED()`/`SUCCEEDED()` checks on them would be wrong (`ERROR_INVALID_PARAMETER` = 0x57 is a "success" HRESULT).

## Note

- `GetAddOnInterface` (ADSampleAddOnDX.cpp:94-108) was examined for a COM ref-count leak; the double-AddRef (one for the stored `m_pAddOnInterface`, one for the returned `result`) is the intended pattern for caching the interface, so it is not flagged.

---

## Fixes applied — 2026-06-20

- **[High] `CSampleAddOnInterface.cpp`** — `m_ptinfo` initialized to `NULL` in the ctor; `GetIDsOfNames`/`GetTypeInfo`/`Invoke` now return `E_NOTIMPL` without dereferencing it; `GetTypeInfoCount` sets `*pctinfo = 0`/returns `S_OK` (mirrors `CDrawTriangleCommand`).
- **[Medium] `CSampleAddOnInterface.cpp`** — `delete pRootMenus;` → `delete[] pRootMenus;`.
- **[Medium] `DrawTriangleCommand.cpp`** — removed the `m_nGlobalDrawCounter = 0;` reset from the 2-arg constructor (now matches the default ctor; offset feature preserved).
- **[Medium] `DrawTriangleCommand.cpp`** — every early-return error path in `DrawTriangle` now destroys the SAFEARRAYs allocated so far and frees the `mySegmentName` BSTR.
- **[Medium] `CSampleAddOnInterface.cpp`** — `SubMenuItems` now sets `*pSubMenuIDs = NULL` up front.
- **[Low] `AddOnSupport.cpp`** — `ERROR_INVALID_PARAMETER`/`ERROR_NOT_ENOUGH_MEMORY` → `E_INVALIDARG`/`E_OUTOFMEMORY`.

*Caveat: changes applied to source; not verified by build.*
