# foo-soni-collecti
foobar2000 component for offline playback statistics logging. <br/>
Essentially a local scrobbler, where the playback stats are saved locally on your machine.

## Installation
Install through the *.fb2k-component* file, in the **packed_component** directory.<br/>
Supports foobar2000 >=v2.0, x64 and x86.

## Usage
When the playback time of the currently playing song reaches the chosen 
threshold percentage (default 50%), the listening and track stats will be 
written to the file (default *soni_collecti.csv*) in the chosen output directory (default C:\Users\\).

Backups of the file can be created using the **Backup Data File** button.
This will create a *backups* directory in the *Statistics File Directory*.

### Settings
* Enable playback statistic logging: Turns logging on or off.
* Threshold (%): Determines when the playing song will be logged into the data file.
* Statistics File Directory: Location of the .csv file where the playback info will be logged.


## Building / Editing
Rough steps to build the solution in MSVS:
1. Need installs (Visual Studio Installer)
	* MFC
	* ATL
2. Open project solution .sln in MSVS
	* Retarget solution / update
3. Set references for foo_soni_collecti project to dependent projects
	* foobar2000_SDK
	* foobar2000_sdk_helpers
	* libPPUI
	* pfc
4. Include **lib\WTL** directory in C/C++ includes for projects
	* foo_soni_collecti
	* foobar2000_SDK
	* foobar2000_sdk_helpers
	* libPPUI
	* pfc
5. Compile *stdafx.cpp* in projects
	* foobar2000_SDK
	* foobar2000_sdk_helpers
	* libPPUI
	* pfc
6. Build Solution

### Dependencies
Placed in the **lib** directory:
* [foobar2000 SDK](https://www.foobar2000.org/SDK)
* [Windows Template Library](https://sourceforge.net/projects/wtl/files/)

## References / Resources
Guides and references for developing and troubleshooting components are not very straightforward, so I compiled some of the ones I used here.

### Example Project & Development Tutorials
* [Development Overview](https://wiki.hydrogenaud.io/index.php?title=Foobar2000:Development:Overview)
* [foobar2000 development tutorial](https://yirkha.fud.cz/tmp/496351ef.tutorial-draft.html)
* [Example Component Source Code](https://github.com/Chocobo1/fb2k_example)
* [General Component Tutorial (foo_tutorial1) 0.4](https://foosion.foobar2000.org/components/?id=tutorial1&version=0.4)
 
### SDK Reference & foobar2000 Usage
* [SDK Docs](https://chocobo1.github.io/fb2k_doc/SDK-2015-08-03/doxygen/index.html)
* [Title Formatting Reference](https://wiki.hydrogenaud.io/index.php?title=Foobar2000:Title_Formatting_Reference)

### Hydrogenaudio Forums
* [New SDK with foobar2000 v2.0 features released (2022-08-10)](https://hydrogenaud.io/index.php/topic,122761.25.html)
	* \$(VC_LibraryPath_x64);\$(WindowsSDK_LibraryPath_x64); to solve unresolved externals for x64 
* [Next track and recurring callbacks](https://hydrogenaud.io/index.php/topic,65973.0.html)
* [Updated foobar2000 development documentation](https://hydrogenaud.io/index.php/topic,122602.0.html)
* [on_playback_pause not triggering at all](https://hydrogenaud.io/index.php/topic,124217.0.html)

### MSVS
* [Cannot open resource file in VS 2015: Can't open include file afxres.h](https://stackoverflow.com/questions/35436654/cannot-open-resource-file-in-vs-2015-cant-open-include-file-afxres-h)
* [Error C1047: Object file created with an older compiler than other objects](https://stackoverflow.com/questions/837073/error-c1047-object-file-created-with-an-older-compiler-than-other-objects)
* [Linker Tools Error LNK2001](https://learn.microsoft.com/en-us/cpp/error-messages/tool-errors/linker-tools-error-lnk2001?view=msvc-170)

