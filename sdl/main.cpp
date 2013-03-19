#include "SDL.h"

#define HP_TOUCHPAD

#ifdef HP_TOUCHPAD
const unsigned WINDOW_WIDTH = 1024;
const unsigned WINDOW_HEIGHT = 768;
#else // TV, Desktop
const unsigned WINDOW_WIDTH = 1280;//1920;
const unsigned WINDOW_HEIGHT = 720;//1080;
#endif // TOUCHPAD
const char *WINDOW_CAPTION = "SDL/OpenGL App";

bool CreateSDL(unsigned Width, unsigned Height);
void DestroySDL();
bool Initialize();
void Cleanup();
void Render(unsigned Width, unsigned Height);

//
// ShowAppWindow
//
void ShowAppWindow()
{
    SDL_WM_SetCaption(WINDOW_CAPTION, WINDOW_CAPTION);
}

//
// EventLoop
//
void EventLoop()
{
    // Event descriptor
    SDL_Event Event;
    bool bPaused = false;

    while (true)
    {
        // Render our scene
        Render(WINDOW_WIDTH, WINDOW_HEIGHT);

        bool bGotEvent;

        if (bPaused)
        {
            SDL_WaitEvent(&Event);
            bGotEvent = true;
        }
        else
        {
            bGotEvent = SDL_PollEvent(&Event);
        }

        while (bGotEvent)
        {
            switch (Event.type)
            {
                // List of keys that have been pressed
                case SDL_KEYDOWN:
                    break;

                case SDL_ACTIVEEVENT:
                    if (SDL_APPACTIVE == Event.active.state)
                    {
                        bPaused = !Event.active.gain;
                    }
                    break;

                case SDL_QUIT:
                    return;

                default:
                    break;
            }

            bGotEvent = SDL_PollEvent(&Event);
        }
    }
}

//
// main
//
int main(int argc, char *argv[])
{
    if (CreateSDL(WINDOW_WIDTH, WINDOW_HEIGHT))
    {
        if (Initialize())
        {
            ShowAppWindow();
            EventLoop();
        }

        Cleanup();
        DestroySDL();
    }

    return 0;
}
