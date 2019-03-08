#ifndef USERINTERFACE_H
#define USERINTERFACE_H

// Forward declaration of class Engine
class Engine;

#include <sstream>
#include <string>
#include <irrlicht/irrlicht.h>

#include "Debug.h"
#include "Engine.h"

#include "extlib/CGUITTFont.h"

enum UserInterfaceElements
{
    UIE_PLAYBACKWINDOW              = 1000,
    UIE_LOADBUTTON                  = 1001,
    UIE_LOADFILEDIALOG              = 1002,
    UIE_FILEMENU                    = 1003,
    UIE_PLAYBACKSTARTSTOPBUTTON     = 1004,
    UIE_VIEWMENU                    = 1005,
    UIE_LOADTEXTUREDIALOG           = 1006,
    UIE_PLAYBACKINCREASEBUTTON      = 1007,
    UIE_PLAYBACKDECREASEBUTTON      = 1008
};

enum UserInterfaceCommands
{
    UIC_FILE_LOAD           = 1000,
    UIC_FILE_QUIT           = 1001,
    UIC_FILE_LOAD_TEXTURE   = 1002,
    UIC_VIEW_WIREFRAME      = 2000,
    UIC_VIEW_LIGHTING       = 2001
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

    bool KeyIsDown[irr::KEY_KEY_CODES_COUNT];
    irr::s32 keyState[irr::KEY_KEY_CODES_COUNT];
    irr::s32 LMouseState,RMouseState;

public:
    irr::gui::IGUIButton *playbackStartStopButton;
    irr::gui::IGUIButton *playbackIncreaseButton;
    irr::gui::IGUIButton *playbackDecreaseButton;

    UserInterface( Engine *device );
    ~UserInterface();
    irr::gui::IGUIEnvironment *getGUIEnvironment() const;
    void drawStatusLine() const;

    // IEventReceiver
    virtual bool OnEvent( const irr::SEvent &event );
};

#endif // USERINTERFACE_H
