#ifndef ENGINE_H
#define ENGINE_H

// Forward declaration of class UserInterface
class UserInterface;
class View;

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "EventHandler.h"
#include "extlib/CGUITTFont.h"
#include <irrlicht/irrlicht.h>
#include "settings.h"

enum SceneItemID {
    SIID_LIGHT = 1,
    SIID_CAMERA = 2,
    SIID_MODEL = 3
};

class Engine {
    friend class UserInterface;
    friend class View;

private:
    irr::IrrlichtDevice* m_Device;
    irr::video::IVideoDriver* m_Driver;
    irr::scene::ISceneManager* m_Scene;
    irr::scene::IAnimatedMeshSceneNode* m_LoadedMesh;
    irr::scene::ILightSceneNode* m_SceneLight;
    irr::gui::CGUITTFont* m_AxisFont;
    irr::gui::CGUITTFace* m_AxisFontFace;

    irr::core::dimension2d<irr::u32> m_WindowSize;

    bool m_RunEngine;
    bool m_EnableWireframe;
    bool m_EnableLighting;
    bool m_EnableTextureInterpolation;

    EventHandler* m_EventHandler;
    UserInterface* m_UserInterface;
    View* m_View;

    void setupScene();
    void drawAxisLines();
    void drawBackground();
    void checkResize();
    irr::gui::IGUIEnvironment* getGUIEnvironment() const;
    irr::s32 getNumberOfVertices();
    bool m_IsPlaying;
    irr::u32 m_WorldFPS;
    irr::u32 m_PrevFPS;
    std::vector<std::wstring> m_TextureExtensions;
    // Making materials in contructor or setupScene causes segfault at
    // `m_Driver->setMaterial(*lineX);` in
    // `Engine::drawAxisLines` for unknown reason:
    // irr::video::SMaterial *lineX;
    // irr::video::SMaterial *lineY;
    // irr::video::SMaterial *lineZ;
    irr::core::vector3df m_CamPos;
    irr::core::vector3df m_CamTarget;
    std::wstring m_FontPath = L"ClearSansRegular.ttf"; // core::stringc has implicit conversion to io::path
    bool KeyIsDown[irr::KEY_KEY_CODES_COUNT];
    irr::s32 keyState[irr::KEY_KEY_CODES_COUNT];
    irr::s32 LMouseState, RMouseState;
    Settings settings;
public:
    std::wstring m_LoadedMeshPath;
    std::wstring m_LoadedTexturePath;
    irr::f32 m_AxisLength;
    bool m_zUp;

    Engine();
    ~Engine();

    irr::core::vector3df camTarget();
    void run();
    bool loadScene(const std::wstring& fileName);
    bool loadMesh(const std::wstring& fileName);
    bool reloadMesh();
    std::wstring saveMesh(const irr::io::path path, const std::string& nameOrBlank, const std::string& extension);
    void reloadTexture();
    bool loadTexture(const std::wstring& fileName);
    void setMeshDisplayMode(bool wireframe = false, bool lighting = false, bool textureInterpolation = true);
    bool isAnimating();
    void playAnimation();
    void pauseAnimation();
    void toggleAnimation();
    void setAnimationFPS(irr::u32 animationFPS);
    void incrementAnimationFPS(irr::f32 by);
    void setZUp(bool zUp);
    irr::u32 animationFPS();
    bool getEnableWireframe() const;
    bool getEnableLighting() const;
    bool getEnableTextureInterpolation() const;
    void setEnableWireframe(bool EnableWireframe);
    void setEnableLighting(bool EnableLighting);
    void setEnableTextureInterpolation(bool EnableTextureInterpolation);
    void addRecent(std::string path);
    void addRecentPaths(std::vector<std::string> paths);
    int countRecent();
    std::vector<std::string> recentPaths();
    bool hasRecent(std::string path);
};

#endif // ENGINE_H
