//
// Globals
//
int g_FBWidth = 0;
int g_FBHeight = 0;

bool CreateOpenGL(int *pWidth, int *pHeight);
void DestroyOpenGL();
bool Initialize();
void Cleanup();
void Render(unsigned Width, unsigned Height);

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
    if (CreateOpenGL(&g_FBWidth, &g_FBHeight))
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
