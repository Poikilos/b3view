#include <string>

#include "Engine.h"

#include "UserInterface.h"
#include "Utility.h"
#include "View.h"

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
    camera->setAspectRatio(static_cast<f32>(m_Driver->getScreenSize().Width)
                           / static_cast<f32>(m_Driver->getScreenSize().Height));
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
        m_Driver->draw3DLine(vector3df(), vector3df(axisLength, 0, 0),
                             SColor(255, 255, 0, 0));
        position2d<s32> textPos = m_Scene->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(
            vector3df(axisLength + axisLength*.1f, 0, 0)
        );
        dimension2d<u32> textSize;
        if (m_AxisFont != nullptr) {
            textSize = m_AxisFont->getDimension(L"X+");
            m_AxisFont->draw(L"X+", rect<s32>(textPos, textSize),
                             SColor(255, 255, 0, 0), true, true);
        }

        m_Driver->setMaterial(yMaterial);
        m_Driver->draw3DLine(vector3df(), vector3df(0, axisLength, 0),
                             SColor(255, 0, 255, 0));
        textPos = m_Scene->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(
            vector3df(0, axisLength + axisLength*.1f, 0)
        );
        if (m_AxisFont != nullptr) {
            textSize = m_AxisFont->getDimension(L"Y+");
            m_AxisFont->draw(L"Y+", rect<s32>(textPos, textSize),
                             SColor(255, 0, 255, 0), true, true);
        }

        m_Driver->setMaterial(zMaterial);
        m_Driver->draw3DLine(vector3df(), vector3df(0, 0, axisLength),
                             SColor(255, 0, 0, 255));
        textPos = m_Scene->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(
            vector3df(0, 0, axisLength + axisLength*.1f)
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
    // For monitoring single press: see
    // <http://irrlicht.sourceforge.net/forum/viewtopic.php?p=210744>
    for (u32 i = 0; i < KEY_KEY_CODES_COUNT; ++i)
        KeyIsDown[i] = false;
    for (u32 i = 0; i < KEY_KEY_CODES_COUNT; ++i)
        keyState[i] = 0;
    LMouseState = 0;
    RMouseState = 0;
    this->axisLength = 10;
    this->worldFPS = 60;
    this->prevFPS = 30;
    this->textureExtensions.push_back(L"png");
    this->textureExtensions.push_back(L"jpg");
    this->textureExtensions.push_back(L"bmp");
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

bool Engine::loadMesh(const wstring& fileName)
{
    bool ret = false;
    this->m_PreviousPath = fileName; // even if bad, set this
        // to allow F5 to reload

    if (m_LoadedMesh != nullptr)
        m_LoadedMesh->remove();

    irr::scene::IAnimatedMesh* mesh = m_Scene->getMesh(fileName.c_str());
    if (mesh != nullptr) {
        m_Device->setWindowCaption((wstring(L"b3view - ") + fileName).c_str());
        m_LoadedMesh = m_Scene->addAnimatedMeshSceneNode(mesh);
        Utility::dumpMeshInfoToConsole(m_LoadedMesh);
        if (Utility::toLower(Utility::extensionOf(fileName)) == L"3ds") {
            m_View->setZUp(true);
        } else {
            m_View->setZUp(false);
        }
        if (m_LoadedMesh != nullptr) {
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
    }
    return ret;
}

bool Engine::reloadMesh()
{
    bool ret = false;
    if (this->m_PreviousPath.length() > 0) {
        ret = loadMesh(this->m_PreviousPath);
    }
    return ret;
}

std::wstring Engine::saveMesh(const io::path path, const std::string& nameOrBlank, const std::string& extension)
{
    wstring ret = L"";
    // see also https://bitbucket.org/mzeilfelder/irr-playground-micha/src/default/obj_readwrite.cpp (saves scene::EMWT_OBJ)
    scene::ISceneManager* smgr = m_Device->getSceneManager();
    scene::IMeshWriter* meshWriter = nullptr;
    //this->m_FileName = "";
    io::path fileName = io::path();
    std::string beginning = "export-";
    if (nameOrBlank.length() > 0) {
        beginning = nameOrBlank + "#" + Utility::toString(static_cast<int>(round(m_LoadedMesh->getFrameNr()))) + "-";
    }
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
        //io::path filePath = path + fileName;
        io::path filePath = path + "/" + fileName;
        io::IWriteFile* meshFile = m_Device->getFileSystem()->createAndWriteFile(filePath);
        if (!meshWriter->writeMesh(meshFile, m_LoadedMesh->getMesh())) {
            debug() << "saving failed" << endl;
        }
        else {
            debug() << "saving ok" << endl;
            ret = Utility::toWstring(filePath.c_str());
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

void Engine::reloadTexture()
{
    if (this->m_PrevTexturePath.length() > 0) {
        if (wcslen(this->m_UserInterface->texturePathEditBox->getText()) == 0)
            loadTexture(this->m_UserInterface->texturePathEditBox->getText());
        else
            loadTexture(this->m_PrevTexturePath);
    }
}

bool Engine::loadTexture(const wstring& fileName)
{
    bool ret = false;
    if (m_LoadedMesh != nullptr) {
        ITexture* texture = this->m_Driver->getTexture(fileName.c_str());
        if (texture != nullptr) {
            m_LoadedMesh->setMaterialTexture(0, texture);
            ret = true;
        }
        this->m_PrevTexturePath = fileName;
        this->m_UserInterface->texturePathEditBox->setText(
            this->m_PrevTexturePath.c_str()
        );
    }
    return ret;
}

void Engine::setMeshDisplayMode(bool wireframe, bool lighting,
                                bool textureInterpolation)
{
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
            // ); //already done on load
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
                //m_LoadedMesh->setMaterialFlag(video::E_ALPHA_SOURCE, true);

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
    return this->isPlaying;
}

void Engine::playAnimation()
{
    if (this->animationFPS() < 1) {
        this->setAnimationFPS(5);
    }
    if (!this->isAnimating()) {
        if (this->m_LoadedMesh != nullptr) {
            if (this->prevFPS < 1)
                this->prevFPS = 5;
            this->m_LoadedMesh->setAnimationSpeed(this->prevFPS);
        }
    }
    this->isPlaying = true;
}

void Engine::pauseAnimation()
{
    if (this->isAnimating()) {
        this->prevFPS = animationFPS();
        if (this->m_LoadedMesh != nullptr) {
            this->prevFPS = this->m_LoadedMesh->getAnimationSpeed();
            this->m_LoadedMesh->setAnimationSpeed(0);
        }
    }
    this->isPlaying = false;
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
        if (animationFPS > 0) this->isPlaying = true;
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
    if (this->worldFPS > 0) {
        timePerFrame = static_cast<u32>(1000.0f / this->worldFPS);
    }
    ITimer* timer = m_Device->getTimer();

    // Run the Device with fps frames/sec
    while (m_Device->run() && m_RunEngine) {
        u32 startTime = timer->getRealTime();

        checkResize();
        if (this->m_LoadedMesh != nullptr) {
            if (isPlaying) {
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
