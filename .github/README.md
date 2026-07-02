# Alibre Design C++ Add-On Template

A C++ starter project for an Alibre Design add-on.

The template builds on the Alibre "DX Triangle" sample. It implements the COM-based `IAlibreAddOn` interface and ships one working add-on (`Sample Addon - DX Triangle`) that draws a textured triangle into the Alibre 3D window. Rename the project, descriptor, and GUID, then replace the sample command with your own logic.

The project targets the Alibre Design 29.0.0.29060 type libraries and builds as a native x64 COM DLL with Visual Studio 2022 (Platform Toolset v143), MFC with dynamic linking, C++. It is not a .NET or IronPython add-on.

## Table Of Contents

- [What Is Here](#what-is-here)
- [Official Alibre Resources](#official-alibre-resources)
- [Requirements](#requirements)
- [Quick Start](#quick-start)
- [Installation](#installation)
- [Usage](#usage)
- [Key Files](#key-files)
- [Key Folders](#key-folders)
- [Notes](#notes)
- [License](#license)

## What Is Here

- A Visual Studio 2022 solution and `.vcxproj` for a native x64 add-on DLL (`ADSampleAddOnDX.dll`).
- A full `IAlibreAddOn` COM implementation (`CSampleAddOnInterface`) covering menu, command, persistence, and licensing entry points.
- DLL entry points wired for Alibre integration: `AddOnLoad`, `AddOnUnload`, `GetAddOnInterface`, `AddOnInvoke`.
- A sample command class (`CDrawTriangleCommand`, an `IAlibreAddOnCommand`) that renders a textured triangle through Alibre's HOOPS Visualize canvas (`IADAddOnCanvasDisplay`).
- A root menu (`DX Triangle`) with two commands, `Post Render` and `Override Render`.
- An add-on descriptor (`ADSampleAddOnDX.adc`) defining the friendly name, menu, icon, Part workspace target, and unique identifier.
- `#import` lines for the Alibre type libraries (`AlibreX_64.tlb`, `AlibreAddOn_64.tlb`) in `StdAfx.h`.
- A test part (`Data/TestPart1.AD_PRT`), a texture image (`wood.jpg`), and the original Alibre build note (`Docs/ReadMe.txt`).

## Official Alibre Resources

Alibre's official resources for API development and AI/LLM/agent workflows: <https://www.alibre.com/api/>

## Requirements

- Alibre Design with its add-on API. `StdAfx.h` imports `AlibreX_64.tlb` and `AlibreAddOn_64.tlb` from the Alibre Design `Program` folder (default paths point at `Alibre Design 29.0.0.29060`).
- Visual Studio 2022 with the C++ and MFC workloads. The project uses Platform Toolset v143 and MFC with dynamic linking.
- Target platform: Windows, x64. Win32 configurations are also present.
- No DirectX SDK is needed for the x64 build. See [Notes](#notes) for the legacy DirectX 9 configurations.

## Quick Start

1. Open `source/ADSampleAddOnDX.sln` in Visual Studio 2022.
2. In `source/StdAfx.h`, set the two `#import` paths to your `AlibreX_64.tlb` and `AlibreAddOn_64.tlb` (for example `C:\Program Files\Alibre Design 29.0.0.29060\Program`).
3. Select the `Release|x64` (or `Debug|x64`) configuration and build. The output is `ADSampleAddOnDX.dll`.

## Installation

Alibre discovers an add-on through its `.adc` descriptor and a registry entry keyed by the add-on's GUID.

1. Copy `ADSampleAddOnDX.dll`, `ADSampleAddOnDX.adc`, `ADSampleAddOnDX.ico`, and `wood.jpg` into one folder.
2. Add a string value under the `Alibre Design Add-Ons` registry key. Name it with the descriptor's identifier `{D05E1217-21A7-4e37-A302-398AA1BDDD7E}` and set its data to the folder from step 1.
3. The descriptor sets `loadedWhen="Startup"`, so Alibre loads the add-on on launch.

Registering under `HKEY_CURRENT_USER\SOFTWARE\Alibre Design Add-Ons` installs the add-on per user and needs no administrator rights; `HKEY_LOCAL_MACHINE` installs it for all users.

To turn this into your own add-on, rename the project, DLL, and descriptor, assign a new `Identifier` GUID in the `.adc`, and replace `CDrawTriangleCommand` with your logic.

## Usage

1. Launch Alibre Design and open a Part workspace, or open the supplied `source/Data/TestPart1.AD_PRT`.
2. On the ribbon, open the `Add-Ons` tab. If the add-on is not listed, enable it in the Add-on Manager (shown there as `Sample Addon - DX Triangle`).
3. From the `DX Triangle` menu, run `Post Render` to draw the triangle on top of Alibre's rendered model, or `Override Render` to draw it on a cleared viewport with Alibre's rendering suppressed.
4. In a running command, press a key to translate the triangle along X; press Escape to end the command.

## Key Files

| File | Purpose |
| --- | --- |
| `source/ADSampleAddOnDX.sln` | Visual Studio 2022 solution. |
| `source/ADSampleAddOnDX.vcxproj` | Project file; v143 toolset, MFC dynamic, Debug/Release on Win32 and x64. |
| `source/ADSampleAddOnDX.adc` | Add-on descriptor: friendly name, menu, icon, Part workspace, identifier GUID. |
| `source/ADSampleAddOnDX.def` | Module definition file (exports are handled in code). |
| `source/ADSampleAddOnDX.cpp` | DLL entry points: `AddOnLoad`, `AddOnUnload`, `GetAddOnInterface`, `AddOnInvoke`. |
| `source/CSampleAddOnInterface.cpp` / `.h` | `IAlibreAddOn` implementation: menus, command dispatch, persistence, licensing. |
| `source/DrawTriangleCommand.cpp` / `.h` | `CDrawTriangleCommand`, the sample `IAlibreAddOnCommand` that draws the triangle. |
| `source/AddOnSupport.cpp` / `.h` | Menu IDs, menu text constants, and SAFEARRAY helpers. |
| `source/StdAfx.h` | Precompiled header; holds the two Alibre `#import` lines to edit. |
| `source/ADSampleAddOnDX.rc` / `Resource.h` | Resource script and resource IDs. |
| `source/ADSampleAddOnDX.ico` | Add-on icon referenced by the descriptor. |
| `source/wood.jpg` | Texture applied to the sample triangle. |
| `source/alibre.disclaimer.txt` | MIT statement plus Alibre trademark and ownership notice. |

## Key Folders

| Folder | Purpose |
| --- | --- |
| `source/` | Add-on source, project files, and resources. |
| `source/Data/` | `TestPart1.AD_PRT`, the sample part used to exercise the add-on. |
| `source/Docs/` | `ReadMe.txt`, Alibre's original build and install note for the DX sample. |
| `source/Res/` | Resource include (`ADSampleAddOnDX.rc2`). |
| `reviews/` | Dated code-review notes for this repository. |
| `.github/` | This README. |

## Notes

- Alibre, LLC provides the underlying sample for demonstration only. This repository's own code is under the MIT License (see `source/alibre.disclaimer.txt`); Alibre and Alibre Design names and materials belong to Alibre, LLC.
- Rendering uses Alibre's HOOPS Visualize engine (`IADAddOnCanvasDisplay`), the default since Alibre Design 2019. The two x64 configurations link no DirectX libraries. Both Win32 configurations still link `d3d9.lib` and `d3dx9d.lib`, and all four configurations add `$(DXSDK_DIR)` to the include and library search paths. The legacy DirectX 9 SDK (June 2010) is relevant only when `Use Legacy Display` is enabled in Alibre's System Options.
- The `IDispatch` methods (`GetIDsOfNames`, `GetTypeInfo`, `Invoke`) return `E_NOTIMPL`. The type-info member (`m_ptinfo`) is left as a placeholder for callers that need it.
- `source/Docs/ReadMe.txt` is Alibre's original note and references older Visual Studio versions and HKLM-only registration. For the current toolset, follow the Quick Start above.

## License

See [LICENSE](../LICENSE).
