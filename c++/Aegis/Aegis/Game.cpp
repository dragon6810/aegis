#include "Game.h"

#include <chrono>

#include "mathutils.h"

#include "AssetManager.h"

void Game::Main()
{
	renderer.Init();
	window = new Window("Aegis", SCREEN_HIGH_WIDTH, SCREEN_HIGH_HEIGHT, true);
	window->SelectForRendering();
    renderer.PostWindowInit();
	int fullwidth;
	int fullheight;
	window->GetWindowDimensions(&fullwidth, &fullheight);
	glViewport(0, 0, fullwidth, fullheight);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	window->MakeFullscreenViewport((float) SCREEN_HIGH_WIDTH / (float) SCREEN_HIGH_HEIGHT);

    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);  // Enable depth test
    glDepthFunc(GL_LEQUAL);   // Specify the depth function

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65, (float)SCREEN_HIGH_WIDTH / (float)SCREEN_HIGH_HEIGHT, 1.0, 10000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float d = 300;
    vec3_t camp = { d, d, d };
    vec3_t camf = { 0, 0, 75 };
    gluLookAt(camp.x, camp.y, camp.z,
        camf.x, camf.y, camf.z,
        0.0, 0.0, 1.0);
    
    wad.Load("valve/halflife.wad");

    map.Load("valve/maps/test.bsp");
    map.SetCameraPosition({ camp.x, camp.y, camp.z });
    map.cameraforward = NormalizeVector3({ camf.x - camp.x, camf.y - camp.y, camf.z - camp.z });
    map.cameraup = { 0, 0, 1 };
    map.sky.campos = camp;

    long long lastFrame = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    while (!window->ShouldWindowClose())
    {
        long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        long long delta = now - lastFrame;
        float fps = 1.0 / ((float)delta / 1000.0);

        //printf("%d FPS.\n", (int)fps);

        glClear(GL_DEPTH_BUFFER_BIT);

        //map.sky.Render();

        map.Think(1.0 / fps);
        map.Draw();

        window->SwapBuffers();
        glfwPollEvents();

        lastFrame = now;
    }

    window->Kill();
    glfwTerminate();

    AssetManager::getInst().cleanup();
}

void Game::Tick()
{

}

void Game::Render()
{

}

Renderer* Game::GetRenderer()
{
	return &renderer;
}

float Game::Time()
{
	long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	float delta = (float)(now - start) / 1000.0;
	return delta;
}

float Game::R_Random(float min, float max)
{
	r_seed = (r_seed * 1664525 + 1013904223) & 0xFFFFFFFF;

	float normalized = static_cast<float>(r_seed) / 0xFFFFFFFF;
	return min + normalized * (max - min);
}