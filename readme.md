# b3view
("Be View")
Press 't' for Minetest ../textures with this (poikilos') forked model
viewer for B3D, X, OBJ, MS3D, 3DS (or any supported by Irrlicht).

![screenshot with gull from poikilos mobs_sky fork](https://github.com/poikilos/b3view/raw/master/screenshot.jpg)

bat: [github.com/poikilos/mobs_sky](https://github.com/poikilos/mobs_sky)

Website: [poikilos.org](https://poikilos.org)


## Main Features in poikilos fork
* stabilized (makes sure font, model or texture loads before using;
  makes sure model is loaded before setting View options)
* modernized includes (`#include` statements specify "irrlicht"
  directory instead of assuming it)
* double-click after you associate this program with the file types
  (**mime types** are installed by install.sh to allow this on GNU+Linux
  Systems--see Compile and Install)
* hotkeys to cycle through textures and reload model OR texture
  (see [Usage](#Usage) below).
* see also CHANGELOG.md
* export feature: COLLADA (non-Blender), IRR (Irrlicht Scene settings
  and mesh file paths only), IRRMESH (Static Irrlicht Mesh), OBJ
  (Wavefront), STL (stereolithography)
* Turn off interpolation if loadNextTexture (F3) detects the following
  Minetest-like directory structure and texture naming:
  "<texture directory>/<texture filename based on model name>" where
  "<texture directory>" is either `.` (same directory as model)
  or `../textures` (where model would be in a parallel directory next to
  textures).
* Set the animation loop range (the animation includes the end frame).


## Related Software
- [https://github.com/stujones11/SAM-Viewer](SAM-Viewer): View a
  minetest player model and see the effect of changing various wield
  settings that are available in the minetest Lua API.
- Blitz3d: Blitz3d was released
  [on GitHub](https://github.com/blitz-research/blitz3d) under the
  zlib/libpng license in 2014!
- Blitz3D plug-in for [Ultimate Unwrap
  3D](https://www.unwrap3d.com/u3d/formats.aspx): Ultimate Unwrap 3D is
  a standalone unwrapping tool ("UV Mapping Software").
- Milkshape can export B3D and import x without animations.
- TREEmagik Plus by AlienCodec (the original version is now
 [free](http://www.aliencodec.com/Aliencodec%C2%A9+-+Software+Developers);
 superceded by TREEmagik-G2) can export to b3d.


## B3D Format
B3D in this case is the Blitz3D model format.
- "stores model information in 'chunks;' may contain textures, brushes,
  vertices, triangles, meshes, bones, or animation data."
  -<https://fileinfo.com/extension/b3d>

### What it is not
The B3D format (Blitz3D format) supported by Irrlicht has nothing to do
with other formats which also have the B3D extension.
- It is not [.B3D - Maxon Bodypaint 3D texture
  file](http://http.maxon.net/pub/bp2/docu/bodypaint3d_r2_reference_e.pdf),
  an internal format that Cinema4D uses to store [multi-layer
  textures](https://forums.creativecow.net/docs/forums/post.php?forumid=19&postid=236712&univpostid=236712&pview=t&archive=T).
- It is not ASCII
  - not [.B3D - Ben's 3D Format](https://www.bcchang.com/research/vr/b3d.php)


## Compile
(the original version of this section is from
<https://code.google.com/archive/p/b3view/wikis/BuildingFromSource.wiki>)

### Prerequisites
* C++ Compiler (Linux: gcc [C++14], icc; Windows: the project file is
  from Visual Studio 2010, but C++14 is required--update or see
  [Troubleshooting](#Troubleshooting) and comments for the `#include`
  statements in UserInterface.cpp if you have compiler or linker errors)
* Irrlicht Library
* Linux only: QtCreator/Qt (4 or 5)

### Linux
* To generate a working makefile, edit the .pro file to reflect your
  path settings and run:
```bash
qmake make -f Makefile.Debug
```
This will (hopefully) build a binary in the "build" subdirectory.
See [Troubleshooting](#Troubleshooting) for compiling via GUI.

### Windows
(If you use MinGW the Linux section of instructions apply. This section
only applies to Visual Studio users.)
* In the "win32" folder of your source tree there's a VC++2010 Solution.
  Open it up and edit the properties to reflect the correct path to the
  directory which **contains** the "irrlicht" directory.
  After building, you end up with a "win32_build" folder in the source
  tree which contains the win32 binary.
* Additionally, you have to manually copy the files
  from the "build" directory to the output folder.

### Troubleshooting
(gcc and GUI compilation)
* If you are using GCC to compile, you may need gcc 8.2.1 or higher
  (C++14 `experimental/filesystem` works on 8.2.1 for sure, but C++17's
  `filesystem` has not been tested and requires minor changes to
  `#include` and `using` statements in UserInterface.cpp)
* Make sure you have the Irrlicht and Qt development packages
  (such as via `sudo dnf -y install qt-devel qt-creator irrlicht-devel`
  on Fedora 29)
  - Qt is not actually used, only Qt creator (the `CONFIG -= qt`
    setting is in the `.pro` file).
* Open Qt Creator, click File, Open, then choose this project's .pro
  file.
* Build & Run (if you have trouble compiling, see
  [filehandoff#compiling-issues](https://github.com/poikilos/filehandoff#compiling-issues))
* Copy all of the files from `./build` to your actual build directory,
  if your build directory is not `./build`
* (optional) Copy your favorite public-licensed font over
  `./build/ClearSansRegular.tff` or to current working directory of
  program (varies on Linux when you double-click files). If you don't,
  and didn't copy the included one to the output directory,
  the following fonts will be tried, in the following order:
  * C:\Windows\Fonts: calibrib.ttf, arialbd.ttf
  * /usr/share/fonts: liberation/LiberationSans-Bold.ttf,
    gnu-free/FreeSansBold.ttf, dejavu/DejaVuSans-Bold.ttf,
    google-droid/DroidSans-Bold.ttf


## Install
### Windows
* If you are not using a release version, compile the program (see
  above) then copy install.bat to your build directory.
* double-click install.bat (read any messages that appear then press
  a key to continue when prompted).
* Associate the B3D file extension, and other Irrlicht-compatible 3D
  mesh formats such as X and OBJ.
  `%USERPROFILE%\Applications\b3view\b3view.exe` (right-click a b3d
  file, then Open With, show additional programs, then either paste that
  path or choose This PC, C:, Users, your username, Applications,
  b3view, b3view.exe)

### GNU+Linux Systems
* copy install.sh to your build directory if you are not using a release
  version and your build directory is not `./build`
* run install.sh as root, such by running `sudo bash install.sh` in
  Terminal from the project directory (if you are not root, the install
  will install to $USER/Applications after giving an opportunity to
  cancel with Ctrl C during a countdown).
  (the script will install the "Blitz3D Model File" (`model/b3d`) mime
  type, which your OS will use for all files with the `b3d` extension--
  this allows associating the file type with a program).
* Associate the b3d file extension (and possibly other Irrlicht-
  compatible 3D mesh formats) with b3view: Right-click any B3D file,
  "Open With...", find or type b3view, then check "Remember
  application..." or "Set as default..." or something similar depending
  on your desktop environment (an option to always use the same program
  will only be available if the mime type was successfully installed for
  the user profile or system such as via install.sh).


## Usage
* You can click "File," "Open," then choose an Irrlicht-compatible B3D
  file. However, the program is much easier to use if you associate the
  format with b3view (see "Installation" above) so you can just double-
  click the file to open b3view automatically.
* `left` / `right` (arrow keys): Go frame by frame (Pauses if playing).
* `Ctrl left` / `Ctrl right` (arrow keys): Change animation frame rate
  in increments of 5 fps--click "Faster" or "Slower," or use `-` key or
  `+`/`=` key. By default, the scene refreshes at 60fps and the
  animation runs as 30 fps (Irrlicht does interpolation automatically).
  - Edit the frame rate manually using the input box under "Faster" and
    "Slower."
* `F3` / `Shift F3`: Cycle through textures where the filename contains
  the model filename (or that without underscores) in the current
  directory or `../textures`. If there are no matches, use a list of
  all found textures. The `F3` key goes to the next texture file (hold
  `Shift` and press`F3` to go backward), but does nothing on the first
  press if the model had loaded its own texture.
  - Example: Both automatic loading (when you open a mesh) and manually
    cycling using F3 works for Minetest mods, where the model should be
    in `modname/models/` and the texture should be in
    `modname/textures/` (but occasionally is in the same directory as
    the model).
* `Ctrl i`: toggle texture interpolation (shortcut for View, Texture
  Interpolation)
* `F5`: Reload last model file
* `Shift F5`: Reload last texture file (may not be working due to
  caching, but does try to load different file if texture edit box
  changed).
* drag with middle button: rotate view
* drag with middle button while holding shift key: pan up and down
* View, choose "Up" axis: change camera "up" axis to Z or Y (Y is
  default; automatically changed to Z when 3ds file is loaded)


## Known Issues
* Warn on missing texture.
* Test and complete install.bat on Windows.
* Look for fonts on OS X (see "Set Font for UI Elements" in
  UserInterface.cpp).
* (View.cpp) Set pitch correctly for shift & middle mouse button drag.
* Lighting not correct until you rotate or enable z-Up


## Authors
* ClearSansRegular.ttf (**Apache 2.0 License**) by Intel
  <https://01.org/clear-sans> via
  <https://www.fontsquirrel.com/fonts/clear-sans>
* ninja.b3d, nskin*.jpg by Psionic (psionic3d.co.uk)
  <http://www.psionic3d.co.uk/downloads/ninja.zip>
  (I've seen this file ripped and in several repos, but finally found
  the original site above, which has additional skins not found
  elsewhere. The original site is listed in "ninja animation ranges.txt"
  such as from <https://sledjhamr.org/source/media/Irrlicht/>)
  **"Feel free to use however you like, commercial etc, credits are
  Appreciated..."** -Psionic
* icon (b3view.xcf, p3view.png) Creative Commons Attribution Share-Alike
  4.0 [poikilos](https://poikilos.org)
* All files not mentioned above, and not described in text files in the
  same folder as media (such as "build" folder) are licensed under the
  **GPL v3** as per <https://code.google.com/archive/p/b3view/>
  (see [LICENSE](https://github.com/poikilos/b3view/blob/master/LICENSE)
  file in your favorite text editor).


## Developer Notes
See contributing.md
