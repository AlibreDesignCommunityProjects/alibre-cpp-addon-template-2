# Code Review — alibre-cpp-addon-template-2

- **Date:** 2026-06-15
- **Branch:** `review/2026-06-15-code-review` (branched from `the-tool-store` @ `fc28654` "cleanup prep")
- **Reviewer:** Claude (Opus 4.8)
- **Scope:** Full repository review (C++/MFC tightly-integrated Alibre Design add-on, the `ADSampleAddOnDX` "DX Triangle" sample/template)

---

## 1. Summary

This repo is a C++/MFC sample add-on for Alibre Design, derived from Alibre's stock
`ADSampleAddOnDX` DirectX-rendering sample. It builds a DLL that registers a ribbon menu
("DX Triangle" with "Post Render" / "Override Render" sub-commands) and draws a textured
triangle into Alibre's canvas via the modern Hoops `IADAddOnCanvasDisplay` API. The legacy
DirectX 9 code path has been *commented out* in-place rather than removed, leaving the source
in a half-migrated state.

The COM/menu plumbing (`CSampleAddOnInterface`) and the lifecycle entry points
(`ADSampleAddOnDX.cpp`) are largely intact and follow the Alibre add-on spec. However the repo
has **two unresolved git merge-conflict markers committed into `.gitignore`**, an
**uninitialized `ITypeInfo* m_ptinfo`** that the IDispatch methods dereference, a **header that
references `D3DCOLOR` with every DirectX include commented out** (won't compile), and the demo's
**rendering is effectively dead** because `m_bLegacyRenderer` is hard-set to `VARIANT_TRUE` while
the only draw path runs when it is `VARIANT_FALSE`. There are also several **hard-coded,
version-pinned and machine-specific absolute paths** (TLB import, DirectX SDK, a Windows-XP-era
output directory), stale **D3D9 link dependencies**, and **committed editor artifacts** (`.aps`,
`.bmp`).

**Overall:** A half-migrated vendor sample with real build-breakage and correctness bugs plus
repo hygiene problems. It is unlikely to compile cleanly or render anything as-is. Address the
Critical and High items before using this as a template.

### Findings by severity

| Severity | Count |
|----------|-------|
| Critical | 3 |
| High     | 4 |
| Medium   | 4 |
| Low / Nit| 6 |

---

## 2. Critical

### C-1. Unresolved merge-conflict markers committed into `.gitignore`
**File:** [.gitignore:1](.gitignore), [.gitignore:103](.gitignore), [.gitignore:108](.gitignore)

```
<<<<<<< HEAD
...                (lines 2–102: full ignore list)
=======
Debug/
.vs/
*.pch
*.ipch
>>>>>>> d159e0d061b533bf1787e39e45dc86551d93c3eb
```

A three-way merge was committed without resolving the conflict. Git does not treat
`<<<<<<<`/`=======`/`>>>>>>>` specially in `.gitignore` — they become literal (invalid) patterns,
and the file is essentially broken/garbage from line 1. This is also a strong signal the working
tree was committed mid-merge. Resolve the conflict (keep the richer HEAD block, drop the
duplicate `Debug/`/`*.pch`/`*.ipch` lines that already exist above), and audit the rest of the
tree for the same state — see C-2.

### C-2. `ITypeInfo* m_ptinfo` is never initialized but is dereferenced by IDispatch methods
**File:** [CSampleAddOnInterface.cpp:24-31](CSampleAddOnInterface.cpp), [CSampleAddOnInterface.cpp:294-332](CSampleAddOnInterface.cpp), [CSampleAddOnInterface.h:85](CSampleAddOnInterface.h)

The constructor initializes `m_nRefCount` and the global counter but **never sets `m_ptinfo`**:

```cpp
CSampleAddOnInterface::CSampleAddOnInterface()
{
    initializeMenus();
    m_nRefCount = 0;
    m_nGlobalDrawCounter = 0;
    // m_ptinfo left uninitialized
}
```

Yet `GetTypeInfo`, `GetIDsOfNames`, and `Invoke` all use it:

```cpp
m_ptinfo->AddRef();                                   // GetTypeInfo  (line 304)
return DispGetIDsOfNames(m_ptinfo, ...);              // GetIDsOfNames (line 291)
return DispInvoke(this, m_ptinfo, ...);               // Invoke        (line 328)
```

`m_ptinfo` holds an indeterminate pointer value (it's a raw class member, not zero-initialized),
so any IDispatch call into this object dereferences garbage → access violation crossing the COM
boundary, destabilizing Alibre. (Note the sibling `CDrawTriangleCommand` *recognized* this exact
problem and stubbed its IDispatch methods to `E_NOTIMPL` — see [DrawTriangleCommand.cpp:242-269](DrawTriangleCommand.cpp).)
At minimum initialize `m_ptinfo = NULL` in both constructors and have the IDispatch methods guard
on it (return `E_NOTIMPL`/`E_FAIL` when null), or actually load the type library.

### C-3. `ColorVertex` references `D3DCOLOR` with all DirectX headers removed → does not compile
**File:** [DrawTriangleCommand.h:108-121](DrawTriangleCommand.h)

```cpp
struct ColorVertex
{
    ColorVertex(float x, float y, float z, D3DCOLOR c) { ... }
    float _x, _y, _z;
    D3DCOLOR _color;
    static const DWORD FVF;
};
```

`D3DCOLOR` is declared in `<d3d9types.h>`/`<d3d9.h>`. Every DirectX include was commented out
(`// #include <d3dx9.h>`, `// #include <d3d9.h>` in [DrawTriangleCommand.cpp:8-11](DrawTriangleCommand.cpp);
`//#include <d3dx9.h>` in [AddOnSupport.h:5](AddOnSupport.h); none in [StdAfx.h](StdAfx.h)).
Nothing in the remaining translation units defines `D3DCOLOR`, so `DrawTriangleCommand.h` fails
to compile (`'D3DCOLOR': undeclared identifier`). The `static const DWORD FVF;` is also
*declared* here but its only definition is commented out at [DrawTriangleCommand.cpp:21](DrawTriangleCommand.cpp),
so any use of `ColorVertex::FVF` would be an unresolved external. The struct is entirely unused
elsewhere (`git grep` finds no constructor/field usage) — delete it.

---

## 3. High

### H-1. Hard-coded, version-pinned TLB import path
**File:** [StdAfx.h:49-51](StdAfx.h)

```cpp
#import "C:\Program Files\Alibre Design 28.0.4.28141\Program\AlibreX_64.tlb"
using namespace AlibreX;
#import "C:\Program Files\Alibre Design 28.0.4.28141\Program\AlibreAddOn_64.tlb" raw_interfaces_only
```

This pins the build to one exact Alibre version (`28.0.4.28141`) in a specific install location.
It breaks on any other version/path and on any machine that doesn't have that exact build. The
bundled `Docs/ReadMe.txt` even says the path is *"usually `C:\Program Files\Alibre Design\Program`"*
([Docs/ReadMe.txt:34-36](Docs/ReadMe.txt)) — the committed code disagrees with its own docs. Prefer
the version-less `C:\Program Files\Alibre Design\Program\` path, or drive it from an MSBuild
property / environment variable so the import resolves per machine.

### H-2. Stale D3D9 link dependencies and machine-specific SDK paths in the project
**File:** [ADSampleAddOnDX.vcxproj:79-80](ADSampleAddOnDX.vcxproj), [ADSampleAddOnDX.vcxproj:89-90](ADSampleAddOnDX.vcxproj), [ADSampleAddOnDX.vcxproj:131](ADSampleAddOnDX.vcxproj), [ADSampleAddOnDX.vcxproj:194](ADSampleAddOnDX.vcxproj), [ADSampleAddOnDX.vcxproj:218-220](ADSampleAddOnDX.vcxproj)

The DirectX 9 code is fully commented out, but the project still tries to link against it and
points at developer-specific absolute paths:

```xml
<!-- Release|Win32 and Debug|Win32 -->
<AdditionalDependencies>d3d9.lib;d3dx9d.lib;%(AdditionalDependencies)</AdditionalDependencies>
<!-- Debug|Win32 -->
<AdditionalIncludeDirectories>D:\dev\sdk\DirectX201006\Include\;...</AdditionalIncludeDirectories>
<AdditionalLibraryDirectories>D:\dev\sdk\DirectX201006\Lib\x86;...</AdditionalLibraryDirectories>
<!-- Debug|x64 and Release|x64 -->
<IncludePath>C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include;...</IncludePath>
<LibraryPath>C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x64;...</LibraryPath>
```

On a machine without the June-2010 DirectX SDK (which is the supported/default scenario per the
ReadMe, since Hoops is now the default engine), the Win32 configs fail to link (`cannot open
d3d9.lib`/`d3dx9d.lib`) and the x64 configs add non-existent include/lib search paths. Since the
D3D9 path was removed from the source, drop these `d3d9.lib`/`d3dx9d.lib` dependencies and the
DirectX SDK include/lib paths from all four configurations. Note `d3dx9d.lib` is also the *debug*
import lib being referenced from a *Release* config, which is wrong regardless.

### H-3. Debug|Win32 output path is a hard-coded, non-existent Windows-XP-era directory
**File:** [ADSampleAddOnDX.vcxproj:216](ADSampleAddOnDX.vcxproj)

```xml
<OutputFile>C:\Documents and Settings\All Users\Application Data\Alibre AddOns\ADSampleAddOnDX\ADSampleAddOnDX.dll</OutputFile>
```

`C:\Documents and Settings\...` is the pre-Vista profile layout; on Windows 10 this path doesn't
exist, so the Debug|Win32 link step writes (or fails to write) to a stale location instead of the
normal `$(OutDir)$(TargetName).dll` used by the other three configs (lines 171/260). Make it
consistent: `<OutputFile>$(OutDir)$(TargetName).dll</OutputFile>` and let the install step copy
into the AddOns folder.

### H-4. Committed editor/build artifacts (`.aps`, `.bmp`) — and they're already in `.gitignore`
**Files:** `ADSampleAddOnDX.aps` (tracked, ~20 KB binary), `ADSampleAddOnDX.bmp` (tracked)

`git ls-files` shows `ADSampleAddOnDX.aps` is checked in. An `.aps` is a Visual Studio
*resource-compiler cache* — a regenerated binary that should never be in source control. It is
even matched by `*.aps` in the (broken) `.gitignore` ([.gitignore:38](.gitignore)), so the intent
was clearly to ignore it; it was committed before the rule (or while the ignore file was broken
per C-1). Run `git rm --cached ADSampleAddOnDX.aps` and confirm `.gitignore` (once fixed) covers
it. Review whether `ADSampleAddOnDX.bmp` is a real source resource or a generated artifact.

---

## 4. Medium

### M-1. Demo never renders: `m_bLegacyRenderer` is `TRUE` but the draw path requires `FALSE`
**File:** [DrawTriangleCommand.cpp:114](DrawTriangleCommand.cpp), [DrawTriangleCommand.cpp:126](DrawTriangleCommand.cpp), [DrawTriangleCommand.cpp:171-196](DrawTriangleCommand.cpp), [DrawTriangleCommand.cpp:214](DrawTriangleCommand.cpp)

Both constructors set `m_bLegacyRenderer = VARIANT_TRUE`, and the code that would have queried
the real value from the host is commented out:

```cpp
// m_pCmdSite->LegacyRenderingEngine(&m_bLegacyRenderer); // D3D9 path removed
```

But the only rendering implementation, `On3DRender`, executes its body **only when the flag is
`VARIANT_FALSE`**:

```cpp
HRESULT _stdcall CDrawTriangleCommand::On3DRender(void)
{
    if (m_bLegacyRenderer == VARIANT_FALSE)   // never true → DrawTriangle() never called
    { ... DrawTriangle(pCanvasDisplay, ...); ... }
    return S_OK;
}
```

So with the D3D9 path gone, the Hoops path is gated off by a flag that is hard-wired the wrong
way — clicking "Post Render"/"Override Render" draws nothing. Since the add-on now targets only
the Hoops canvas, set `m_bLegacyRenderer = VARIANT_FALSE` (or remove the flag and the guard
entirely).

### M-2. SAFEARRAY memory leaks on early-return paths in `DrawTriangle`
**File:** [DrawTriangleCommand.cpp:51-71](DrawTriangleCommand.cpp)

Each allocation can `return false` before the cleanup block at lines 90-95, leaking everything
allocated so far (the code even comments this at line 71):

```cpp
if (getSafeArrayFromArray<float>(vertices, 9, VT_R4, &saVertices) != S_OK) return false;
if (getSafeArrayFromArray<float>(normals,  9, VT_R4, &saNormals)  != S_OK) return false;  // leaks saVertices
...
if (getSafeArrayFromArray<float>(vertexUVs,6, VT_R4, &saVertexUVparams) != S_OK)
    return false; // leaks saVertices/saNormals/saIndices/saRGB/saTransform
```

Use a single cleanup path (goto-cleanup or a small RAII wrapper / `CComSafeArray`) so all
allocated arrays are destroyed on every exit. `SafeArrayDestroy(NULL)` is a no-op, so a unified
`cleanup:` label destroying all six handles is safe.

### M-3. `GetAddOnInterface` constructs the object but reference counting is fragile/leaky
**File:** [ADSampleAddOnDX.cpp:94-108](ADSampleAddOnDX.cpp), [CSampleAddOnInterface.cpp:28](CSampleAddOnInterface.cpp)

```cpp
CSampleAddOnInterface* addOnInterface = new CSampleAddOnInterface (); // m_nRefCount starts at 0
if (theApp.m_pAddOnInterface == NULL)
    theApp.m_pAddOnInterface = addOnInterface;   // IUnknownPtr assignment → AddRef (1)
result = (IUnknown *)addOnInterface;             // IUnknownPtr assignment → AddRef (2)
return result;                                    // returns with refcount 2; smart ptr 'result' releases at scope end → 1
```

The object is created with `m_nRefCount = 0` and relies entirely on smart-pointer `AddRef`s. If
`GetAddOnInterface` is ever called a second time, a brand-new object is `new`'d but only assigned
to the local `result` (the `if` skips the member store), so it is returned with a transient
refcount that nets to 1 — meanwhile the comment claims the app object "stores a reference," which
only happens on the first call. The lifecycle works by luck on the first call. Make the
ownership explicit (e.g. `AddRef()` once after construction and `Release()` deterministically, or
keep a single cached instance and return it consistently).

### M-4. `.adc` / docs describe a Part-only, DirectX sample that no longer matches the code
**Files:** [ADSampleAddOnDX.adc:7-9](ADSampleAddOnDX.adc), [Docs/ReadMe.txt:25-41](Docs/ReadMe.txt), [README.md:1-5](README.md)

- The `.adc` advertises `friendlyName="Sample Addon - DX Triangle"` / `Workspace type="Part"` and
  the menu text "DX Triangle" — all carried over from the DirectX sample even though DirectX is
  gone.
- `Docs/ReadMe.txt` is the original Alibre instructions: it tells the user to install the June
  2010 DirectX SDK, open `ADSampleAddOnDX.dsw` (a VC6 workspace that does not exist — the repo
  ships a `.sln`/`.vcxproj`), and references VS2015, contradicting the project's `v143`
  (VS2022) toolset.
- `README.md` is an empty `<details>` stub with no content.

None are compile-blocking, but the documentation actively misleads anyone using this as a
template. Update or remove the stale instructions and write a real README.

---

## 5. Low / Nits

### L-1. `delete` used on an array allocation (`delete` vs `delete[]`)
[CSampleAddOnInterface.cpp:225-232](CSampleAddOnInterface.cpp): `int *pRootMenus = new int[...]` is
freed with `delete pRootMenus;` instead of `delete[] pRootMenus;`. Undefined behavior (in
practice benign for a trivially-destructible `int[]`, but incorrect). Use `delete[]`, or better a
local `std::array<int,nMAIN_MENUS_COUNT>`.

### L-2. `SubMenuItems` ignores the `SafeArrayCopy` HRESULT and can return an uninitialized out-param
[CSampleAddOnInterface.cpp:64-76](CSampleAddOnInterface.cpp): when `menuID != nROOT_MENU_ID` the
function returns `S_OK` without ever writing `*pSubMenuIDs`, and when it does match it ignores the
`SafeArrayCopy` return code. Initialize `*pSubMenuIDs = NULL;` at entry and propagate failures.

### L-3. Copyright/version metadata is stale boilerplate
[ADSampleAddOnDX.rc:88](ADSampleAddOnDX.rc): `LegalCopyright "Copyright (C) 2007"`, FILEVERSION
`1,0,0,1`, empty `CompanyName`. Cosmetic, but worth updating for a shipping/template artifact.

### L-4. Source files left littered with migration TODO comments
The D3D9-removal left a trail of self-notes in shipped code, e.g.
[DrawTriangleCommand.cpp:8-11](DrawTriangleCommand.cpp) ("please check and remove it from there"),
[DrawTriangleCommand.cpp:114](DrawTriangleCommand.cpp)/[126](DrawTriangleCommand.cpp),
[DrawTriangleCommand.cpp:116](DrawTriangleCommand.cpp)/[128](DrawTriangleCommand.cpp),
[DrawTriangleCommand.cpp:242-244](DrawTriangleCommand.cpp). These should be resolved and removed,
not committed as long-lived comments.

### L-5. `.def` file exports nothing useful
[ADSampleAddOnDX.def:6-7](ADSampleAddOnDX.def): the `EXPORTS` section is empty (only a comment).
The add-on entry points (`AddOnLoad`, `GetAddOnInterface`, …) are exported via
`__declspec(dllexport)` from [ADSampleAddOnDX.h:45-72](ADSampleAddOnDX.h), so the `.def` is inert.
Harmless, but either populate it or drop the `ModuleDefinitionFile` reference to avoid confusion.

### L-6. `SLEEP_TIME` / `nTRIANGLE_SIZE` constants are dead
[AddOnSupport.h:16-17](AddOnSupport.h): neither constant is referenced anywhere (`git grep` finds
no use). Remove them.

---

## 6. What looks good

- The COM/menu surface in `CSampleAddOnInterface` correctly implements the `IAlibreAddOn` contract
  (root menu, sub-menu enumeration, menu text/state) and wraps `InvokeCommand` in a try/catch so a
  command-construction failure can't escape across COM.
- `getSafeArrayFromArray<T>` is a clean, explicitly-instantiated template with proper
  access/unaccess pairing and parameter validation ([AddOnSupport.cpp](AddOnSupport.cpp)).
- The Hoops `DrawTriangle` implementation does free its SAFEARRAYs and BSTRs on the success path
  ([DrawTriangleCommand.cpp:77-95](DrawTriangleCommand.cpp)).
- Image path for `wood.jpg` is resolved relative to the DLL's own module location via
  `GetModuleFileNameW`/`PathRemoveFileSpecW`/`PathCombineW`
  ([DrawTriangleCommand.cpp:130-137](DrawTriangleCommand.cpp)) — no hard-coded resource path, which
  is the right pattern.
- `AddOnUnload` releases the cached interface and root smart pointers
  ([ADSampleAddOnDX.cpp:79-91](ADSampleAddOnDX.cpp)).
- The `.vcxproj` carries all four Debug/Release × Win32/x64 configurations on the modern `v143`
  toolset.

---

## 7. Recommended fix order

1. **C-1** — resolve the `.gitignore` merge conflict and verify nothing else was committed
   mid-merge. (blocks clean repo state)
2. **C-3** + **H-2** — delete the dead `ColorVertex`/`D3DCOLOR` struct and strip the D3D9
   lib/include paths so the project actually compiles and links. (blocks build)
3. **C-2** — initialize/guard `m_ptinfo` so IDispatch calls can't crash Alibre. (blocks safe load)
4. **M-1** — flip `m_bLegacyRenderer` to `FALSE` (or remove it) so the demo renders. (blocks the
   one feature)
5. **H-1 / H-3** — unpin the TLB import path and fix the Debug|Win32 output directory.
6. **H-4** — `git rm --cached` the `.aps`/build artifacts.
7. **M-2 / M-3** — tighten SAFEARRAY cleanup and the add-on refcount/ownership.
8. Sweep the **M-4 / L-*** documentation and cleanup nits.
