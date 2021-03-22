#include "Debug.h"
#include "Engine.h"
#include "Utility.h"
#include "UserInterface.h"

#include <algorithm>
#include <iostream>
#include <string>

// NOTE: to use filesystem, you must also include the fs library such
// as via the `-lstdc++fs` linker option -- see b3view.pro
// #include <filesystem>  // requires C++17
#include <experimental/filesystem> // requires C++14 such as gcc 8.2.1


using namespace irr;
using namespace irr::core;
using namespace irr::gui;

using std::string;
using std::wstring;
using namespace std;

// C++14: namespace filesystem = std::experimental::filesystem;
// namespace fs = std::filesystem;  // doesn't work (not a namespace in gcc's C++17)
// using namespace std::filesystem;  // doesn't work (not a namespace in gcc's C++17)
namespace fs = std::experimental::filesystem;
// namespace fs = std::filesystem;  // doesn't work (not a namespace in gcc's C++17)

const u32 UserInterface::UIC_FILE_RECENT_FIRST = UIE_RECENTMENU + 1;

// PRIVATE
void UserInterface::setupUserInterface()
{
    // Menu
    menu = m_Gui->addMenu();
    menu->addItem(L"File", UIE_FILEMENU, true, true);
    menu->addItem(L"Playback", UIE_PLAYBACKMENU, true, true);
    menu->addItem(L"View", UIE_VIEWMENU, true, true);

    // File Menu
    fileMenu = menu->getSubMenu(0);
    fileMenu->addItem(L"Open", UIC_FILE_OPEN);
    this->fileRecentIdx = fileMenu->addItem(L"Open Recent", UIC_FILE_RECENT, true, true);
    std::vector<std::string> recentPaths = this->m_Engine->recentPaths();
    fileMenu->addItem(L"Change Texture", UIC_FILE_OPEN_TEXTURE);
    fileMenu->addItem(L"Previous Texture    Shift F3", UIC_FILE_PREVIOUS_TEXTURE);
    fileMenu->addItem(L"Next Texture        F3", UIC_FILE_NEXT_TEXTURE);
    fileMenu->addItem(L"Export DAE (non-Blender COLLADA)", UIC_FILE_EXPORT_DAE);
    fileMenu->addItem(L"Export IRR (Irrlicht Scene settings and mesh paths only)", UIC_FILE_EXPORT_IRR);
    fileMenu->addItem(L"Export IRRMESH (Static Irrlicht Mesh)", UIC_FILE_EXPORT_IRRMESH);
    fileMenu->addItem(L"Export OBJ (Wavefront)", UIC_FILE_EXPORT_OBJ);
    fileMenu->addItem(L"Export STL (stereolithography)", UIC_FILE_EXPORT_STL);
    fileMenu->addItem(L"Quit", UIC_FILE_QUIT);

    // File, Open Recent submenu
    this->recentMenu = fileMenu->getSubMenu(this->fileRecentIdx);

    this->recentMenu->addItem(L"Clear Recent", UIC_FILE_RECENT_CLEAR);
    this->uic_file_recent_next = UserInterface::UIC_FILE_RECENT_FIRST;
    this->m_file_recent_first_idx = -1;
    this->m_file_recent_last_idx = -1;

    this->recent_initialized = true;
    this->addRecentMenuItems(recentPaths, false);

    // Playback Menu
    playbackMenu = menu->getSubMenu(1);
    playbackMenu->addItem(L"Previous Frame            Left",
                      UIC_PLAYBACK_PREVIOUS, true, false,
                      false, false);
    playbackMenu->addItem(L"Next Frame                Right",
                      UIC_PLAYBACK_NEXT, true, false,
                      false, false);
    playbackMenu->addItem(L"Slower               Ctrl Left",
                      UIC_PLAYBACK_SLOWER, true, false,
                      false, false);
    playbackMenu->addItem(L"Faster               Ctrl Right",
                      UIC_PLAYBACK_FASTER, true, false,
                      false, false);

    // View Menu
    viewMenu = menu->getSubMenu(2);
    viewWireframeIdx = viewMenu->addItem(L"Wireframe",
                                         UIC_VIEW_WIREFRAME, true,
                                         false, this->m_Engine->getEnableWireframe(), true);
    viewLightingIdx = viewMenu->addItem(L"Lighting",
                                        UIC_VIEW_LIGHTING, true,
                                        false, this->m_Engine->getEnableLighting(), true);
    viewAxisWidgetIdx = viewMenu->addItem(L"Origin Axis Widget",
                                          UIC_VIEW_AXIS_WIDGET, true, false,
                                          true, true);
    viewTargetIdx = viewMenu->addItem(L"Camera Target",
                                      UIC_VIEW_TARGET, true, false,
                                      false, true);

    viewTextureInterpolationIdx = viewMenu->addItem(L"Texture Interpolation  Ctrl i",
                                                    UIC_VIEW_TEXTURE_INTERPOLATION, true, false,
                                                    this->m_Engine->getEnableTextureInterpolation(), true);

    viewYUpIdx = viewMenu->addItem(L"Y Up",
                                   UIC_VIEW_Y_UP, true, false,
                                   true, true);
    viewZUpIdx = viewMenu->addItem(L"Z Up",
                                   UIC_VIEW_Z_UP, true, false,
                                   false, true);

    // Playback Control Window
    dimension2d<u32> windowSize = m_Engine->m_Driver->getScreenSize();
    playbackWindow = m_Gui->addWindow(
        rect<s32>(vector2d<s32>(windowSize.Width - 4 - 160, 28),
                  dimension2d<s32>(160, 300)),
        false,
        L"Playback",
        nullptr,
        UIE_PLAYBACKWINDOW
    );
    playbackWindow->getCloseButton()->setVisible(false);
    s32 spacing_x = 4;
    s32 margin_y = 4;
    spacing_y = 4;
    s32 size_x = playbackWindow->getClientRect().getWidth() - 8;
    s32 size_y = 24;
    s32 y = 24;

    playbackStartFrameStaticText = m_Gui->addStaticText(
        L"Start Frame:",
        rect<s32>(vector2d<s32>(spacing_x, y),
                  dimension2d<s32>(size_x, size_y)),
        true,
        true,
        playbackWindow,
        UIE_PLAYBACKSTARTFRAMESTATICTEXT,
        false);
    y += size_y;
    playbackStartFrameEditBox = m_Gui->addEditBox(
        L"",
        rect<s32>(vector2d<s32>(spacing_x, y),
                  dimension2d<s32>(size_x, size_y)),
        true,
        playbackWindow,
        UIE_PLAYBACKSTARTFRAMEEDITBOX);
    y += size_y + spacing_y;
    playbackEndFrameStaticText = m_Gui->addStaticText(
        L"End Frame:",
        rect<s32>(vector2d<s32>(spacing_x, y),
                  dimension2d<s32>(size_x, size_y)),
        true,
        true,
        playbackWindow,
        UIE_PLAYBACKENDFRAMESTATICTEXT,
        false);
    y += size_y;
    playbackEndFrameEditBox = m_Gui->addEditBox(
        L"",
        rect<s32>(vector2d<s32>(spacing_x, y),
                  dimension2d<s32>(size_x, size_y)),
        true,
        playbackWindow,
        UIE_PLAYBACKENDFRAMEEDITBOX);
    y += size_y + spacing_y;

    playbackStartStopButton = m_Gui->addButton(
        rect<s32>(vector2d<s32>(spacing_x, y),
                  dimension2d<s32>(size_x, size_y)),
        playbackWindow,
        UIE_PLAYBACKSTARTSTOPBUTTON,
        L"Start/Stop",
        nullptr);

    y += size_y + spacing_y;
    playbackSetFrameEditBox = m_Gui->addEditBox(
        L"",
        rect<s32>(vector2d<s32>(spacing_x, y),
                  dimension2d<s32>(size_x, size_y)),
        true,
        playbackWindow,
        UIE_PLAYBACKSETFRAMEEDITBOX);
    y += margin_y;

    y += size_y + spacing_y;
    playbackIncreaseButton = m_Gui->addButton(
        rect<s32>(vector2d<s32>(spacing_x, y),
                  dimension2d<s32>(size_x, size_y)),
        playbackWindow,
        UIE_PLAYBACKINCREASEBUTTON,
        L"Faster",
        nullptr);

    y += size_y + spacing_y;
    playbackDecreaseButton = m_Gui->addButton(
        rect<s32>(vector2d<s32>(spacing_x, y),
                  dimension2d<s32>(size_x, size_y)),
        playbackWindow,
        UIE_PLAYBACKDECREASEBUTTON,
        L"Slower",
        nullptr);

    y += size_y + spacing_y;
    playbackFPSEditBox = m_Gui->addEditBox(
        L"",
        rect<s32>(vector2d<s32>(spacing_x, y),
                  dimension2d<s32>(size_x, size_y)),
        true,
        playbackWindow,
        UIE_FPSEDITBOX);
    y += margin_y;

    y += size_y + spacing_y;
    texturePathStaticText = m_Gui->addStaticText(
        L"Texture Path:",
        rect<s32>(vector2d<s32>(spacing_x, y),
                  dimension2d<s32>(size_x, size_y)),
        true,
        true,
        playbackWindow,
        UIE_TEXTUREPATHSTATICTEXT,
        false);

    y += size_y + spacing_y;
    texturePathEditBox = m_Gui->addEditBox(
        L"",
        rect<s32>(vector2d<s32>(spacing_x, y),
                  dimension2d<s32>(size_x, size_y)),
        true,
        playbackWindow,
        UIE_TEXTUREPATHEDITBOX);
    y += margin_y;

    y += size_y + spacing_y;
    axisSizeStaticText = m_Gui->addStaticText(
        L"Axis Size:",
        rect<s32>(vector2d<s32>(spacing_x, y),
                  dimension2d<s32>(size_x, size_y)),
        true,
        true,
        playbackWindow,
        UIE_AXISSIZESTATICTEXT,
        false);

    y += size_y + spacing_y;
    axisSizeEditBox = m_Gui->addEditBox(
        std::to_wstring(this->m_Engine->m_AxisLength).c_str(),
        rect<s32>(vector2d<s32>(spacing_x, y),
                  dimension2d<s32>(size_x, size_y)),
        true,
        playbackWindow,
        UIE_AXISSIZEEDITBOX);
    y += margin_y;

    y += size_y + spacing_y;

    // Set Font for UI Elements
    m_GuiFontFace = new CGUITTFace();
    // irrString defines stringc as string<c8>
    if (!Utility::isFile(m_Engine->m_FontPath)) {
        m_Engine->m_FontPath = L"C:\\Windows\\Fonts\\calibrib.ttf";
    }
    if (!Utility::isFile(m_Engine->m_FontPath)) {
        m_Engine->m_FontPath = L"C:\\Windows\\Fonts\\arialbd.ttf";
    }
    if (!Utility::isFile(m_Engine->m_FontPath)) {
        m_Engine->m_FontPath = L"/usr/share/fonts/liberation/LiberationSans-Bold.ttf";
    }
    if (!Utility::isFile(m_Engine->m_FontPath)) {
        m_Engine->m_FontPath = L"/usr/share/fonts/gnu-free/FreeSansBold.ttf";
    }
    if (!Utility::isFile(m_Engine->m_FontPath)) {
        m_Engine->m_FontPath = L"/usr/share/fonts/dejavu/DejaVuSans-Bold.ttf";
    }
    if (!Utility::isFile(m_Engine->m_FontPath)) {
        m_Engine->m_FontPath = L"/usr/share/fonts/google-droid/DroidSans-Bold.ttf";
    }

    if (m_GuiFontFace->load(m_Engine->m_FontPath.c_str())) { // actually takes `const io::path &`
        m_GuiFont = new CGUITTFont(m_Gui);
        m_GuiFont->attach(m_GuiFontFace, 14);
        m_Gui->getSkin()->setFont(m_GuiFont);
    } else {
        std::wcerr << L"WARNING: Missing '" << m_Engine->m_FontPath << L"'"
                   << endl;
        delete m_GuiFontFace;
        m_GuiFontFace = nullptr;
        if (m_GuiFont != nullptr) {
            std::wcerr << L"WARNING: Keeping old font loaded." << endl;
        }
    }
    // }
}

void UserInterface::displayLoadFileDialog()
{
    m_Gui->addFileOpenDialog(L"Select file to load",
                             true, nullptr, UIE_LOADFILEDIALOG);
}

void UserInterface::displaySaveFileDialog()
{
    m_Gui->addFileOpenDialog(L"Select where to save export.dae",
                             true, nullptr, UIE_SAVEFILEDIALOG);
    // NOTE: if restoreCWD is false (default), cwd changes.
}

void UserInterface::displayLoadTextureDialog()
{
    m_Gui->addFileOpenDialog(L"Select file to load",
                             true, nullptr, UIE_LOADTEXTUREDIALOG);
}

void UserInterface::incrementFrame(f32 frameCount, bool enableRound)
{
    if (this->m_Engine->m_LoadedMesh != nullptr) {
        if (this->m_Engine->m_IsPlaying)
            this->m_Engine->toggleAnimation();
        this->m_Engine->m_LoadedMesh->setCurrentFrame(
            enableRound
            ? (round(this->m_Engine->m_LoadedMesh->getFrameNr()) + frameCount)
            : (round(this->m_Engine->m_LoadedMesh->getFrameNr()) + frameCount)
        );
        this->playbackSetFrameEditBox->setText(
            Utility::toWstring(
                this->m_Engine->m_LoadedMesh->getFrameNr()
            ).c_str()
        );
    }
}

void UserInterface::handleMenuItemPressed(IGUIContextMenu* menu)
{
    s32 selected = menu->getSelectedItem();
    if (selected > -1) {
        s32 id = menu->getItemCommandId(static_cast<u32>(selected));

        switch (id) {
        case UIC_FILE_OPEN:
            displayLoadFileDialog();
            break;

        case UIC_FILE_RECENT_CLEAR:
            clearRecent();
            break;

        case UIC_FILE_EXPORT_DAE:
            exportMeshToHome("dae");
            break;

        case UIC_FILE_EXPORT_IRR:
            exportMeshToHome("irr");
            break;

        case UIC_FILE_EXPORT_IRRMESH:
            exportMeshToHome("irrmesh");
            break;

        case UIC_FILE_EXPORT_OBJ:
            exportMeshToHome("obj");
            break;

        case UIC_FILE_EXPORT_STL:
            exportMeshToHome("stl");
            break;

        case UIC_FILE_OPEN_TEXTURE:
            if (m_Engine->m_LoadedMesh != nullptr) {
                displayLoadTextureDialog();
            }
            else {
                this->m_Engine->m_Device->getGUIEnvironment()->addMessageBox(
                            L"Change Texture", L"You must load a model before a texture.");
            }
            break;

        case UIC_FILE_PREVIOUS_TEXTURE:
            if (m_Engine->m_LoadedMesh != nullptr) {
                loadNextTexture(-1);
            }
            else {
                this->m_Engine->m_Device->getGUIEnvironment()->addMessageBox(
                            L"Change Texture", L"You must load a model before a texture.");
            }
            break;

        case UIC_FILE_NEXT_TEXTURE:
            if (m_Engine->m_LoadedMesh != nullptr) {
                loadNextTexture(1);
            }
            else {
                this->m_Engine->m_Device->getGUIEnvironment()->addMessageBox(
                            L"Change Texture", L"You must load a model before a texture.");
            }
            break;

        case UIC_FILE_QUIT:
            m_Engine->m_RunEngine = false;
            break;

        case UIC_PLAYBACK_PREVIOUS:
            this->incrementFrame(-1.0f, true);
            break;

        case UIC_PLAYBACK_NEXT:
            this->incrementFrame(1.0f, true);
            break;

        case UIC_PLAYBACK_SLOWER:
            //if (ge->EventType == EGET_BUTTON_CLICKED) {
            this->m_Engine->incrementAnimationFPS(-5);
            //}
            break;

        case UIC_PLAYBACK_FASTER:
            //if (ge->EventType == EGET_BUTTON_CLICKED) {
            this->m_Engine->incrementAnimationFPS(5);
            //}
            break;

        case UIC_VIEW_TARGET:
            //
            break;

        case UIC_VIEW_Y_UP:
            m_Engine->setZUp(false);
            viewMenu->setItemChecked(viewZUpIdx, false);
            break;

        case UIC_VIEW_Z_UP:
            m_Engine->setZUp(true);
            viewMenu->setItemChecked(viewYUpIdx, false);
            break;

        case UIC_VIEW_WIREFRAME:
            m_Engine->setEnableWireframe(
                !m_Engine->getEnableWireframe()
            );
            viewMenu->setItemChecked(
                viewWireframeIdx,
                m_Engine->getEnableWireframe()
            );
            break;

        case UIC_VIEW_LIGHTING:
            m_Engine->setEnableLighting(
                !m_Engine->getEnableLighting()
            );
            viewMenu->setItemChecked(
                viewLightingIdx,
                m_Engine->getEnableLighting()
            );
            break;

        case UIC_VIEW_TEXTURE_INTERPOLATION:
            m_Engine->setEnableTextureInterpolation(
                !m_Engine->getEnableTextureInterpolation()
            );
            viewMenu->setItemChecked(
                viewTextureInterpolationIdx,
                m_Engine->getEnableTextureInterpolation()
            );
            break;
        default:
            cerr << "[UserInterface::handleMenuItemPressed] Unknown caller id: " << id << endl;
            break;
        }
    }
}

void UserInterface::updateSettingsDisplay()
{
    viewMenu->setItemChecked(
        viewWireframeIdx,
        m_Engine->getEnableWireframe()
    );
    viewMenu->setItemChecked(
        viewLightingIdx,
        m_Engine->getEnableLighting()
    );
    viewMenu->setItemChecked(
        viewTextureInterpolationIdx,
        m_Engine->getEnableTextureInterpolation()
    );
}

void UserInterface::snapWidgets()
{
    dimension2d<u32> screenSize = m_Engine->m_Driver->getScreenSize();
    rect<s32> newRect;
    // newRect.LowerRightCorner.X = static_cast<s32>(size.Width);
    // newRect.LowerRightCorner.Y = static_cast<s32>(size.Height);
    rect<s32> prevRect = playbackWindow->getRelativePosition();
    newRect.UpperLeftCorner.X = static_cast<s32>(screenSize.Width)
                                - prevRect.getWidth() - spacing_y;
    // debug() << "screen size: " << screenSize.Width << "x" << screenSize.Height;
    // debug() << "  prevRect: "
    //         << prevRect.UpperLeftCorner.X << ","
    //         << prevRect.UpperLeftCorner.Y << ","
    //         << prevRect.LowerRightCorner.X << ","
    //         << prevRect.LowerRightCorner.Y
    //         << " size=(" << prevRect.getWidth() << ","
    //         << prevRect.getHeight() << ")" << endl;
    newRect.UpperLeftCorner.Y = prevRect.UpperLeftCorner.Y;
    newRect.LowerRightCorner.X = newRect.UpperLeftCorner.X
                                 + prevRect.getWidth();
    newRect.LowerRightCorner.Y = newRect.UpperLeftCorner.Y
                                 + prevRect.getHeight();
    playbackWindow->setRelativePosition(newRect);
    m_WindowSize.Width = m_Engine->m_Driver->getScreenSize().Width;
    m_WindowSize.Height = m_Engine->m_Driver->getScreenSize().Height;
}

// PUBLIC
UserInterface::UserInterface(Engine* engine)
{
    this->recent_initialized = false;
    viewTextureInterpolationIdx = 0;
    viewWireframeIdx = 0;
    viewLightingIdx = 0;
    this->playbackStartStopButton = nullptr;

    m_Engine = engine;
    m_Gui = engine->getGUIEnvironment();
    playbackWindow = nullptr;

    setupUserInterface();
}

UserInterface::~UserInterface()
{
    delete m_GuiFont;
    delete m_GuiFontFace;
}

IGUIEnvironment* UserInterface::getGUIEnvironment() const
{
    return m_Gui;
}

void UserInterface::drawStatusLine() const
{
}

bool UserInterface::OnSelectMesh() {
    this->m_MatchingTextures.clear();
    this->m_AllTextures.clear();
    return true;
}

void UserInterface::setPlaybackText(s32 id, const wchar_t* text)
{
    switch (id) {
    case (UIE_PLAYBACKSTARTFRAMEEDITBOX):
        this->playbackStartFrameEditBox->setText(text);
        break;
    case (UIE_PLAYBACKENDFRAMEEDITBOX):
        this->playbackEndFrameEditBox->setText(text);
        break;
    default:
        std::cerr << "ERROR: setPlaybackText got a bad id: " << id << std::endl;
        // break;
    }
}

/**
 * Load the next texture from the list of found textures.
 * Files are only listed once for speed, so you must reload the
 * model to trigger another list ("dir") operation (since loading
 * a mesh calls OnSelectMesh() which clears allTextures and matchingTextures).
 *
 * @param direction Specify <0 to choose previous texture, >0 for next, 0 to
 * reload current texture if any; otherwise, only select a texture if any
 * matching textures (named like model) are present in . or ../textures.
 * @return Any texture was loaded (true/false).
 */
bool UserInterface::loadNextTexture(int direction)
{
    cerr << "Loading texture..." << flush;
    bool ret = false;
    std::wstring basePath = L".";
    if (this->m_Engine->m_LoadedMeshPath.length() > 0) {
        std::wstring prevModelName = Utility::basename(
            this->m_Engine->m_LoadedMeshPath
        );
        wstring foundPath;
        wstring prevModelNoExt;
        prevModelNoExt = Utility::withoutExtension(prevModelName);
        /*
        vector<wstring> names;
        names.push_back(prevModelNoExt+L"_mesh");
        names.push_back(prevModelNoExt);
        names.push_back(prevModelNoExt+L"_1");
        names.push_back(prevModelNoExt+L"_2");
        names.push_back(prevModelNoExt+L"_1");
        names.push_back(prevModelNoExt+L"_2");
        names.push_back(prevModelNoExt+L"_01");
        names.push_back(prevModelNoExt+L"_02");
        names.push_back(prevModelNoExt+L"_01");
        names.push_back(prevModelNoExt+L"_02");
        names.push_back(prevModelNoExt+L"_child");
        names.push_back(prevModelNoExt+L"_female");
        names.push_back(prevModelNoExt+L"_f");
        names.push_back(prevModelNoExt+L"_male");
        names.push_back(prevModelNoExt+L"_m");
        */
        vector<wstring> badSuffixes;
        badSuffixes.push_back(L"_inv");

        std::wstring lastDirPath = Utility::parentOfPath(
            this->m_Engine->m_LoadedMeshPath
        );
        std::wstring parentPath = Utility::parentOfPath(lastDirPath);
        std::wstring dirSeparator = Utility::delimiter(
            this->m_Engine->m_LoadedMeshPath
        );
        std::wstring texturesPath = parentPath + dirSeparator + L"textures";
        std::wstring tryTexPath = texturesPath + dirSeparator + prevModelNoExt
                                  + L".png";
        vector<wstring> texturePaths;

        texturePaths.push_back(lastDirPath);

        if (fs::is_directory(fs::status(texturesPath))) {
            texturePaths.push_back(texturesPath);
        }
        vector<wstring> dotExts;
        for (auto ext : this->m_Engine->m_TextureExtensions) {
            dotExts.push_back(L"." + ext);
        }

        if (this->m_MatchingTextures.size() + this->m_AllTextures.size() < 1) {
            for (auto path : texturePaths) {
                for (const auto& itr : fs::directory_iterator(path)) {
                    if (fs::is_regular_file(itr.status())) {
                        std::wstring name = itr.path().filename().wstring();
                        std::wstring suffix = Utility::getSuffix(name, dotExts,
                                                                 true);
                        bool isUsable = true;
                        std::wstring nameNoExt = Utility::withoutExtension(
                            name
                        );
                        if (Utility::endsWithAny(nameNoExt, badSuffixes, true))
                            isUsable = false;
                        if (isUsable && suffix.length() > 0) {
                            this->m_AllTextures.push_back(
                                path + dirSeparator + name
                            );
                            if (Utility::startsWith(name, prevModelNoExt)) {
                                this->m_MatchingTextures.push_back(
                                    path + dirSeparator + name
                                );
                            }
                            else if (name.find(prevModelNoExt) != std::wstring::npos) {
                                this->m_MatchingTextures.push_back(
                                    path + dirSeparator + name
                                );
                            }
                            else if (name.find(Utility::replaceAll(prevModelNoExt, L"_", L"")) != std::wstring::npos) {
                                this->m_MatchingTextures.push_back(
                                    path + dirSeparator + name
                                );
                            }
                        }
                    }
                }
            }
        }
        vector<wstring> paths = this->m_MatchingTextures;
        if (this->m_MatchingTextures.size() < 1) {
            paths = this->m_AllTextures;
            debug() << "There were no matching textures."
                    << " The entire list of " << this->m_AllTextures.size()
                    << " found textures will be used." << std::endl;
        }
        else {
            // Assume the user wants to view name-matched texture using
            // the render settings of Minetest.
            this->m_Engine->setEnableTextureInterpolation(false);
            viewMenu->setItemChecked(
                viewTextureInterpolationIdx,
                this->m_Engine->getEnableTextureInterpolation()
            );
        }
        std::wstring prevTexture = L"";
        std::wstring nextTexture = L"";
        std::wstring lastTexture = L"";
        std::wstring firstTexture = L"";
        bool found = false;
        for (auto path : paths) {
            if (firstTexture.length() == 0)
                firstTexture = path;
            lastTexture = path;
            if (this->m_Engine->m_LoadedTexturePath.length() > 0) {
                if (path == this->m_Engine->m_LoadedTexturePath) {
                    found = true;
                }
                else if (!found) {
                    prevTexture = path;
                }
                else {
                    if (nextTexture.length() == 0)
                        nextTexture = path;
                }
            }
            else {
                prevTexture = path; // Use the last one as the previous.
                if (nextTexture.length() == 0)
                    nextTexture = path;
            }
        }
        if (nextTexture.length() == 0)
            nextTexture = firstTexture; // The last is current, so next is 1st.
        if (prevTexture.length() == 0) {
            if (lastTexture != firstTexture)
                prevTexture = lastTexture; // Wrap to end.
            else
                prevTexture = firstTexture; // Use the only texture.
        }
        if (lastTexture.length() > 0) {
            if (direction < 0) {
                ret = this->m_Engine->loadTexture(prevTexture);
            }
            else if (direction > 0) {
                ret = this->m_Engine->loadTexture(nextTexture);
            }
            else {
                // If direction is 0 (such as when a model is loaded that has
                // no texture), only load a preloaded or matching texture.
                if (this->m_Engine->m_LoadedTexturePath.length() > 0) {
                    ret = this->m_Engine->loadTexture(
                        this->m_Engine->m_LoadedTexturePath
                    );
                }
                else if (this->m_MatchingTextures.size() >= 1) {
                    ret = this->m_Engine->loadTexture(firstTexture);
                }
            }
        }
        else if (this->m_Engine->m_LoadedTexturePath.length() > 0) {
            ret = this->m_Engine->loadTexture(
                this->m_Engine->m_LoadedTexturePath
            );
        }
    } else
        debug() << "Can't cycle texture since no file was opened" << endl;
    cerr << (ret?"OK":"FAILED") << endl;
    return ret;
}

void UserInterface::exportMeshToHome(std::string extension)
{
    if (this->m_Engine->m_LoadedMesh != nullptr) {
        // this->m_Engine->m_LoadedMesh->getName();
        // displaySaveFileDialog();
        irr::io::path where = irr::io::path();
        if (const char* env_p = std::getenv("HOME")) {
            // std::cout << "Your PATH is: " << env_p << '\n';
            where = irr::io::path(env_p);
            std::cout << "Your PATH is: " << where.c_str() << '\n';
        }
        else if (const char* env_p = std::getenv("USERPROFILE")) {
            // std::cout << "Your PATH is: " << env_p << '\n';
            where = irr::io::path(env_p);
            std::cout << "Your PATH is: " << where.c_str() << '\n';
        }
        std::string name = "";
        if (m_Engine->m_LoadedMeshPath.length() > 0) {
            name = Utility::toString(Utility::withoutExtension(Utility::basename(m_Engine->m_LoadedMeshPath)));
        }

        wstring result = m_Engine->saveMesh(where, name, extension);
        std::wstring caption = L"Export Failed";
        std::wstring msg = L"The format or home variable is unwriteable";
        if (result.length() > 0) {
            caption = L"Export Finished";
            msg = L"Saved " + result;
        }
        std::cout << "Exported as: " << Utility::toString(result) << '\n';
        this->m_Engine->m_Device->getGUIEnvironment()->addMessageBox(
                    caption.c_str(), msg.c_str());
    }
    else {
        this->m_Engine->m_Device->getGUIEnvironment()->addMessageBox(
                    L"Export", L"There is nothing to export.");
    }
}

void UserInterface::clearRecent()
{
    // for (int idx=this->uic_file_recent_next-1; idx>=UserInterface::uic_file_recent_first; idx--) {
    for (std::vector<u32>::iterator idxIt = this->recentIndices.begin(); idxIt != this->recentIndices.end(); ++idxIt) {
        this->recentMenu->removeItem(*idxIt);
    }
    this->recentIndices.clear();
    this->uic_file_recent_next = UserInterface::UIC_FILE_RECENT_FIRST;
    this->m_file_recent_first_idx = -1;
    this->m_file_recent_last_idx = -1;
}

void UserInterface::addRecentMenuItem(std::string path, bool addToEngine)
{
    if (!this->recent_initialized) {
        throw std::runtime_error("The UI is not ready in addRecent.");
    }
    if (!this->hasRecent(path)) {
        wstring path_ws = Utility::toWstring(path);
        if (this->uic_file_recent_next < UserInterface::UIC_FILE_RECENT_FIRST) {
            throw std::runtime_error("this->uic_file_recent_next is "
                                     + std::to_string(this->uic_file_recent_next)
                                     + " but should be equal to or greater than first: "
                                     + std::to_string(this->uic_file_recent_next));
        }
        u32 newI = this->recentMenu->addItem(path_ws.c_str(), this->uic_file_recent_next);
        // IGUIContextMenu* menu = this->recentMenu->getSubMenu(newI);
        this->recentIndices.push_back(newI);
        this->uic_file_recent_next++;
        /*
        if (this->m_file_recent_first_idx < 0) {
            this->m_file_recent_first_idx = menu->getID(); // SIGSEGV crash
        }
        this->m_file_recent_last_idx = menu->getID();
        */
        this->m_Engine->addRecent(path);
    }
}

void UserInterface::addRecentMenuItems(std::vector<std::string> paths, bool addToEngine)
{
    if (!this->recent_initialized) {
        throw std::runtime_error("The UI is not ready in addRecent.");
    }
    for (std::vector<std::string>::iterator it = paths.begin() ; it != paths.end(); ++it) {
        try {
            this->addRecentMenuItem(*it, addToEngine);
        }
        catch (const std::runtime_error& ex) {
            cerr << ex.what() << std::endl;
            break;
        }
    }
}

bool UserInterface::hasRecent(std::string path)
{
    if (!this->recent_initialized) {
        throw std::runtime_error("The UI is not ready in addRecent.");
    }
    for (std::vector<u32>::iterator uiIt = this->recentIndices.begin() ; uiIt != this->recentIndices.end(); ++uiIt) {
        IGUIContextMenu* menu = this->recentMenu->getSubMenu(*uiIt);
        if (menu != nullptr) {
            if (Utility::toString(menu->getText()) == path) {
                return true;
            }
        }
        else {
            std::string uiItMsg = std::to_string(*uiIt);
            // std::string uiItMsg = "<bad uiIt value in recentIndices: ";
            // try {
            //     uiItMsg += std::to_string(*uiIt);
            // }
            // catch (const std::invalid_argument& ex) {
            //     uiItMsg += ex.what();
            // }
            // uiItMsg += ">";
            const std::string msg = "There was no menu for " + uiItMsg + " in hasRecent";
            cerr << msg << endl;
            throw std::runtime_error(msg);
        }
    }
    return false;
}

void UserInterface::openRecent(s32 menuID, std::wstring menuText)
{
    if (!this->recent_initialized) {
        throw std::runtime_error("The UI is not ready in addRecent.");
    }
    IGUIElement* menu = this->recentMenu->getElementFromId(menuID);
    std::string path = Utility::toString(menu->getText());
    cerr << "path: " << path << endl;
    cerr << "menuID: " << menuID << endl;
    cerr << "menuText: " << Utility::toString(menuText) << endl;
}

// IEventReceiver
bool UserInterface::OnEvent(const SEvent& event)
{
    // Events arriving here should be destined for us
    bool handled = false;
    if (event.EventType == EET_USER_EVENT) {
        // debug() << "EET_USER_EVENT..." << endl;
        if (event.UserEvent.UserData1 == UEI_WINDOWSIZECHANGED) {
            if ((m_WindowSize.Width != m_Engine->m_Driver->getScreenSize().Width)
                    || (m_WindowSize.Height != m_Engine->m_Driver->getScreenSize().Height)) {
                snapWidgets();
            }
            handled = true;
        }
    } else if (event.EventType == EET_GUI_EVENT) {
        // debug() << "EET_GUI_EVENT..." << endl;
        handled = true; // set to false below if not handled
        const SEvent::SGUIEvent* ge = &(event.GUIEvent);
        s32 callerID = ge->Caller->getID();
        // TODO: switch (ge->EventType) {
        //     // See http://irrlicht.sourceforge.net/docu/example009.html
        //    case EGET_MENU_ITEM_SELECTED:
        //    case EGET_SCROLL_BAR_CHANGED:
        //    case EGET_COMBO_BOX_CHANGED:
        //    case EGET_BUTTON_CLICKED:
        //        switch(callerID) {
        //            case UIE_PLAYBACKSTARTSTOPBUTTON:
        //        default:
        //            cerr <<  "Unknown button clicked: " << callerID << std::endl;
        //    default:
        //        cerr <<  "Unknown event.GUIEvent.EventType " << ge->EventType << std::endl;
        //        break;
        switch (callerID) {
        case UIE_FILEMENU:
        case UIE_RECENTMENU:
        case UIE_PLAYBACKMENU:
        case UIE_VIEWMENU:
            // call handler for all menu related actions
            handleMenuItemPressed(static_cast<IGUIContextMenu*>(ge->Caller));
            break;
        case UIE_LOADFILEDIALOG:
            if (ge->EventType == EGET_FILE_SELECTED) {
                IGUIFileOpenDialog* fileOpenDialog = static_cast<IGUIFileOpenDialog*>(ge->Caller);
                wstring path = fileOpenDialog->getFileName();
                bool result = false;
                wstring extension = Utility::extensionOf(path);
                if (Utility::toLower(Utility::toString(extension)) == "irr") {
                    result = m_Engine->loadScene(fileOpenDialog->getFileName());
                }
                else {
                    result = m_Engine->loadMesh(fileOpenDialog->getFileName());
                }
                if (result) {
                    try {

                        this->addRecentMenuItem(Utility::toString(path), true);
                    }
                    catch (const std::runtime_error& ex) {
                        cerr << ex.what() << std::endl;
                        break;
                    }
                }
                if (!result) {
                    this->m_Engine->m_Device->getGUIEnvironment()->addMessageBox(
                                L"Load Mesh", L"The model is inaccessible or not in a compatible format.");
                }
            }
            break;
        case UIE_SAVEFILEDIALOG:
            if (ge->EventType == EGET_FILE_SELECTED) {
                if (m_Engine->m_LoadedMesh != nullptr) {
                    IGUIFileOpenDialog* fileOpenDialog = static_cast<IGUIFileOpenDialog*>(ge->Caller);
                    // fileOpenDialog->getFileName()
                    m_Engine->saveMesh(fileOpenDialog->getDirectoryName(), "", "dae");
                }
                else {
                    this->m_Engine->m_Device->getGUIEnvironment()->addMessageBox(
                                L"Export", L"There is nothing to save.");
                }
            }
            break;

        case UIE_LOADTEXTUREDIALOG:
            if (ge->EventType == EGET_FILE_SELECTED) {
                IGUIFileOpenDialog* fileOpenDialog = static_cast<IGUIFileOpenDialog*>(ge->Caller);
                m_Engine->loadTexture(fileOpenDialog->getFileName());
            }
            break;

        case UIE_PLAYBACKSTARTSTOPBUTTON:
            if (ge->EventType == EGET_BUTTON_CLICKED) {
                this->m_Engine->toggleAnimation();
            }
            break;

        case UIE_PLAYBACKINCREASEBUTTON:
            if (ge->EventType == EGET_BUTTON_CLICKED) {
                this->m_Engine->incrementAnimationFPS(5);
            }
            break;

        case UIE_PLAYBACKDECREASEBUTTON:
            if (ge->EventType == EGET_BUTTON_CLICKED) {
                this->m_Engine->incrementAnimationFPS(-5);
            }
            break;

        case UIE_PLAYBACKSETFRAMEEDITBOX:
            if (ge->EventType == EGET_EDITBOX_ENTER) {
                if (this->m_Engine->m_LoadedMesh != nullptr) {
                    this->m_Engine->m_LoadedMesh->setCurrentFrame(
                        Utility::toF32(this->playbackSetFrameEditBox->getText())
                    );
                }
            }
            break;
        case UIE_PLAYBACKSTARTFRAMEEDITBOX:
            if (ge->EventType == EGET_EDITBOX_ENTER) {
                if (this->m_Engine->m_LoadedMesh != nullptr) {
                    this->m_Engine->m_LoadedMesh->setFrameLoop(
                                Utility::toF32(this->playbackStartFrameEditBox->getText()),
                                Utility::toF32(this->playbackEndFrameEditBox->getText())
                    );
                }
            }
            break;
        case UIE_PLAYBACKENDFRAMEEDITBOX:
            if (ge->EventType == EGET_EDITBOX_ENTER) {
                if (this->m_Engine->m_LoadedMesh != nullptr) {
                    this->m_Engine->m_LoadedMesh->setFrameLoop(
                                Utility::toF32(this->playbackStartFrameEditBox->getText()),
                                Utility::toF32(this->playbackEndFrameEditBox->getText())
                    );
                }
            }
            break;
        case UIE_TEXTUREPATHEDITBOX:
            if (ge->EventType == EGET_EDITBOX_ENTER) {
                if (this->m_Engine->m_LoadedMesh != nullptr) {
                    this->m_Engine->loadTexture(texturePathEditBox->getText());
                }
            }
            break;
        case UIE_FPSEDITBOX:
            if (ge->EventType == EGET_EDITBOX_ENTER) {
                if (this->m_Engine->m_LoadedMesh != nullptr) {
                    this->m_Engine->m_LoadedMesh->setAnimationSpeed(
                        Utility::toF32(this->playbackFPSEditBox->getText())
                    );
                }
            }
            break;
        case UIE_AXISSIZEEDITBOX:
            if (ge->EventType == EGET_EDITBOX_ENTER) {
                this->m_Engine->m_AxisLength = Utility::toF32(
                    this->axisSizeEditBox->getText()
                );
            }
            break;
        case -1:
            break;
        default:
            // if ((ge->Caller->getID() >= this->m_file_recent_first_idx)
            //         && (ge->Caller->getID() <= m_file_recent_last_idx)) {
            if (std::find(this->recentIndices.begin(), this->recentIndices.end(), ge->Caller->getID()) != this->recentIndices.end()) {
                cerr << "Recent item id: " << callerID << endl;
                this->openRecent(callerID, ge->Caller->getText());
            }
            else {
                cerr << "Unknown caller id: " << callerID << " Text:" << ge->Caller->getText() << endl;

                handled = false;
            }
        }
    } else if (event.EventType == EET_KEY_INPUT_EVENT) {
        // debug() << "EET_KEY_INPUT_EVENT..." << endl;
        handled = true; // set to false below if not handled
        if (event.KeyInput.PressedDown
                && !m_Engine->KeyIsDown[event.KeyInput.Key]) {
            if (event.KeyInput.Key == irr::KEY_F5) {
                if (m_Engine->KeyIsDown[irr::KEY_LSHIFT]
                        || m_Engine->KeyIsDown[irr::KEY_RSHIFT]) {
                    m_Engine->reloadTexture();
                }
                else {
                    if (m_Engine->m_LoadedMeshPath.length() > 0) {
                        bool result = m_Engine->reloadMesh();
                        if (!result) {
                            this->m_Engine->m_Device->getGUIEnvironment()->addMessageBox(
                                        L"Reload Mesh", L"The model is inaccessible or not in a compatible format.");
                        }
                    }
                    else {
                        debug() << "  - No mesh is loaded." << endl;
                    }
                }
            } else if (event.KeyInput.Key == irr::KEY_F3) {
                if (m_Engine->KeyIsDown[irr::KEY_LSHIFT]
                        || m_Engine->KeyIsDown[irr::KEY_RSHIFT]) {
                    loadNextTexture(-1);
                    debug() << "  - back" << endl;
                }
                else
                    loadNextTexture(1);
            } else if (event.KeyInput.Key == irr::KEY_KEY_I) {
                if (m_Engine->KeyIsDown[irr::KEY_LCONTROL]
                        || m_Engine->KeyIsDown[irr::KEY_RCONTROL]) {
                    // IGUIContextMenu* textureInterpolationElement = (
                    //     dynamic_cast<IGUIContextMenu*>(
                    //         viewMenu->getElementFromId(
                    //             UIC_VIEW_TEXTURE_INTERPOLATION
                    //         )
                    //     );
                    // )

                    m_Engine->setEnableTextureInterpolation(
                        !m_Engine->getEnableTextureInterpolation()
                    );
                    viewMenu->setItemChecked(
                        viewTextureInterpolationIdx,
                        m_Engine->getEnableTextureInterpolation()
                    );
                }
                else
                    handled = false;
            } else if (event.KeyInput.Key == irr::KEY_RIGHT) {
                if (m_Engine->KeyIsDown[irr::KEY_LCONTROL]
                        || m_Engine->KeyIsDown[irr::KEY_RCONTROL]) {
                    m_Engine->incrementAnimationFPS(5);
                }
                else {
                    incrementFrame(1.0f, true);
                }
            } else if (event.KeyInput.Key == irr::KEY_LEFT) {
                if (m_Engine->KeyIsDown[irr::KEY_LCONTROL]
                        || m_Engine->KeyIsDown[irr::KEY_RCONTROL]) {
                    m_Engine->incrementAnimationFPS(-5);
                }
                else {
                    incrementFrame(-1.0f, true);
                }
            } else if (event.KeyInput.Char == L' ') {
                m_Engine->toggleAnimation();
            }
            else
                handled = false;
            // std::wcerr << "Char: " << event.KeyInput.Char << endl;
        }
        m_Engine->KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
        return true;
    } else if (event.EventType == EET_MOUSE_INPUT_EVENT) {
        // debug() << "EET_MOUSE_INPUT_EVENT..." << endl;
        handled = true; // set to false below if not handled
        // TODO: improve this copypasta (or elsewhere use states 1 and 3 as
        // events, and add 1 as "handled" (or set back to 0 for no drag feature)
        // as intended for drag or long press handling).
        switch (event.MouseInput.Event) {
        case EMIE_LMOUSE_LEFT_UP:
            if (m_Engine->LMouseState == 2) {
                m_Engine->LMouseState = 3;
            }
            break;

        case EMIE_LMOUSE_PRESSED_DOWN:
            if (m_Engine->LMouseState == 0) {
                m_Engine->LMouseState = 1;
            }
            break;

        case EMIE_RMOUSE_LEFT_UP:
            if (m_Engine->RMouseState == 2) {
                m_Engine->RMouseState = 3;
            }
            break;

        case EMIE_RMOUSE_PRESSED_DOWN:
            if (m_Engine->RMouseState == 0) {
                m_Engine->RMouseState = 1;
            }
            break;
        default:
            handled = false;
        }
    }
    return handled;
}
