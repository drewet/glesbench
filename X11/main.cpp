#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <memory.h>

const unsigned WINDOW_WIDTH = 1280;//1920;
const unsigned WINDOW_HEIGHT = 720;//1080;
const char *WINDOW_CAPTION = "OpenGL App";

bool CreateOpenGL(unsigned Width, unsigned Height);
void DestroyOpenGL();
bool Initialize();
void Cleanup();
void Render(unsigned Width, unsigned Height);

//
// Globals
//
extern Display*     g_pDisplay;
extern Window       g_Window;

Atom                g_DeleteWindow;
Atom                g_Protocols;

//
// ShowAppWindow
//
void ShowAppWindow()
{
    int ScreenWidth = 0;
    int ScreenHeight = 0;

    int Event;
    int Error;

    // Assume Full HD resolution
    ScreenWidth = 1920;
    ScreenHeight = 1080;

    XSetStandardProperties(
        g_pDisplay,
        g_Window,
        WINDOW_CAPTION,
        WINDOW_CAPTION,
        None,
        NULL,
        0,
        NULL
       );

    XSizeHints Hints;

    // Fixed window size.
    Hints.flags = PMinSize | PMaxSize;
    Hints.min_width = Hints.max_width = WINDOW_WIDTH;
    Hints.min_height = Hints.max_height = WINDOW_HEIGHT;

    XSetWMNormalHints(g_pDisplay, g_Window, &Hints);

    // Catch [x] UI element click
    g_DeleteWindow = XInternAtom(g_pDisplay, "WM_DELETE_WINDOW", True);
    g_Protocols = XInternAtom(g_pDisplay, "WM_PROTOCOLS", True);

    XSetWMProtocols(g_pDisplay, g_Window, &g_DeleteWindow, 1);

    XWindowChanges Changes;

    memset(&Changes, 0, sizeof(Changes));
    if ((WINDOW_WIDTH < ScreenWidth) &&
        (WINDOW_HEIGHT < ScreenHeight))
    {
        Changes.x = (ScreenWidth - WINDOW_WIDTH) / 2;
        Changes.y = (ScreenHeight - WINDOW_HEIGHT) / 2;
    }

    XMapRaised(g_pDisplay, g_Window);
    XConfigureWindow(g_pDisplay, g_Window, CWX | CWY, &Changes);

    XFlush(g_pDisplay);
}

//
// EventLoop
//
void EventLoop()
{
    while (true)
    {
        while (XPending(g_pDisplay))
        {
            XEvent Event;

            XNextEvent(g_pDisplay, &Event);
            switch (Event.type)
            {
            case KeyPress:
                {
                    //KeySym Code = XKeycodeToKeysym(m_Display, Event.xkey.keycode, 0);
                    //OnKeyDown(TranslateKey(Code), 0, 0);
                }
                break;

            case KeyRelease:
                {
                    //KeySym Code = XKeycodeToKeysym(m_Display, Event.xkey.keycode, 0);
                    //OnKeyUp(TranslateKey(Code), 0, 0);
                }
                break;

            case MotionNotify:
                //OnMouseMove(Event.xmotion.x, Event.xmotion.y);
                break;

            case ClientMessage:
                // Catch [x] UI element click.
                if (Event.xclient.message_type == g_Protocols)
                {
                    if (Event.xclient.data.l[ 0 ] == (long)g_DeleteWindow)
                        return;
                }
                break;
            }
        } // end of while (XPending())

        Render(WINDOW_WIDTH, WINDOW_HEIGHT);
    };
}

//
// main
//
int main(int argc, char *argv[])
{
    if (CreateOpenGL(WINDOW_WIDTH, WINDOW_HEIGHT))
    {
        if (Initialize())
        {
            ShowAppWindow();
            EventLoop();
        }

        Cleanup();
        DestroyOpenGL();
    }

    return 0;
}
