#include <stdlib.h>
#include <getopt.h>

bool CreateOpenGL(int *pWidth, int *pHeight, int MSAASamples);
void DestroyOpenGL();
bool Initialize(int argc, char *argv[]);
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
        Render((unsigned)g_FBWidth, (unsigned)g_FBHeight);
    }
}

//
// main
//
int main(int argc, char *argv[])
{
    const char *optstring = "m:";
    char m_arg[80];

    m_arg[0] = '\0';

    int opt = getopt(argc, argv, optstring);
    while (opt != -1)
    {
        switch (opt)
        {
        case 'm':
            strcpy(m_arg, optarg);
            break;
        }

        opt = getopt(argc, argv, optstring);
    }

    int MSAASamples = (m_arg[0] ? atoi(m_arg) : 1);

    if (CreateOpenGL(&g_FBWidth, &g_FBHeight, MSAASamples))
    {
        if (Initialize(argc, argv))
        {
            //ShowAppWindow();
            RunLoop();
        }

        Cleanup();
        DestroyOpenGL();
    }

    return 0;
}
