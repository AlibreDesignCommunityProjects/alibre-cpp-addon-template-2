 * DISCLAIMER:
 * ALL SOURCE CODE IN THIS PROJECT IS PROVIDED BY ALIBRE, LLC FOR
 * DEMONSTRATION PURPOSES ONLY AND ANY EXPRESSED OR IMPLIED WARRANTIES
 * ARE DISCLAIMED. IN NO EVENT SHALL ALIBRE INC. BE LIABLE FOR ANY 
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE.


**** IMPORTANT NOTE ****

New Rendering Engine in V2019
=============================
Starting from version 2019 of Alibre Design, a new Hoops based rendering engine will become the default 3D rendering 3D engine in Alibre Design. A new api interface has been introduced for tightly integrated addons to render into AlibreDesign window.

How to continue using Legacy DirectX 9 based Rendering Engine
=============================================================='
In order to switch to Legacy Display engine, you will have to check the "Use Legacy Display" checkbox in the 'System Options' dialog (under 'Display'). This will set the DirectX based renderer as the graphics display engine in Alibre Design.
Then, as usual, a tightly integrated addon will be able to render directly using DirectX 9 device interface. For this, we require "DirectX SDK - June 2010" to build and run this addon. 
If you don't have it installed on your machine, download "June 2010" release of DirectX 9.0 SDK  from Microsoft website, and install it.

**** END IMPORTANT NOTE ****



INSTRUCTIONS FOR BUILDING, INSTALLING & RUNNING the Sample Add-on for using DXDevice for rendering.
==================================================================================================

1. Open ADSampleAddOnDX.dsw in Microsoft Visual Studio 2015.

2. This C++ project has already has already set up "Include" and "Lib" directories to  include C:\Program Files\Microsoft DirectX 9.0 SDK (June 2010)\Include and C:\Program Files\Microsoft DirectX 9.0 SDK (June 2010)\Lib respectively.
Update the "Include" and "Lib" directories in VisualStudio if these paths are different on your computer.
NOTE: Above noted DirectX 9.0 SDK is used only when Alibre Design is run with the Legacy Display turned ON. By default this is turned OFF because HOOPs Visualize is the default display engine starting V2019.

3. Open the include file StdAfx.H. Change the line that reads #import "AlibreX_64.TLB" to include the full folder path of AlibreX_64.DLL on your machine (usually C:\Program Files\Alibre Design\Program).

   Similarly, modify the line that reads #include "AlibreAddOn_64.TLB" to include the full folder path (usually same as the above).

4. Save changes by clicking the File->Save menu command.

5. Next, make sure the active configuration is "Debug - x64" or "Release - x64". Select "Build->Rebuild All" command from menu to build this C++ project.
This will create the DLL ADSampleAddOnDX.DLL under "x64\Debug" (or, "x64\Release") folder. This DLL is meant to be loaded in Alibre Deisgn's runtime process (see next step)

6. We are now ready to install the built add-on in its final delivery location:

	a. In Windows, create a new folder, say, "C:\Documents and Settings\All Users\Application Data\Alibre AddOns\ADSampleAddOnDX"

	b. Next copy the following files from the C++ project folder to the new folder you just created (see above step):

		i)  ADSampleAddOnDX.dll -- we created this dll in step 4; it should find it under the .\x64\Release sub-folder.
		ii) ADSampleAddOn.adc -- this is the add-on configuration file; check it out using NotePad.
		iii)ADSampleAddon.ico -- add-on's custom icon used in Alibre's Add-on Manager
		iv) Wood.jpg -- image file used by add-on's command that draws a textured mesh (V2019 and later only)
  
7. Review the ADSampleAddOn.adc file from the above step using a text editor like NotePad. This adc file has been pre-created for this sample. 
Note that each add-on needs a GUID to uniquely identify it. Close the file without making any changes.

8. Using a Windows registry viewer like RegEdit.EXE :

	- navigate to the following registry key:
	  "HKEY_LOCAL_MACHINE\SOFTWARE\Alibre Design Add-Ons"

	- Add a new String Value under this key.

	- Set the Value Name to:  {D05E1217-21A7-4e37-A302-398AA1BDDD7E}

	  Note: This should match entry in ADSampleAddOnDX.adc file.

	- Set the Value Data to the addon's installation folder. Example:
	  C:\Documents and Settings\All Users\Application Data\Alibre AddOns\ADSampleAddOnDX

	- Close Registry

9. To run this Add-on:

	- Launch Alibre Design

	- Open the part file: .\Data\TestPart1.AD_PRT. It is supplied with this AddOn Sample
	  add-on project.

	- On the ribbon interface, click the "Add-Ons" tab. You should see the "DX Triangle" button there.
	  If not, click on "Add-on Manager". You should see the "Sample Addon - DX Triangle" on the Add-on Manager dialog.
	  Make sure it is enabled by selecting the check box next to it.

	- From the "DX Triangle" pull down menu, click on the "Post Render" command.
	  Notice a Triangle drawn on View along with the model. Press any keyboard key to
	  translate the triangle by an offset along X; press Escape key to terminate command.
	  This demonstrates how both Alibre Design and the sample addon can draw their graphics
	  on the graphics window.

	- From the "DX Triangle" pull down menu, click on the "Override Render" command.
	  Notice a Triangle drawn on Cleared View port with overridden rendering.
	  This demonstrates how sample addon can draw its graphics while suppressing Alibre Design's
	  graphics.

10. Browse the source codes to check out the add-on's implementation. The class 'DrawTriangleCommand' implements the add-on's commands

==============================================================================================================================================