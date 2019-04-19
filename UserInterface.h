#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "extlib/CGUITTFont.h"
#include <irrlicht/irrlicht.h>

// Forward declaration of class Engine
class Engine;

enum UserInterfaceElements {
    UIE_FILEMENU                    = 1003,
    UIE_LOADFILEDIALOG              = 1100,
    // UIE_LOADBUTTON                  = 1101,
    UIE_LOADTEXTUREDIALOG           = 1200,

    UIE_VIEWMENU                    = 2000,

    UIE_PLAYBACKWINDOW              = 3000,
    UIE_PLAYBACKSTARTSTOPBUTTON     = 3001,
    UIE_PLAYBACKSETFRAMEEDITBOX     = 3002,
    UIE_PLAYBACKINCREASEBUTTON      = 3003,
    UIE_PLAYBACKDECREASEBUTTON      = 3004,
    UIE_FPSEDITBOX                  = 3005,
    UIE_TEXTUREPATHSTATICTEXT       = 3006,
    UIE_TEXTUREPATHEDITBOX          = 3007,
    UIE_AXISSIZESTATICTEXT          = 3008,
    UIE_AXISSIZEEDITBOX             = 3009
};

enum UserInterfaceCommands {
    UIC_FILE_OPEN                   = 1000,
    UIC_FILE_QUIT                   = 1001,
    UIC_FILE_OPEN_TEXTURE           = 1002,
    UIC_FILE_NEXT_TEXTURE           = 1003,
    UIC_FILE_PREVIOUS_TEXTURE       = 1004,
    UIC_VIEW_WIREFRAME              = 2001,
    UIC_VIEW_LIGHTING               = 2002,
    UIC_VIEW_TARGET                 = 2003,
    UIC_VIEW_TEXTURE_INTERPOLATION  = 2004,
    UIC_VIEW_Y_UP                   = 2005,
    UIC_VIEW_Z_UP                   = 2006,
    UIC_VIEW_SLOWER                 = 2007,
    UIC_VIEW_FASTER                 = 2008
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
    void displayLoadTextureDialog();
    void handleMenuItemPressed(irr::gui::IGUIContextMenu* menu);

    bool m_WireframeDisplay;
    bool m_Lighting;
    bool m_TextureInterpolation;
    irr::gui::IGUIWindow* playbackWindow;
    irr::core::dimension2d<irr::u32> m_WindowSize; // previous size
public:
    irr::gui::IGUIContextMenu* menu;
    irr::gui::IGUIContextMenu* fileMenu;
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

    // IEventReceiver
    virtual bool OnEvent(const irr::SEvent& event);
};

#endif // USERINTERFACE_H
