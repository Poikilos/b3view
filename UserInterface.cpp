#include "UserInterface.h"
#include <algorithm>
#include <iostream>
#include <string>

// NOTE: to use filesystem, you must also include the fs library such
// as via the `-lstdc++fs` linker option -- see b3view.pro
// #include <filesystem>  // requires C++17
#include <experimental/filesystem> // requires C++14 such as gcc 8.2.1

#include "Debug.h"
#include "Engine.h"
#include "Utility.h"

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

// PRIVATE
void UserInterface::setupUserInterface()
{
    // Menu
    menu = m_Gui->addMenu();
    menu->addItem(L"File", UIE_FILEMENU, true, true);
    menu->addItem(L"View", UIE_VIEWMENU, true, true);

    // File Menu
    fileMenu = menu->getSubMenu(0);
    fileMenu->addItem(L"Open", UIC_FILE_OPEN);
    fileMenu->addItem(L"Change Texture", UIC_FILE_OPEN_TEXTURE);
    fileMenu->addItem(L"Previous Texture    Shift F3", UIC_FILE_PREVIOUS_TEXTURE);
    fileMenu->addItem(L"Next Texture        F3", UIC_FILE_NEXT_TEXTURE);
    fileMenu->addItem(L"Export DAE (non-Blender COLLADA)", UIC_FILE_EXPORT_DAE);
    fileMenu->addItem(L"Export IRR (Irrlicht Scene settings and mesh paths only)", UIC_FILE_EXPORT_IRR);
    fileMenu->addItem(L"Export IRRMESH (Static Irrlicht Mesh)", UIC_FILE_EXPORT_IRRMESH);
    fileMenu->addItem(L"Export OBJ (Wavefront)", UIC_FILE_EXPORT_OBJ);
    fileMenu->addItem(L"Export STL (stereolithography)", UIC_FILE_EXPORT_STL);
    fileMenu->addItem(L"Quit", UIC_FILE_QUIT);

    // View Menu
    viewMenu = menu->getSubMenu(1);
    viewWireframeIdx = viewMenu->addItem(L"Wireframe",
                                         UIC_VIEW_WIREFRAME, true,
                                         false, this->m_WireframeDisplay, true);
    viewLightingIdx = viewMenu->addItem(L"Lighting",
                                        UIC_VIEW_LIGHTING, true,
                                        false, this->m_Lighting, true);
    viewAxisWidgetIdx = viewMenu->addItem(L"Origin Axis Widget",
                                          UIC_VIEW_AXIS_WIDGET, true, false,
                                          true, true);
    viewTargetIdx = viewMenu->addItem(L"Camera Target",
                                      UIC_VIEW_TARGET, true, false,
                                      false, true);

    viewTextureInterpolationIdx = viewMenu->addItem(L"Texture Interpolation  Ctrl i",
                                                    UIC_VIEW_TEXTURE_INTERPOLATION, true, false,
                                                    this->m_TextureInterpolation, true);

    viewYUpIdx = viewMenu->addItem(L"Y Up",
                                   UIC_VIEW_Y_UP, true, false,
                                   true, true);
    viewZUpIdx = viewMenu->addItem(L"Z Up",
                                   UIC_VIEW_Z_UP, true, false,
                                   false, true);
    viewMenu->addItem(L"Slower               Ctrl Left",
                      UIC_VIEW_SLOWER, true, false,
                      false, false);
    viewMenu->addItem(L"Faster               Ctrl Right",
                      UIC_VIEW_FASTER, true, false,
                      false, false);

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
        L"",
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
    // if (QFile(fontPath).exists()) {
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

void UserInterface::handleMenuItemPressed(IGUIContextMenu* menu)
{
    s32 selected = menu->getSelectedItem();
    if (selected > -1) {
        s32 id = menu->getItemCommandId(static_cast<u32>(selected));

        switch (id) {
        case UIC_FILE_OPEN:
            displayLoadFileDialog();
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

        case UIC_VIEW_WIREFRAME:
            m_WireframeDisplay = viewMenu->isItemChecked(viewWireframeIdx);
            m_Engine->setMeshDisplayMode(m_WireframeDisplay, m_Lighting,
                                         m_TextureInterpolation);
            break;

        case UIC_VIEW_LIGHTING:
            m_Lighting = viewMenu->isItemChecked(viewLightingIdx);
            m_Engine->setMeshDisplayMode(m_WireframeDisplay, m_Lighting,
                                         m_TextureInterpolation);
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

        case UIC_VIEW_TEXTURE_INTERPOLATION:
            m_TextureInterpolation = viewMenu->isItemChecked(
                viewTextureInterpolationIdx
            );
            m_Engine->setMeshDisplayMode(m_WireframeDisplay, m_Lighting,
                                         m_TextureInterpolation);
            break;
        }
    }
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
    viewTextureInterpolationIdx = 0;
    viewWireframeIdx = 0;
    viewLightingIdx = 0;
    this->playbackStartStopButton = nullptr;

    m_Engine = engine;
    m_Gui = engine->getGUIEnvironment();

    m_WireframeDisplay = false;
    m_Lighting = true;
    m_TextureInterpolation = true;
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

bool UserInterface::loadNextTexture(int direction)
{
    bool ret = false;
    this->m_Engine->m_NextPath = L"";
    std::wstring basePath = L".";
    if (this->m_Engine->m_PreviousPath.length() > 0) {
        std::wstring lastName = Utility::basename(
            this->m_Engine->m_PreviousPath
        );
        std::wstring lastDirPath = Utility::parentOfPath(
            this->m_Engine->m_PreviousPath
        );
        std::wstring parentPath = Utility::parentOfPath(lastDirPath);
        std::wstring dirSeparator = Utility::delimiter(
            this->m_Engine->m_PreviousPath
        );
        std::wstring texturesPath = parentPath + dirSeparator + L"textures";
        std::wstring tryTexPath = texturesPath + dirSeparator
                                  + Utility::withoutExtension(lastName)
                                  + L".png";
        if (direction == 0 && Utility::isFile(tryTexPath)) {
            this->m_Engine->m_NextPath = tryTexPath;
            this->m_Engine->loadTexture(this->m_Engine->m_NextPath);
        } else {
            tryTexPath = lastDirPath + dirSeparator
                         + Utility::withoutExtension(lastName) + L".png";
            if (direction == 0 && Utility::isFile(tryTexPath)) {
                this->m_Engine->m_NextPath = tryTexPath;
                ret = this->m_Engine->loadTexture(this->m_Engine->m_NextPath);
            } else {
                std::wstring path = texturesPath;

                if (!fs::is_directory(fs::status(path)))
                    path = lastDirPath; // cycle in model's directory instead

                fs::directory_iterator end_itr; // default yields past-the-end

                std::wstring nextPath = L"";
                std::wstring retroPath = L"";
                std::wstring lastPath = L"";

                bool found = false;
                bool force = false;
                wstring tryPath;
                if (fs::is_directory(fs::status(path))) {
                    if (this->m_Engine->m_PrevTexturePath.length() == 0) {
                        if (this->m_Engine->m_PreviousPath.length() > 0) {
                            tryPath = texturesPath + dirSeparator
                                      + Utility::withoutExtension(
                                            Utility::basename(
                                                this->m_Engine->m_PreviousPath
                                            )
                                        )
                                      + L".png";
                            tryPath = Utility::toWstring(Utility::toString(tryPath));
                            if (!Utility::isFile(tryPath)) {
                                tryPath = texturesPath + dirSeparator
                                          + Utility::withoutExtension(
                                                Utility::basename(
                                                    this->m_Engine->m_PreviousPath
                                                )
                                            )
                                          + L".jpg";
                                tryPath = Utility::toWstring(
                                              Utility::toString(tryPath)
                                          );
                                if (Utility::isFile(tryPath)) {
                                    nextPath = tryPath;
                                    found = true;
                                    force = true;
                                }
                            } else {
                                nextPath = tryPath;
                                found = true;
                                force = true;
                            }
                        }
                    }
                    for (const auto& itr : fs::directory_iterator(path)) {
                        std::wstring ext = Utility::extensionOf(
                                               itr.path().wstring()
                                           ); // no dot!
                        if (!is_directory(itr.status())
                            && std::find(m_Engine->textureExtensions.begin(),
                                         m_Engine->textureExtensions.end(), ext)
                               != m_Engine->textureExtensions.end()) {
                            // cycle through files (go to next after
                            // m_PrevTexturePath if any previously loaded,
                            // otherwise first)
                            if (nextPath.length() == 0)
                                nextPath = itr.path().wstring();
                            lastPath = itr.path().wstring();
                            if (found && direction > 0) {
                                if (!force)
                                    nextPath = itr.path().wstring();
                                break;
                            }
                            if (itr.path().wstring()
                                    == this->m_Engine->m_PrevTexturePath)
                                found = true;
                            if (!found)
                                retroPath = itr.path().wstring();
                        }
                    }
                    if (retroPath.length() == 0)
                        retroPath = lastPath; // previous is last if at start
                    if (direction < 0)
                        nextPath = retroPath;
                    if (nextPath.length() > 0) {
                        ret = this->m_Engine->loadTexture(nextPath);
                    }
                }
            }
        }
    } else
        debug() << "Can't cycle texture since no file was opened" << endl;
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
        if (m_Engine->m_PreviousPath.length() > 0) {
            name = Utility::toString(Utility::withoutExtension(Utility::basename(m_Engine->m_PreviousPath)));
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
        switch (ge->Caller->getID()) {
        case UIE_FILEMENU:
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
                    scene::ISceneManager* smgr = m_Engine->m_Device->getSceneManager();
                    result = smgr->loadScene(fileOpenDialog->getFileName());
                }
                else {
                    result = m_Engine->loadMesh(fileOpenDialog->getFileName());
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
                    ///fileOpenDialog->getFileName()
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
                this->m_Engine->axisLength = Utility::toF32(
                    this->axisSizeEditBox->getText()
                );
            }
            break;

        default:
            // break;
            handled = false;
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
                    if (m_Engine->m_PreviousPath.length() > 0) {
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
                    m_TextureInterpolation = m_TextureInterpolation ? false : true;
                    m_Engine->setMeshDisplayMode(m_WireframeDisplay, m_Lighting,
                                                 m_TextureInterpolation);
                    viewMenu->setItemChecked(viewTextureInterpolationIdx,
                                             m_TextureInterpolation);
                }
                else
                    handled = false;
            } else if (event.KeyInput.Key == irr::KEY_RIGHT) {
                if (m_Engine->KeyIsDown[irr::KEY_LCONTROL]
                        || m_Engine->KeyIsDown[irr::KEY_RCONTROL]) {
                    m_Engine->incrementAnimationFPS(5);
                }
                else
                    handled = false;
            } else if (event.KeyInput.Key == irr::KEY_LEFT) {
                if (m_Engine->KeyIsDown[irr::KEY_LCONTROL]
                        || m_Engine->KeyIsDown[irr::KEY_RCONTROL]) {
                    m_Engine->incrementAnimationFPS(-5);
                }
                else
                    handled = false;
            } else if (event.KeyInput.Char == L' ') {
                m_Engine->toggleAnimation();
            } else if (event.KeyInput.Key == irr::KEY_LEFT) {
                if (this->m_Engine->m_LoadedMesh != nullptr) {
                    if (m_Engine->isPlaying)
                        m_Engine->toggleAnimation();
                    this->m_Engine->m_LoadedMesh->setCurrentFrame(
                        round(this->m_Engine->m_LoadedMesh->getFrameNr()) - 1
                    );
                    this->playbackSetFrameEditBox->setText(
                        Utility::toWstring(
                            this->m_Engine->m_LoadedMesh->getFrameNr()
                        ).c_str()
                    );
                }
            } else if (event.KeyInput.Key == irr::KEY_RIGHT) {
                if (this->m_Engine->m_LoadedMesh != nullptr) {
                    if (m_Engine->isPlaying)
                        m_Engine->toggleAnimation();
                    this->m_Engine->m_LoadedMesh->setCurrentFrame(
                        round(this->m_Engine->m_LoadedMesh->getFrameNr()) + 1
                    );
                    this->playbackSetFrameEditBox->setText(
                        Utility::toWstring(
                            this->m_Engine->m_LoadedMesh->getFrameNr()
                        ).c_str()
                    );
                }
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
