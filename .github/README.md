# Alibre Design C++ Add-On Template

A C++ starter project for building tightly integrated Alibre Design add-ons. It implements the COM-based `IAlibreAddOn` interface and ships a working sample (the "DX Triangle" add-on) that you can rename and extend into your own add-on.

## Features
- Implements the full `IAlibreAddOn` COM interface (menu, command, persistence, and licensing entry points).
- DLL entry points wired up for Alibre Design integration: `AddOnLoad`, `AddOnUnload`, `GetAddOnInterface`, and `AddOnInvoke`.
- Add-on descriptor (`ADSampleAddOnDX.adc`) defining the friendly name, menu, icon, target workspace (Part), and unique identifier.
- Root menu ("DX Triangle") with two sample commands, "Post Render" and "Override Render", that draw a textured triangle into the Alibre 3D window via `IAlibreAddOnCommand`.
- Imports the Alibre type libraries (`AlibreX_64.tlb`, `AlibreAddOn_64.tlb`) for direct access to the Alibre automation API.
- Visual Studio solution preconfigured for Debug/Release on Win32 and x64.

## Requirements
- Alibre Design with its add-on API (the `AlibreX_64.tlb` and `AlibreAddOn_64.tlb` type libraries, found under the Alibre Design `Program` folder).
- Visual Studio 2022 (Platform Toolset v143) with the C++ and MFC workloads (the project uses MFC with dynamic linking).
- Target platform: Windows, x64 (Win32 configurations are also present).
- The active rendering code targets Alibre's HOOPS Visualize engine (`IADAddOnCanvasDisplay`) and does not require the DirectX SDK. The `Release|x64` configuration builds without it; the other configurations still reference the legacy DirectX 9 SDK (June 2010) through `$(DXSDK_DIR)` and link `d3d9.lib`/`d3dx9d.lib`.

## Getting Started
1. Open `source/ADSampleAddOnDX.sln` in Visual Studio 2022.
2. In `source/StdAfx.h`, update the two `#import` paths so they point to `AlibreX_64.tlb` and `AlibreAddOn_64.tlb` in your Alibre Design `Program` folder (e.g. `C:\Program Files\Alibre Design 29.0.0.29060\Program`).
3. Select the `Release|x64` (or `Debug|x64`) configuration.
4. Build the solution. The output is `ADSampleAddOnDX.dll`, the loadable add-on.

## Usage
Alibre Design discovers add-ons through their descriptor (`.adc`) file. To load the built add-on:
1. Place `ADSampleAddOnDX.dll`, `ADSampleAddOnDX.adc`, and `ADSampleAddOnDX.ico` together in a folder that Alibre Design scans for add-ons.
2. The descriptor's `loadedWhen="Startup"` setting causes Alibre Design to load the add-on on launch.
3. Start Alibre Design and open a Part workspace (the descriptor targets `Workspace type="Part"`). The add-on's "DX Triangle" menu and its "Post Render" and "Override Render" commands become available.

To create your own add-on, rename the project, DLL, and descriptor, assign a new `Identifier` GUID in the `.adc`, and replace the sample command implementation with your own logic.

## License
See [LICENSE](../LICENSE).
