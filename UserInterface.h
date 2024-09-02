#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "extlib/CGUITTFont.h"
#include <irrlicht/irrlicht.h>

#include <string>
#include <vector>

// Forward declaration of class Engine
class Engine;

enum UserInterfaceElements {
    UIE_FILEMENU                    = 1000,

    // This whole range (1100-1198) must stay free for generated submenus:
    UIE_RECENTMENU                  = 1100,
    UIE_RECENTMENU_LAST             = 1198,

    UIE_RECENTMENU_CLEAR            = 1199,
    UIE_LOADFILEDIALOG              = 1200,
    // UIE_LOADBUTTON                  = 1101,
    UIE_LOADTEXTUREDIALOG           = 1300,
    UIE_SAVEFILEDIALOG              = 1400,

    UIE_PLAYBACKMENU                = 2000,

    UIE_VIEWMENU                    = 3000,

    UIE_PLAYBACKWINDOW              = 10000,
    UIE_PLAYBACKSTARTFRAMESTATICTEXT= 10001,
    UIE_PLAYBACKSTARTFRAMEEDITBOX   = 10002,
    UIE_PLAYBACKENDFRAMESTATICTEXT  = 10003,
    UIE_PLAYBACKENDFRAMEEDITBOX     = 10004,
    UIE_PLAYBACKSTARTSTOPBUTTON     = 10005,
    UIE_PLAYBACKSETFRAMEEDITBOX     = 10006,
    UIE_PLAYBACKINCREASEBUTTON      = 10007,
    UIE_PLAYBACKDECREASEBUTTON      = 10008,
    UIE_FPSEDITBOX                  = 10009,
    UIE_TEXTUREPATHSTATICTEXT       = 10010,
    UIE_TEXTUREPATHEDITBOX          = 10011,
    UIE_AXISSIZESTATICTEXT          = 10012,
    UIE_AXISSIZEEDITBOX             = 10013
};

enum UserInterfaceCommands {
    UIC_FILE_OPEN                   = 1001,
    UIC_FILE_RECENT                 = 1100, // this whole range (1100-1198) must stay free for generated submenus
    UIC_FILE_RECENT_CLEAR           = 1199,
    UIC_FILE_QUIT                   = 1002,
    UIC_FILE_OPEN_TEXTURE           = 1003,
    UIC_FILE_NEXT_TEXTURE           = 1004,
    UIC_FILE_PREVIOUS_TEXTURE       = 1005,
    UIC_FILE_EXPORT_DAE             = 1006,
    UIC_FILE_EXPORT_IRR             = 1007,
    UIC_FILE_EXPORT_IRRMESH         = 1008,
    UIC_FILE_EXPORT_OBJ             = 1009,
    UIC_FILE_EXPORT_STL             = 1010,
    UIC_FILE_RELOAD_MESH            = 1011,
    UIC_FILE_RELOAD_TEXTURE         = 1012,
    UIC_PLAYBACK_PREVIOUS           = 2001,
    UIC_PLAYBACK_NEXT               = 2002,
    UIC_PLAYBACK_SLOWER             = 2003,
    UIC_PLAYBACK_FASTER             = 2004,
    UIC_VIEW_WIREFRAME              = 3001,
    UIC_VIEW_LIGHTING               = 3002,
    UIC_VIEW_AXIS_WIDGET            = 3003,
    UIC_VIEW_TARGET                 = 3004,
    UIC_VIEW_TEXTURE_INTERPOLATION  = 3005,
    UIC_VIEW_Y_UP                   = 3006,
    UIC_VIEW_Z_UP                   = 3007
};

class UserInterface : public irr::IEventReceiver {
private:
    irr::s32 spacing_y;
    static const irr::u32 UIC_FILE_RECENT_FIRST;
    irr::u32 uic_file_recent_next;
    irr::s32 m_file_recent_first_idx;
    irr::s32 m_file_recent_last_idx;
    Engine* m_Engine;
    irr::gui::IGUIEnvironment* m_Gui;
    irr::gui::CGUITTFont* m_GuiFont;
    irr::gui::CGUITTFace* m_GuiFontFace;
    bool recent_initialized;

    void setupUserInterface();
    void displayLoadFileDialog();
    void displaySaveFileDialog();
    void displayLoadTextureDialog();
    void incrementFrame(irr::f32 frameCount, bool enableRound);
    bool handleMenuItemPressed(const irr::SEvent::SGUIEvent* ge);
    void updateSettingsDisplay();

    irr::gui::IGUIWindow* playbackWindow;
    irr::core::dimension2d<irr::u32> m_WindowSize; // previous size
    std::vector<std::wstring> m_AllTextures;
    std::vector<std::wstring> m_MatchingTextures;
public:
    irr::gui::IGUIContextMenu* menu;
    irr::gui::IGUIContextMenu* fileMenu;
    irr::gui::IGUIContextMenu* recentMenu;
    irr::gui::IGUIContextMenu* playbackMenu;
    irr::gui::IGUIContextMenu* viewMenu;
    irr::gui::IGUIStaticText* playbackStartFrameStaticText;
    irr::gui::IGUIEditBox* playbackStartFrameEditBox;
    irr::gui::IGUIStaticText* playbackEndFrameStaticText;
    irr::gui::IGUIEditBox* playbackEndFrameEditBox;
    irr::gui::IGUIButton* playbackStartStopButton;
    irr::gui::IGUIEditBox* playbackSetFrameEditBox;
    irr::gui::IGUIButton* playbackIncreaseButton;
    irr::gui::IGUIButton* playbackDecreaseButton;
    irr::gui::IGUIEditBox* playbackFPSEditBox;
    irr::gui::IGUIStaticText* texturePathStaticText;
    irr::gui::IGUIEditBox* texturePathEditBox;
    irr::gui::IGUIStaticText* axisSizeStaticText;
    irr::gui::IGUIEditBox* axisSizeEditBox;
    std::vector<irr::u32> fileExportIndices;
    irr::u32 fileRecentIdx;
    irr::u32 fileReloadModelIdx;
    irr::u32 fileReloadTextureIdx;
    irr::u32 fileChangeTextureIdx;
    irr::u32 filePreviousTextureIdx;
    irr::u32 fileNextTextureIdx;
    irr::u32 fileRecentClearIdx;
    irr::u32 playbackMenuIdx;
    irr::u32 viewMenuIdx;
    irr::u32 fileMenuIdx;
    std::vector<irr::u32> recentIndices;
    irr::u32 viewTextureInterpolationIdx;
    irr::u32 viewWireframeIdx;
    irr::u32 viewAxisWidgetIdx;
    irr::u32 viewLightingIdx;
    irr::u32 viewTargetIdx;
    irr::u32 viewYUpIdx;
    irr::u32 viewZUpIdx;

    void snapWidgets();

    UserInterface(Engine* device);
    ~UserInterface();
    irr::gui::IGUIEnvironment* getGUIEnvironment() const;
    void drawStatusLine() const;
    bool loadNextTexture(int direction);
    void exportMeshToHome(std::string extension);
    void clearRecent();
    void addRecentMenuItem(std::string path, bool addToEngine);
    void addRecentMenuItems(std::vector<std::string> paths, bool addToEngine);
    bool hasRecent(std::string path);
    bool openRecent(irr::s32 commandID, irr::s32 selectedItemID);
    bool OnSelectMesh();
    void setPlaybackText(irr::s32 id, const wchar_t* str);

    // IEventReceiver
    virtual bool OnEvent(const irr::SEvent& event);
};

#endif // USERINTERFACE_H
