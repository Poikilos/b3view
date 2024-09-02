#include <string>
#include <filesystem>
// See https://stackoverflow.com/questions/22201663/find-and-move-files-in-c

#include "Engine.h"

#include "UserInterface.h"
#include "Utility.h"
#include "View.h"
#include <cerrno>
// _chdir (not chdir--see):
#include <unistd.h>

using std::cout;
using std::endl;
using std::wcerr;
using std::wstring;
using std::wstringstream;

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;
using namespace irr::gui;

/* //////////////////////////////////////////////////////////////////////////
   PRIVATE METHODS
   /////////////////////////////////////////////////////////////////////// */

bool Engine::getEnableWireframe() const
{
    return m_EnableWireframe;
}

bool Engine::getEnableLighting() const
{
    return m_EnableLighting;
}

bool Engine::getEnableTextureInterpolation() const
{
    return m_EnableTextureInterpolation;
}

void Engine::setEnableWireframe(bool EnableWireframe)
{
    if (this->m_EnableWireframe != EnableWireframe) {
        this->setMeshDisplayMode(EnableWireframe, this->m_EnableLighting,
                                 this->m_EnableTextureInterpolation);
    }
}

void Engine::setEnableLighting(bool EnableLighting)
{
    if (this->m_EnableLighting != EnableLighting) {
        this->setMeshDisplayMode(this->m_EnableWireframe, EnableLighting,
                                 this->m_EnableTextureInterpolation);
    }
}

void Engine::setEnableTextureInterpolation(bool EnableTextureInterpolation)
{
    if (this->m_EnableTextureInterpolation != EnableTextureInterpolation)
        debug() << "setEnableTextureInterpolation(..., setEnableTextureInterpolation:"
                << (EnableTextureInterpolation?"true":"false") << ")" << endl;

    if (this->m_EnableTextureInterpolation != EnableTextureInterpolation) {
        this->setMeshDisplayMode(this->m_EnableWireframe, this->m_EnableLighting,
                                 EnableTextureInterpolation);
    }
}

void Engine::addRecent(std::string path)
{
    if (!this->hasRecent(path)) {
        int count = this->countRecent();
        std::string name = "recent" + std::to_string(count);
        this->settings.set(name, path);
    }
}

void Engine::addRecentPaths(std::vector<std::string> paths)
{
    for (std::vector<std::string>::iterator it = paths.begin(); it != paths.end(); ++it) {
        this->addRecent(*it);
    }
}

int Engine::countRecent()
{
    int count = 0;
    while (this->settings.exists("recent" + std::to_string(count))) {
        count++;
    }
    return count;
}

bool Engine::hasRecent(std::string path) {
    int count = 0;
    while (true) {
        bool found = false;
        std::string value = this->settings.get("recent" + std::to_string(count), found);
        if (found) {
            if (value == path) {
                break;
            }
            count++;
        }
        else {
            break;
        }
    }
    return false;
}

std::vector<std::string> Engine::recentPaths()
{
    std::vector<std::string> results;
    int count = 0;
    while (true) {
        bool found;
        std::string value = this->settings.get("recent" + std::to_string(count), found);
        if (found) {
            results.push_back(value);
            count++;
        }
        else {
            break;
        }
    }
    return results;
}

void Engine::setupScene()
{
    // Setup Light
    m_SceneLight = m_Scene->addLightSceneNode();
    m_SceneLight->setID(SIID_LIGHT);
    m_SceneLight->setLightType(ELT_DIRECTIONAL);
    m_SceneLight->getLightData().AmbientColor = SColorf(0.2f, 0.2f, 0.2f);
    m_SceneLight->getLightData().DiffuseColor = SColorf(0.8f, 0.8f, 0.8f);
    m_Scene->setAmbientLight(SColorf(0.2f, 0.2f, 0.2f));

    // Setup Camera
    // (so z-forward characters face camera partially
    // (formerly vector3df(0, 0, -10), vector3df())
    m_CamPos = vector3df(4.5, 3.5, 9);
    m_CamTarget = vector3df(0, 3, 0);
    // Below will be overridden by View m_Yaw and m_Pitch--see "calculate m_Yaw"
    // further down.
    ICameraSceneNode* camera = m_Scene->addCameraSceneNode(nullptr, m_CamPos,
                                                           m_CamTarget);
    camera->setAspectRatio(
        static_cast<f32>(m_Driver->getScreenSize().Width)
        / static_cast<f32>(m_Driver->getScreenSize().Height)
    );
}

IGUIEnvironment* Engine::getGUIEnvironment() const
{
    return m_Device->getGUIEnvironment();
}

void Engine::drawAxisLines()
{
    SMaterial xMaterial;
    xMaterial.Lighting = false;
    xMaterial.EmissiveColor = SColor(255, 255, 0, 0);
    xMaterial.Thickness = 1.0f;

    SMaterial yMaterial(xMaterial);
    yMaterial.EmissiveColor = SColor(255, 0, 255, 0);

    SMaterial zMaterial(xMaterial);
    zMaterial.EmissiveColor = SColor(255, 0, 0, 255);

    SMaterial descenderMaterialVert(xMaterial);
    descenderMaterialVert.EmissiveColor = SColor(128, 100, 140, 190); // ARGB
    SMaterial descenderMaterialHorz(xMaterial);
    descenderMaterialHorz.EmissiveColor = SColor(255, 255, 255, 255);

    vector3df descend3df(0, 0, 0);
    // vector3df target = m_View->c
    bool enableAxisWidget = true;
    m_Driver->setTransform(ETS_WORLD, matrix4());
    if (m_View != nullptr) {
        if (this->m_UserInterface->viewMenu->isItemChecked(
                    this->m_UserInterface->viewTargetIdx)) {
            if (m_View->zUp()) {
                descend3df.Z = this->m_CamTarget.Z;
            }
            else {
                descend3df.Y = this->m_CamTarget.Y;
            }
            vector3df descendSideways3df(descend3df);
            descendSideways3df.X = this->m_CamTarget.X;
            vector3df descendSidewaysForward3df(descendSideways3df);
            if (m_View->zUp()) {
                descendSidewaysForward3df.Y = this->m_CamTarget.Y;
            }
            else {
                descendSidewaysForward3df.Z = this->m_CamTarget.Z;
            }
            m_Driver->setMaterial(descenderMaterialVert);
            m_Driver->draw3DLine(vector3df(), descend3df,
                                 descenderMaterialVert.EmissiveColor);
            m_Driver->setMaterial(descenderMaterialHorz);
            m_Driver->draw3DLine(descend3df, descendSideways3df,
                                 descenderMaterialHorz.EmissiveColor);
            m_Driver->draw3DLine(descendSideways3df, descendSidewaysForward3df,
                                 descenderMaterialHorz.EmissiveColor);
            f32 arrowDirection = 1.0f;
            vector3df arrowLeft3df(descendSidewaysForward3df);
            vector3df arrowRight3df(descendSidewaysForward3df);
            f32 arrowSize = this->m_View->cameraDistance() / 30;
            if (m_View->zUp()) {
                if (descendSidewaysForward3df.Y > descendSideways3df.Y)
                    arrowDirection = -1.0f;
                arrowLeft3df.X += arrowSize;
                arrowLeft3df.Y += arrowSize * arrowDirection;
                arrowRight3df.X -= arrowSize;
                arrowRight3df.Y += arrowSize * arrowDirection;
            }
            else {
                if (descendSidewaysForward3df.Z > descendSideways3df.Z)
                    arrowDirection = -1.0f;
                arrowLeft3df.X += arrowSize;
                arrowLeft3df.Z += arrowSize * arrowDirection;
                arrowRight3df.X -= arrowSize;
                arrowRight3df.Z += arrowSize * arrowDirection;
            }
            m_Driver->draw3DLine(descendSidewaysForward3df, arrowLeft3df,
                                 descenderMaterialHorz.EmissiveColor);
            m_Driver->draw3DLine(descendSidewaysForward3df, arrowRight3df,
                                 descenderMaterialHorz.EmissiveColor);
            // position2d<s32> targetPos2d = m_Scene->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(
            //     this->m_CamTarget
            // );
            // dimension2d<u32> textSize;
            // if (m_AxisFont != nullptr) {
            //     textSize = m_AxisFont->getDimension(L"target");
            //     m_AxisFont->draw(L"target", rect<s32>(targetPos2d, textSize),
            //                      descenderMaterial.EmissiveColor, true,
            //                      true);
            // }
        }
        enableAxisWidget = this->m_UserInterface->viewMenu->isItemChecked(
            this->m_UserInterface->viewAxisWidgetIdx
        );
    }


    if (enableAxisWidget) {
        m_Driver->setMaterial(xMaterial);
        m_Driver->draw3DLine(vector3df(), vector3df(m_AxisLength, 0, 0),
                             SColor(255, 255, 0, 0));
        position2d<s32> textPos = m_Scene->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(
            vector3df(m_AxisLength + m_AxisLength*.1f, 0, 0)
        );
        dimension2d<u32> textSize;
        if (m_AxisFont != nullptr) {
            textSize = m_AxisFont->getDimension(L"X+");
            m_AxisFont->draw(L"X+", rect<s32>(textPos, textSize),
                             SColor(255, 255, 0, 0), true, true);
        }

        m_Driver->setMaterial(yMaterial);
        m_Driver->draw3DLine(vector3df(), vector3df(0, m_AxisLength, 0),
                             SColor(255, 0, 255, 0));
        textPos = m_Scene->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(
            vector3df(0, m_AxisLength + m_AxisLength*.1f, 0)
        );
        if (m_AxisFont != nullptr) {
            textSize = m_AxisFont->getDimension(L"Y+");
            m_AxisFont->draw(L"Y+", rect<s32>(textPos, textSize),
                             SColor(255, 0, 255, 0), true, true);
        }

        m_Driver->setMaterial(zMaterial);
        m_Driver->draw3DLine(vector3df(), vector3df(0, 0, m_AxisLength),
                             SColor(255, 0, 0, 255));
        textPos = m_Scene->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(
            vector3df(0, 0, m_AxisLength + m_AxisLength*.1f)
        );
        if (m_AxisFont != nullptr) {
            textSize = m_AxisFont->getDimension(L"Z+");
            m_AxisFont->draw(L"Z+", rect<s32>(textPos, textSize),
                             SColor(255, 0, 0, 255), true, true);
        }
    }
}

void Engine::drawBackground()
{
    dimension2d<u32> screenSize = m_Driver->getScreenSize();
    m_Driver->draw2DRectangle(
        rect<s32>(0, 0, static_cast<s32>(screenSize.Width),
                  static_cast<s32>(screenSize.Height)),
        SColor(255, 128, 128, 255),
        SColor(255, 128, 128, 255),
        SColor(255, 224, 224, 255),
        SColor(255, 224, 224, 255)
    );
}

void Engine::checkResize()
{
    if ((m_WindowSize.Width != m_Driver->getScreenSize().Width)
            || (m_WindowSize.Height != m_Driver->getScreenSize().Height)) {
        m_WindowSize.Width = m_Driver->getScreenSize().Width;
        m_WindowSize.Height = m_Driver->getScreenSize().Height;

        // Send custom event
        IEventReceiver* eventReceiver = m_Device->getEventReceiver();

        SEvent event;
        event.EventType = EET_USER_EVENT;
        event.UserEvent.UserData1 = UEI_WINDOWSIZECHANGED;
        eventReceiver->OnEvent(event);
        //m_UserInterface->
    }
}

s32 Engine::getNumberOfVertices()
{
    IMesh* mesh = m_LoadedMesh->getMesh()->getMesh(0, 255, -1, -1);

    int vertices = 0;
    for (irr::u32 bufferIndex = 0; bufferIndex < mesh->getMeshBufferCount(); bufferIndex++)
        vertices += mesh->getMeshBuffer(bufferIndex)->getVertexCount();

    cout << vertices << endl;

    return vertices;
}

/* //////////////////////////////////////////////////////////////////////////
   PUBLIC METHODS
   /////////////////////////////////////////////////////////////////////// */

Engine::Engine()
{
    this->m_EnableTests = false;
    this->m_EnableCountMeshes = false;
    this->m_EnableVerbose = false;
    this->m_EnableExit = false;
    settings.set_int("max_recent", 10);
    std::string profile = std::getenv("HOME");
    // ^ changes to USERPROFILE  below if blank
    std::string appdataParent;
    std::string appdatas;
    std::string myAppData;
    if (profile.length() == 0) {
        profile = std::getenv("USERPROFILE");
        appdataParent = profile + path_separator_s + "AppData";
        appdatas = appdataParent + path_separator_s + "Local";
    }
    else {
        appdataParent = profile;
        appdatas = appdataParent + path_separator_s + ".config";
    }
    if (appdatas.length() > 0) {
        myAppData = appdatas + path_separator_s + std::string("b3view");
        if (!Utility::is_directory(myAppData)) {
            Utility::create_directory(myAppData);
        }
    }
    std::string settingsName = "settings.conf";
    std::string settingsPath = settingsName;
    if (myAppData.length() > 0) {
        settingsPath = myAppData + path_separator_s + settingsName;
    }
    settings.load(settingsPath);
    // For monitoring single press: See
    // <http://irrlicht.sourceforge.net/forum/viewtopic.php?p=210744>
    for (u32 i = 0; i < KEY_KEY_CODES_COUNT; ++i)
        KeyIsDown[i] = false;
    for (u32 i = 0; i < KEY_KEY_CODES_COUNT; ++i)
        keyState[i] = 0;
    LMouseState = 0;
    RMouseState = 0;
    this->m_EnableWireframe = false;
    this->m_EnableLighting = false;
    this->m_EnableTextureInterpolation = true;
    this->m_AxisLength = 10;
    this->m_WorldFPS = 60;
    this->m_PrevFPS = 30;
    this->m_TextureExtensions.push_back(L"png");
    this->m_TextureExtensions.push_back(L"jpg");
    this->m_TextureExtensions.push_back(L"bmp");
#if WIN32
    m_Device = createDevice(EDT_DIRECT3D9, dimension2d<u32>(1024, 768), 32,
                            false, false, false, nullptr);
#else
    m_Device = createDevice(EDT_OPENGL, dimension2d<u32>(1024, 768), 32,
                            false, false, false, nullptr);
#endif
    m_Device->setResizable(true);

    m_EventHandler = new EventHandler(m_Device);
    m_Device->setEventReceiver(m_EventHandler);

    m_Driver = m_Device->getVideoDriver();
    m_Scene = m_Device->getSceneManager();

    wstringstream windowTitle;
    windowTitle << L"b3view (Blitz3D/Irrlicht Viewer) [" << m_Driver->getName()
                << L"]";
    m_Device->setWindowCaption(windowTitle.str().c_str());

    setupScene();

    // Setup User Interface
    m_UserInterface = new UserInterface(this);
    m_EventHandler->addEventReceiver(ERT_USERINTERFACE, m_UserInterface);

    // Setup 3D View
    m_View = new View(this);
    m_EventHandler->addEventReceiver(ERT_3DVIEW, m_View);

    // Load font for displaying Axis names
    m_AxisFontFace = new CGUITTFace();
    // NOTE: m_FontPath is modified y UserInterface constructor above if font
    // was missing
    if (m_AxisFontFace->load(m_FontPath.c_str())) {
        m_AxisFont = new CGUITTFont(m_UserInterface->getGUIEnvironment());
        m_AxisFont->attach(m_AxisFontFace, 14);
        m_AxisFont->AntiAlias = false;
    } else {
        delete m_AxisFontFace;
        m_AxisFontFace = nullptr;
    }
    // Set Engine enabled
    m_RunEngine = true;
    m_LoadedMesh = nullptr;

    // Store actual window size
    m_WindowSize.Width = m_Driver->getScreenSize().Width;
    m_WindowSize.Height = m_Driver->getScreenSize().Height;

    // (do not calculate m_Yaw and m_Pitch here--see View constructor)

    this->playAnimation();
}

Engine::~Engine()
{
    m_Device->drop();
    delete m_AxisFont;
    delete m_AxisFontFace;
}

vector3df Engine::camTarget()
{
    return m_CamTarget;
}

bool Engine::loadMesh(const wstring& filePath, bool enableAddRecent)
{
    this->m_UserInterface->setLastLoadedMeshPath(filePath);
    this->m_UserInterface->setModelingEnabled(true, false);
    this->m_LoadedMeshPath = filePath; // even if bad, set this

    bool ret = false;

    irr::scene::IAnimatedMesh* mesh = m_Scene->getMesh(filePath.c_str());
    //   to allow F5 to reload
    if (mesh != nullptr) {
        // this->addRecent(Utility::toString(fileName));
        if (enableAddRecent) {
            this->m_UserInterface->addRecentMenuItem(Utility::toString(filePath), true);
            // ^ hasRecent throws "There was no menu for 1 in hasRecent"
        }
        this->m_LoadedTexturePath = L"";
        this->m_UserInterface->setModelingEnabled(true, true);
        this->m_UserInterface->setTexturingEnabled(true);

        if (m_LoadedMesh != nullptr)
            m_LoadedMesh->remove();
        this->m_LoadedMesh = nullptr;

        m_Device->setWindowCaption((wstring(L"b3view - ") + filePath).c_str());
        m_LoadedMesh = m_Scene->addAnimatedMeshSceneNode(mesh);
        if (this->m_EnableVerbose) {
            Utility::dumpMeshInfoToConsole(m_LoadedMesh);
            std::cerr << "Arranging scene..." << std::flush;
        }
        if (Utility::toLower(Utility::extensionOf(filePath)) == L"3ds") {
            m_View->setZUp(true);
        } else {
            m_View->setZUp(false);
        }
        if (m_LoadedMesh != nullptr) {
            if (this->m_EnableVerbose) {
                std::cerr << "unloading old mesh..." << std::flush;
            }
            ret = true;
            this->m_UserInterface->playbackFPSEditBox->setText(
                Utility::toWstring(m_LoadedMesh->getAnimationSpeed()).c_str()
            );
            ICameraSceneNode* camera = this->m_Scene->getActiveCamera();
            aabbox3d<f32> box = m_LoadedMesh->getTransformedBoundingBox();
            //vector3d<float> extents = box.getExtent();
            if (m_View->zUp()) {
                float oldDist = m_CamPos.getDistanceFrom(m_CamTarget);
                float newDist = oldDist;
                if (Utility::equalsApprox(oldDist, 0.0f)) {
                    vector3d<float> center = box.getCenter();
                    vector3df edges[8];
                    box.getEdges(edges);
                    /*
                                              /3--------/7
                                             /  |      / |
                                            /   |     /  |
                                            1---------5  |
                                            |   2- - -| -6
                                            |  /      |  /
                                            |/        | /
                                            0---------4/
                    */
                    newDist = 0;
                    for (int i = 0; i < 8; i++) {
                        float tryDist = center.getDistanceFrom(edges[i]);
                        if (tryDist > newDist)
                            newDist = tryDist;
                    }
                    newDist *= 2; // so camera doesn't touch model
                    if (!Utility::equalsApprox<float>(newDist, oldDist)) {
                        float scale = newDist / oldDist; // already checked 0
                        vector3df oldCamPos = camera->getPosition();
                        m_CamPos = oldCamPos;
                        m_CamPos.X = m_CamPos.X * scale;
                        m_CamPos.Y = m_CamPos.Y * scale;
                        m_CamPos.Z = m_CamPos.Z * scale;
                        oldCamPos = m_CamPos;
                        m_View->setCameraDistance(
                            m_CamPos.getDistanceFrom(m_CamTarget)
                        );
                        camera->setPosition(m_CamPos);
                    }
                }
            }
            m_LoadedMesh->setMaterialType(
                video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF
            );
            // EMT_TRANSPARENT_ALPHA_CHANNEL: constant transparency

        }
        if (this->m_EnableVerbose) {
            std::cerr << "setting display mode..." << std::flush;
        }
        this->setMeshDisplayMode(this->m_EnableWireframe, this->m_EnableLighting,
                                 this->m_EnableTextureInterpolation);
        if (this->m_EnableVerbose) {
            std::cerr << "preparing UI..." << std::flush;
        }
        if (this->m_UserInterface != nullptr)
            this->m_UserInterface->OnSelectMesh();
        if (this->m_EnableVerbose) {
            std::cerr << "checking for textures..." << std::flush;
            std::cerr << "OK" << std::endl;
        }
        if (Utility::getTextureCount(m_LoadedMesh) == 0) {
            // NOTE: getMaterialCount doesn't work, since there may not
            // be loaded textures in any material.
            if (this->m_UserInterface != nullptr) {
                this->m_UserInterface->loadNextTexture(0);
            }
        }
        if (this->m_EnableVerbose) {
            std::cerr << "detecting last frame..." << std::flush;
        }
        std::wstring prevStartStr;
        std::wstring prevEndStr;
        if (this->m_UserInterface->playbackMenu->getItemText(UIE_PLAYBACKSTARTFRAMEEDITBOX) != nullptr)
            prevStartStr = std::wstring(this->m_UserInterface->playbackMenu->getItemText(UIE_PLAYBACKSTARTFRAMEEDITBOX));
        if (this->m_UserInterface->playbackMenu->getItemText(UIE_PLAYBACKENDFRAMEEDITBOX) != nullptr)
            prevEndStr = std::wstring(this->m_UserInterface->playbackMenu->getItemText(UIE_PLAYBACKENDFRAMEEDITBOX));
        f32 prevStart = -1;
        f32 prevEnd = -1;
        if (prevStartStr.length() > 0)
            prevStart = Utility::toF32(prevStartStr);
        // std::cerr << prevStart << "..." << std::flush;
        if (prevEndStr.length() > 0)
            prevEnd = Utility::toF32(prevEndStr);
        // std::cerr << prevEnd << "..." << std::flush;
        f32 endFrameF32 = static_cast<f32>(m_LoadedMesh->getEndFrame());
        if (this->m_EnableVerbose) {
            std::cerr << endFrameF32 << "..." << std::flush;
        }
        if (prevEnd < 0 || prevEnd > endFrameF32) {
            if (this->m_EnableVerbose) {
                std::cerr << "showing End Frame..." << std::flush;
            }
            this->m_UserInterface->setPlaybackText(
                UIE_PLAYBACKENDFRAMEEDITBOX,
                Utility::toWstring(endFrameF32).c_str()
            );
        }
        if (prevStart < 0 || prevStart > endFrameF32) {
            if (this->m_EnableVerbose) {
                std::cerr << "showing Start Frame..." << std::flush;
            }
            this->m_UserInterface->setPlaybackText(
                UIE_PLAYBACKSTARTFRAMEEDITBOX,
                L"0.0"
            );
        }
        //this->m_UserInterface->playbackMenu->setItemText(UIE_PLAYBACKSTARTFRAMEEDITBOX, );
        //;
        if (this->m_EnableVerbose) {
            std::cerr << "OK" << std::endl;
        }
    }
    // Don't do anything outside of the mesh != nullptr case that will try to
    // use mesh!
    return ret;
}

bool Engine::pushOption(const std::wstring& optionStr)
{
    if (optionStr == L"--test-and-exit") {
        this->m_EnableTests = true;
        this->m_EnableExit = true;
    }
    else if (optionStr == L"--exit") {
        this->m_EnableExit = true;
    }
    else if (optionStr == L"--count-meshes") {
        this->m_EnableCountMeshes = true;
    }
    else if (optionStr == L"--verbose") {
        this->m_EnableVerbose = true;
    }
    else if (optionStr == L"--help") {
        std::cerr
            << "--test-and-exit    Run tests then exit the program." << std::endl
            << "--count-meshes     Count the number of meshes in the file." << std::endl
            << "--verbose          Show mesh metadata (must be before mesh filename to show that) and internal events." << std::endl
            << "--exit             Exit the program after processing other options." << std::endl
        ;
        this->m_EnableExit = true;
    }
    else {
        std::cerr << "The option is not valid: " << Utility::toString(optionStr) << std::endl;
        return false;
    }
    return true;
}

bool Engine::reloadMesh()
{
    bool ret = false;
    if (this->m_LoadedMeshPath.length() > 0) {
        if (m_LoadedMesh != nullptr) {
            // See <https://irrlicht.sourceforge.io/forum/viewtopic.php?t=52859>:
            // only deleted when ref count is 0.
            // m_Scene = m_Device->getSceneManager();
            m_LoadedMesh->remove();
            m_LoadedMesh = nullptr;
            irr::scene::IMeshCache* mesh_cache = m_Scene->getMeshCache();
            mesh_cache->clear();
            // video_driver->removeAllTextures();
            // node->remove();
            // m_LoadedMesh->drop(); // if m_LoadedMesh is not set to nullptr above, this causes a later crash (in remove() in loadMesh?)
            // m_LoadedMesh->remove();
        }
        ret = loadMesh(this->m_LoadedMeshPath, false);
    }
    else {
        this->m_UserInterface->fileMenu->setItemText(this->m_UserInterface->fileReloadModelIdx, L"Reload Model        F5");
    }
    if (this->m_UserInterface != nullptr)
        this->m_UserInterface->OnSelectMesh();
    return ret;
}

std::wstring Engine::saveMesh(const io::path path, const std::string& nameOrBlank, const std::string& extension)
{
    wstring ret = L"";
    // see also https://bitbucket.org/mzeilfelder/irr-playground-micha/src/default/obj_readwrite.cpp (saves scene::EMWT_OBJ)
    scene::ISceneManager* smgr = m_Device->getSceneManager();
    scene::IMeshWriter* meshWriter = nullptr;
    // this->m_FileName = "";
    io::path fileName = io::path();
    io::path mtlName = io::path();
    std::string beginning = "export-";
    std::string mtlTryName = "export.mtl";
    if (nameOrBlank.length() > 0) {
        beginning = nameOrBlank + "#" + Utility::toString(static_cast<int>(round(m_LoadedMesh->getFrameNr()))) + "-";
        mtlTryName = nameOrBlank + ".mtl";
    }
    mtlName = mtlTryName.c_str();
    std::string partial = beginning + Utility::dateTimeNowPathString();
    if (extension == "dae") {
        fileName = (partial + ".dae").c_str();
        meshWriter = smgr->createMeshWriter(scene::EMWT_COLLADA);
    }
    else if (extension == "obj") {
        fileName = (partial + ".obj").c_str();
        meshWriter = smgr->createMeshWriter(scene::EMWT_OBJ);
    }
    else if (extension == "irrmesh") {
        fileName = (partial + ".irrmesh").c_str();
        meshWriter = smgr->createMeshWriter(scene::EMWT_IRR_MESH);
    }
    else if (extension == "stl") {
        fileName = (partial + ".stl").c_str();
        meshWriter = smgr->createMeshWriter(scene::EMWT_STL);
    }
    if (meshWriter != nullptr) {
        // io::path filePath = path + fileName;
        io::path filePath = path + "/" + fileName;
        io::path mtlDestPath = path + "/" + mtlName;
        // TODO: chdir is deprecated due to not following POSIX naming rules--see
        // <https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/chdir?redirectedfrom=MSDN&view=vs-2019>.
        // Therefore, normally use _chdir or _wchdir instead.
        // However, it is undeclared while using gcc even after including unistd.h.
        chdir(path.c_str());
        io::IWriteFile* meshFile = m_Device->getFileSystem()->createAndWriteFile(filePath);
        if (!meshWriter->writeMesh(meshFile, m_LoadedMesh->getMesh())) {
            debug() << "saving failed" << endl;
        }
        else {
            debug() << "saving ok" << endl;
            ret = Utility::toWstring(filePath.c_str());
            debug() << "checking for " << mtlTryName << "..." << std::flush;
            if (Utility::isFile(mtlDestPath.c_str())){
                debug() << "chdir succeeded, so file is in " << mtlDestPath.c_str() << "." << endl;
                ret += L";\n" + Utility::toWstring(mtlDestPath.c_str());
            }
            else if (Utility::isFile(mtlTryName)) {
                // NOTE: std::filesystem::rename
                // requires: C++17 (on gcc 8.0.1, link against -lstdc++fs and #include <filesystem>)
                // (See https://stackoverflow.com/questions/22201663/find-and-move-files-in-c),
                // so use std::rename and <cerrno> instead:
                if (std::rename(mtlTryName.c_str(), mtlDestPath.c_str()) < 0) {
                    ret += L";\n<unmovable mtl file in current directory:>" + Utility::toWstring(mtlTryName.c_str());
                    std::cerr << strerror(errno) << std::endl;
                }
                else {
                    ret += L";\n" + Utility::toWstring(mtlDestPath.c_str());
                    debug() << "moved to " << mtlDestPath.c_str() << "." << endl;
                }
//                try {
//                    std::filesystem::rename(mtlTryName.c_str(), mtlDestPath.c_str())
//                    ret += L";" + Utility::toWstring(mtlDestPath.c_str());
//                } catch (std::filesystem::filesystem_error& e) {
//                    std::cerr << e.what() << '\n';
//                    ret += L";<unmovable mtl file in current directory:>" + Utility::toWstring(mtlTryName.c_str());
//                }

            }
            else {
                debug() << "not found." << endl;
                ret += L";\n<unknown mtl file in current directory>";
            }
        }
        meshFile->drop();
        meshWriter->drop();
    }
    else if (extension == "irr") {
        fileName = (partial + ".irr").c_str();
        io::path filePath = path + "/" + fileName;
        if (!smgr->saveScene(filePath)) {
            debug() << "saving failed" << endl;
        }
        else {
            debug() << "saving ok" << endl;
            ret = Utility::toWstring(filePath.c_str());
        }
    }
    return ret;
}

bool Engine::reloadTexture()
{
    bool result = false;
    if (this->m_LoadedTexturePath.length() > 0) {
        if (wcslen(this->m_UserInterface->texturePathEditBox->getText()) == 0)
            result = loadTexture(this->m_UserInterface->texturePathEditBox->getText(), true);
        else
            result = loadTexture(this->m_LoadedTexturePath, true);
    }
    return result;
}

bool Engine::loadTexture(const wstring& fileName, bool reload)
{
    bool ret = false;
    if (m_LoadedMesh != nullptr) {
        ITexture* texture = this->m_Driver->getTexture(fileName.c_str());
        if (reload) {
            // The texture may already have been read from storage, so:
            this->m_Driver->removeTexture(texture);
            texture = this->m_Driver->getTexture(fileName.c_str());
        }
        if (texture != nullptr) {
            m_LoadedMesh->setMaterialTexture(0, texture);
            ret = true;
            debug() << "* loaded " << "" << std::endl;
        }
        else {
                debug() << "* failed to load " << "" << std::endl;
        }
        this->m_LoadedTexturePath = fileName;
        if (this->m_EnableVerbose) {
            std::cerr << "Setting texture path box to " << Utility::toString(this->m_LoadedTexturePath)  << std::endl;
        }
        this->m_UserInterface->texturePathEditBox->setText(
            this->m_LoadedTexturePath.c_str()
        );
    }
    else {
        if (this->m_EnableVerbose) {
            std::cerr << "NOT Setting texture path box to " << Utility::toString(this->m_LoadedTexturePath)  << std::endl;
        }
    }
    return ret;
}

void Engine::setMeshDisplayMode(bool wireframe, bool lighting,
                                bool textureInterpolation)
{
    this->m_EnableWireframe = wireframe;
    this->m_EnableLighting = lighting;
    this->m_EnableTextureInterpolation = textureInterpolation;
    if (m_LoadedMesh != nullptr) {
        for (u32 materialIndex = 0; materialIndex < m_LoadedMesh->getMaterialCount(); materialIndex++) {
            // Set Wireframe display
            m_LoadedMesh->getMaterial(materialIndex).Wireframe = wireframe;

            // Set Lighting
            if (!lighting) {
                m_LoadedMesh->getMaterial(materialIndex).Lighting = false;
                m_LoadedMesh->getMaterial(materialIndex).EmissiveColor = SColor(
                    255,
                    255,
                    255,
                    255
                );
            } else {
                m_LoadedMesh->getMaterial(materialIndex).Lighting = true;
                m_LoadedMesh->getMaterial(materialIndex).EmissiveColor = SColor(
                    255,
                    0,
                    0,
                    0
                );
            }
            // m_LoadedMesh->setMaterialType(
            //     video::EMT_TRANSPARENT_ALPHA_CHANNEL
            // ); // already done on load
            // // requires EMT_ONETEXTURE:
            // m_LoadedMesh->setMaterialFlag(video::E_ALPHA_SOURCE, true);
            if (textureInterpolation) {
                m_LoadedMesh->setMaterialFlag(video::EMF_BILINEAR_FILTER, true);
                m_LoadedMesh->setMaterialFlag(video::EMF_TRILINEAR_FILTER,
                                              true);
            } else {
                m_LoadedMesh->setMaterialFlag(video::EMF_BILINEAR_FILTER,
                                              false);
                m_LoadedMesh->setMaterialFlag(video::EMF_TRILINEAR_FILTER,
                                              false);
                // m_LoadedMesh->setMaterialFlag(video::E_ALPHA_SOURCE, true);

                // below doesn't work for some reason:
                // video::SMaterial mat = m_LoadedMesh->getMaterial(
                //     materialIndex
                // );
                // mat.UseMipMaps = false;
                // mat.setFlag(video::EMF_BILINEAR_FILTER, false);
                // mat.setFlag(video::EMF_TRILINEAR_FILTER, false);

                // below would require patching Irrlicht:
                // GLint filteringMipMaps = GL_NEAREST_MIPMAP_NEAREST
                // // above is used by glTexParameteri(GL_TEXTURE_2D,
                // //                                  GL_TEXTURE_MIN_FILTER,
                // //                                  filteringMipMaps);
            }
        }
    } else
        debug() << "WARNING in setMeshDisplayMode: No mesh is loaded " << endl;
}

bool Engine::isAnimating()
{
    return this->m_IsPlaying;
}

void Engine::playAnimation()
{
    if (this->animationFPS() < 1) {
        this->setAnimationFPS(5);
    }
    if (!this->isAnimating()) {
        if (this->m_LoadedMesh != nullptr) {
            if (this->m_PrevFPS < 1)
                this->m_PrevFPS = 5;
            this->m_LoadedMesh->setAnimationSpeed(this->m_PrevFPS);
        }
    }
    this->m_IsPlaying = true;
}

void Engine::pauseAnimation()
{
    if (this->isAnimating()) {
        this->m_PrevFPS = animationFPS();
        if (this->m_LoadedMesh != nullptr) {
            this->m_PrevFPS = this->m_LoadedMesh->getAnimationSpeed();
            this->m_LoadedMesh->setAnimationSpeed(0);
        }
    }
    this->m_IsPlaying = false;
}

void Engine::toggleAnimation()
{
    if (this->isAnimating()) {
        this->pauseAnimation();
        debug() << "paused " << this->animationFPS() << "fps" << endl;
    } else {
        this->playAnimation();
        debug() << "unpaused " << this->animationFPS() << "fps" << endl;
    }
}

void Engine::setAnimationFPS(u32 animationFPS)
{
    if (this->m_LoadedMesh != nullptr) {
        if (animationFPS > 0) this->m_IsPlaying = true;
        // Do NOT call playAnimation, otherwise infinite recursion occurs
        // (it calls setAnimationFPS).
        this->m_LoadedMesh->setAnimationSpeed(animationFPS);
        this->m_UserInterface->playbackFPSEditBox->setText(
            Utility::toWstring(this->m_LoadedMesh->getAnimationSpeed()).c_str()
        );
    }
}

void Engine::incrementAnimationFPS(irr::f32 by)
{
    if (this->m_LoadedMesh != nullptr) {
        if (by < 0) {
            if (this->m_LoadedMesh->getAnimationSpeed() + by >= 0.999999f)
                // don't use this->animationFPS() above--its unsigned!
                this->setAnimationFPS(this->m_LoadedMesh->getAnimationSpeed()
                                      + by);
            else
                this->setAnimationFPS(1);
        }
        else {
            this->setAnimationFPS(this->animationFPS() + by);
        }
    }
}

void Engine::setZUp(bool zUp)
{
    if (this->m_View != nullptr) {
        this->m_View->setZUp(zUp);
    }
}

u32 Engine::animationFPS()
{
    f32 ret = 0;
    if (this->m_LoadedMesh != nullptr) {
        ret = this->m_LoadedMesh->getAnimationSpeed();
    }
    return static_cast<u32>(ret);
}

void Engine::run()
{
    u32 timePerFrame = 1000.0f;
    if (this->m_WorldFPS > 0) {
        timePerFrame = static_cast<u32>(1000.0f / this->m_WorldFPS);
    }
    ITimer* timer = m_Device->getTimer();

    // Run the Device with fps frames/sec
    while (m_Device->run() && m_RunEngine) {
        if (this->m_EnableCountMeshes) {
            this->m_EnableCountMeshes = false;
            if (this->m_LoadedMesh != nullptr) {
                std::cout << "mesh_count=" << this->m_LoadedMesh->getMesh()->getMeshBufferCount() << std::endl;
            }
        }
        if (this->m_EnableTests) {
            std::cerr << "* running tests..." << std::endl;
            this->m_EnableTests = false;
            std::cerr << "* loading test model..." << std::endl;
            if (!this->loadMesh(L"dist/share/b3view/meshes/penguin-lowpoly-poikilos.b3d", false)) {
                throw "loading dist/share/b3view/meshes/penguin-lowpoly-poikilos.b3d failed.";
            }
            std::cerr << "* loading test model's next texture..." << std::endl;
            if (!this->m_UserInterface->loadNextTexture(1)) {
                throw "loading the next texture for dist/share/b3view/meshes/penguin-lowpoly-poikilos.b3d failed.";
            }
        }
        if (this->m_EnableExit) {
            this->m_RunEngine = false;
            if (!this->m_EnableTests) {
                break;
            }
            // else don't break yet: Test the main event loop too.
        }

        u32 startTime = timer->getRealTime();

        checkResize();
        if (this->m_LoadedMesh != nullptr) {
            if (m_IsPlaying) {
                this->m_LoadedMesh->setLoopMode(true);
                this->m_UserInterface->playbackSetFrameEditBox->setText(
                    Utility::toWstring(this->m_LoadedMesh->getFrameNr()).c_str()
                );
            } else {
                this->m_LoadedMesh->setLoopMode(false);
            }
        }
        m_Driver->beginScene();
        drawBackground(); // Draw Background
        drawAxisLines(); // Draw XYZ Axis
        m_Scene->drawAll(); // Draw Scenegraph

        m_UserInterface->getGUIEnvironment()->drawAll();
        m_UserInterface->drawStatusLine();

        m_Driver->endScene();

        u32 sleepTime = timePerFrame - (timer->getRealTime() - startTime);
        if (sleepTime > 0 && sleepTime < timePerFrame)
            m_Device->sleep(sleepTime, false);
    }
}

bool Engine::loadScene(const std::wstring &fileName)
{
    scene::ISceneManager* smgr = this->m_Device->getSceneManager();
    bool result = smgr->loadScene(fileName.c_str());
    return result;
}
