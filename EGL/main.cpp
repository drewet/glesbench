#include <stdlib.h>
#include <getopt.h>

bool CreateOpenGL(int *pWidth, int *pHeight, int MSAASamples);
void DestroyOpenGL();
bool Initialize();
void Cleanup();
void Render(unsigned Width, unsigned Height);

//
// Globals
//
int                 g_FBWidth               = 0;
int                 g_FBHeight              = 0;

//
// RunLoop
//
void RunLoop()
{
    while (true)
    {
        // Render our scene
        Render(g_FBWidth, g_FBHeight);
    }
}

//
// main
//
int main(int argc, char *argv[])
{
    const char *optstring = "m:";
    int opt = getopt(argc, argv, optstring);
    const char *m_arg = NULL;

    while (opt != -1)
    {
        switch (opt)
        {
        case 'm':
            // MSAA
            m_arg = optarg;
            break;
        }

        opt = getopt(argc, argv, optstring);
    }

    int MSAASamples = (m_arg ? atoi(m_arg) : 1);

    if (CreateOpenGL(&g_FBWidth, &g_FBHeight, MSAASamples))
    {
        if (Initialize())
        {
            //ShowAppWindow();
            RunLoop();
        }

        Cleanup();
        DestroyOpenGL();
    }

    return 0;
}
