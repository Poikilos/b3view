# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [git] - 2021-02-17
### Added
- a settings system and related tests
- more string functions in Utility

### Fixed
- Add a missing quote in TestUtility output.


## [git] - 2020-07-30
(Poikilos)
### Added
- Allow setting the animation loop.


## [git] - 2020-03-10
(Poikilos)
### Added
- `replaceAll`
- `TestUtility` ("Utility.cpp" now tests itself, but only one feature
  so far.)
- `getTextureCount` (can examine node by examining all materials, or
  examine material
- Blitz3D format notes
- `OnSelectMesh` (cleans up model-specific variables)
- `getSuffix`
- `getPrefix`
- `startsWithAny`
- `endsWithAny`

### Changed
- Rename some member variables to start with `m_`.
- Detect textures better, and simplify code:
  - Search for substring and substring without underscores within
    potential textures.
  - Detect as soon as model is loaded if the model has no textures.
  - Cache both the full and matching texture lists.
  - If there are any matching textures (named like model), only use that
    list for cycling with F3 (or Shift F3).
  - Combine `m_NextPath` and `m_PreviousPath` into `m_LoadedMeshPath`
    and change usage.
- Update screenshot.
- Reduce line length in some places.
- Improve Changelog formatting.

### Fixed
- Fix crash trying to load a non-mesh after a mesh was loaded
  (See "Manipulating mesh on failed load" section of README.md).

### Removed
- `m_NextPath` and `m_PreviousPath` (replaced by `m_LoadedMeshPath` and
  simplified cycling code)


## [git] - 2019-07-03
(Poikilos)
### Changes
- Move the display mode booleans to Engine.
- Add more string utilities.
- Do fuzzy search against actual texture names if can't find theoretical
  ones.


## [git] - 2019-05-16
(Poikilos)
### Added
- playback menu
  - Move framerate controls to playback menu.
- fix frame-by-frame hotkeys
  - move code to new incrementFrame method


## [git] - 2019-05-16
(Poikilos)
### Changed
- improve minetest texture detection (alternate conventions)
- turn off interpolation if loadNextTexture detects minetest directory
  structure (../textures/<texture filename based on model name>)


## [git] - 2019-05-16
(Poikilos)
### Added
- export COLLADA (non-Blender), IRR, IRRMESH, OBJ, STL
- show dialog box if operation can't be performed
  - improve error reporting in called methods
- add irr mimetype (Irrlicht Scene, mesh file references and settings
  only)
- add irrlicht mimetype (static/non-animated Irrlicht mesh)


## [git] - 2019-04-19
(Poikilos)
### Added
- box for axis length (size of the axis widget)
- box for frame rate
- camera target widget
- option for turning off origin axis widget
- Add menu items for hotkeys, and show hotkey on relevant menu items.

### Changed
- Reorder items on panel.
- Hotkeys are different so they're not triggered when typing in the
  panel.
- Don't reset yaw nor camera distance when panning.
- Show name of loaded model on title bar.
- Fix crash on loading texture before model.
- Fix use of unsigned frame delta for slow and fast options.


## [git] - 2019-04-08
(Poikilos)
### Added
- snapWidgets (move playbackWindow on resize, not leave past edge)

### Changed
- changed enum values to leave room in between, comment unused
- fixed issue in Utility not detecting backslashes correctly
- renamed Utils.* to Utility.* to match class name
- coding style to WebKit (run ./etc/quality.sh to check)
- improve pan - don't reset view
- improve initial camera settings: angle calculation


## [git] - 2019-04-08
(Poikilos)
### Added
- toggle texture interpolation (via checkbox and `x` hotkey)
- INDEX_ variables to store ID of GUI elements
- Text box show name of loaded texture path
### Changed
- check if model is loaded before changing view options (prevents crash)
- unified checkboxes with m_* booleans, by tracking whether box is
  checked via INDEX_ variables for each ID of GUI elements.
- look for ../textures/<model basename>.png & .jpg 1st time pressing `t`
- Use alpha on textures by default
  (see EMT_TRANSPARENT_ALPHA_CHANNEL_REF in Engine.cpp)


## [git] - 2019-03-09
(Poikilos)
### Added
- completed rotation controls (Blender-like)
- pan up and down (Blender-like, but only up and down)
- Z or Y to switch ("up" axis)
- change up axis to Z when 3ds is loaded
- model-ms3d.xml mime type file


## [git] - 2019-03-09
(Poikilos)
### Added
- hotkeys to reload model/texture
- license (see README.md for licensing history)

### Changed
- only try to load png or jpg textures--skip others when cycling
- cycle backwards correctly
- fix some of the header creep (remove unecessary includes in h files)
- improve initial camera position and angle (see top of characters since
  camera is higher; z-forward characters face camera at an angle)
- Clarify relationship between camera start position in m_Engine and
  m_View's rotation (m_Pitch and m_Yaw). Now, `setNewCameraPosition`
  operates on view correctly (relatively) no matter where camera starts.


## [git] - 2019-03-07
(Poikilos)
### Added
- playback controls


## [git] - 2019-03-06
(Poikilos)
### Added
- created install.sh and install.bat, and added Install and Usage
  to README.md
- icon, install scripts, and mime type (`model/b3d`)--see README.md
- mime type (`model/x`)
- added ClearSansRegular.ttf
- hotkeys to cycle ../textures/*

### Changed
- The program can now start without "test.b3d" in the current working
  directory (fixed Segmentation Fault).
- set `TARGET = b3view` in B3View.pro, so that binary is lowercase as
  per usual Linux naming conventions.
- check for font load failure properly, and load properly if succeeds
- check for "ClearSansRegular.ttf" instead of "arial.ttf"
- move `using namespace` directives from `h` files and specify upon use,
  as per C++ best practices; add directives to `cpp` files only as
  needed (removed cumulative namespace creep).


## [git-94e3b8f] - 2019-03-06
(Poikilos)
### Added
- README.md

### Changed
(CGUITTFont methods are in CGUITTFont class unless otherwise specified)
- fixed instances of "0 as null pointer constant" (changed to `nullptr`)
- changed inconsistent use of spaces and tabs (changed tabs to 4 spaces)
- (UserInterface.cpp) fixed "logical not is only applied to the left
  hand side of this comparison..." (put parenthesis around
  `event.EventType == EET_GUI_EVENT`)
- Silently degrade to pixel font if font file cannot be read (fixes
  Segmentation Fault when font file cannot be read).
  - check for nullptr before using:
    - (CGUITTFont.cpp) `tt_face->face` in `getWidthFromCharacter`,
      `getGlyphByChar` (return 0 as bad as per convention:
      existing code already checks for 0--see
      `getWidthFromCharacter`), `getKerningWidth`,
      `draw`, `attach` (also don't copy null by
      reference there--instead, set to nullptr if source is nullptr)
  - check length of array before using
    - (CGUITTFont.cpp) elements of `Glyph` array (type
      `core::array<CGUITTGlyph>`) in `getHeightFromCharacter`
  - (CGUITTFont.cpp) check whether file can be read in
    `CGUITTFace::load` before proceeding

### Removed
- arial.tff removed, since it may be the "real" Arial font, which has a
  proprietary license


## [git-d964384] - 2019-03-06
### Changed
(first poikilos commit, based on https://github.com/egrath)
- changed `#include <irrlicht.h>` to `#include <irrlicht/irrlicht.h>`

### Added
- .gitignore (a [Qt .gitignore](https://github.com/github/gitignore/blob/master/Qt.gitignore))
- CHANGELOG.md
