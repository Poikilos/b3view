#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "extlib/CGUITTFont.h"
#include <irrlicht/irrlicht.h>

#include <string>

// Forward declaration of class Engine
class Engine;

enum UserInterfaceElements {
    UIE_FILEMENU                    = 1003,
    UIE_LOADFILEDIALOG              = 1100,
    // UIE_LOADBUTTON                  = 1101,
    UIE_LOADTEXTUREDIALOG           = 1200,
    UIE_SAVEFILEDIALOG              = 1300,

    UIE_PLAYBACKMENU                = 2000,

    UIE_VIEWMENU                    = 3000,

    UIE_PLAYBACKWINDOW              = 10000,
    UIE_PLAYBACKSTARTSTOPBUTTON     = 10001,
    UIE_PLAYBACKSETFRAMEEDITBOX     = 10002,
    UIE_PLAYBACKINCREASEBUTTON      = 10003,
    UIE_PLAYBACKDECREASEBUTTON      = 10004,
    UIE_FPSEDITBOX                  = 10005,
    UIE_TEXTUREPATHSTATICTEXT       = 10006,
    UIE_TEXTUREPATHEDITBOX          = 10007,
    UIE_AXISSIZESTATICTEXT          = 10008,
    UIE_AXISSIZEEDITBOX             = 10009
};

enum UserInterfaceCommands {
    UIC_FILE_OPEN                   = 1000,
    UIC_FILE_QUIT                   = 1001,
    UIC_FILE_OPEN_TEXTURE           = 1002,
    UIC_FILE_NEXT_TEXTURE           = 1003,
    UIC_FILE_PREVIOUS_TEXTURE       = 1004,
    UIC_FILE_EXPORT_DAE             = 1005,
    UIC_FILE_EXPORT_IRR             = 1006,
    UIC_FILE_EXPORT_IRRMESH         = 1007,
    UIC_FILE_EXPORT_OBJ             = 1008,
    UIC_FILE_EXPORT_STL             = 1009,
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
    Engine* m_Engine;
    irr::gui::IGUIEnvironment* m_Gui;
    irr::gui::CGUITTFont* m_GuiFont;
    irr::gui::CGUITTFace* m_GuiFontFace;

    void setupUserInterface();
    void displayLoadFileDialog();
    void displaySaveFileDialog();
    void displayLoadTextureDialog();
    void incrementFrame(irr::f32 frameCount, bool enableRound);
    void handleMenuItemPressed(irr::gui::IGUIContextMenu* menu);

    bool m_WireframeDisplay;
    bool m_Lighting;
    bool m_TextureInterpolation;
    irr::gui::IGUIWindow* playbackWindow;
    irr::core::dimension2d<irr::u32> m_WindowSize; // previous size
public:
    irr::gui::IGUIContextMenu* menu;
    irr::gui::IGUIContextMenu* fileMenu;
    irr::gui::IGUIContextMenu* playbackMenu;
    irr::gui::IGUIContextMenu* viewMenu;
    irr::gui::IGUIButton* playbackStartStopButton;
    irr::gui::IGUIEditBox* playbackSetFrameEditBox;
    irr::gui::IGUIButton* playbackIncreaseButton;
    irr::gui::IGUIButton* playbackDecreaseButton;
    irr::gui::IGUIEditBox* playbackFPSEditBox;
    irr::gui::IGUIStaticText* texturePathStaticText;
    irr::gui::IGUIEditBox* texturePathEditBox;
    irr::gui::IGUIStaticText* axisSizeStaticText;
    irr::gui::IGUIEditBox* axisSizeEditBox;
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

    // IEventReceiver
    virtual bool OnEvent(const irr::SEvent& event);
};

#endif // USERINTERFACE_H
