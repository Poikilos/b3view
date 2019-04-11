#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <irrlicht/irrlicht.h>
#include "extlib/CGUITTFont.h"

// Forward declaration of class Engine
class Engine;

enum UserInterfaceElements
{
    UIE_FILEMENU                    = 1003,
    UIE_LOADFILEDIALOG              = 1100,
    // UIE_LOADBUTTON                  = 1101,
    UIE_LOADTEXTUREDIALOG           = 1200,

    UIE_VIEWMENU                    = 2000,

    UIE_PLAYBACKWINDOW              = 3000,
    UIE_PLAYBACKSTARTSTOPBUTTON     = 3001,
    UIE_PLAYBACKINCREASEBUTTON      = 3002,
    UIE_PLAYBACKDECREASEBUTTON      = 3003,
    UIE_PLAYBACKSETFRAMEEDITBOX     = 3004,
    UIE_TEXTUREPATHSTATICTEXT       = 3005,
    UIE_TEXTUREPATHEDITBOX          = 3006
};

enum UserInterfaceCommands
{
    UIC_FILE_LOAD                   = 1000,
    UIC_FILE_QUIT                   = 1001,
    UIC_FILE_LOAD_TEXTURE           = 1002,
    UIC_VIEW_WIREFRAME              = 2001,
    UIC_VIEW_LIGHTING               = 2002,
    UIC_VIEW_TEXTURE_INTERPOLATION  = 2003
};

class UserInterface : public irr::IEventReceiver
{
private:
    Engine *m_Engine;
    irr::gui::IGUIEnvironment *m_Gui;
    irr::gui::CGUITTFont *m_GuiFont;
    irr::gui::CGUITTFace *m_GuiFontFace;

    void setupUserInterface();
    void displayLoadFileDialog();
    void displayLoadTextureDialog();
    void handleMenuItemPressed(irr::gui::IGUIContextMenu *menu);

    bool m_WireframeDisplay;
    bool m_Lighting;
    bool m_TextureInterpolation;
    irr::gui::IGUIWindow *playbackWindow;
public:
    irr::gui::IGUIContextMenu *menu;
    irr::gui::IGUIContextMenu *fileMenu;
    irr::gui::IGUIContextMenu *viewMenu;
    irr::gui::IGUIButton *playbackStartStopButton;
    irr::gui::IGUIButton *playbackIncreaseButton;
    irr::gui::IGUIButton *playbackDecreaseButton;
    irr::gui::IGUIEditBox *playbackSetFrameEditBox;
    irr::gui::IGUIStaticText *texturePathStaticText;
    irr::gui::IGUIEditBox *texturePathEditBox;
    irr::u32 INDEX_VIEW_TEXTURE_INTERPOLATION;
    irr::u32 INDEX_VIEW_WIREFRAME_MESH;
    irr::u32 INDEX_VIEW_LIGHTING;


    UserInterface( Engine *device );
    ~UserInterface();
    irr::gui::IGUIEnvironment *getGUIEnvironment() const;
    void drawStatusLine() const;
    bool loadNextTexture(int direction);

    // IEventReceiver
    virtual bool OnEvent( const irr::SEvent &event );
};

#endif // USERINTERFACE_H
