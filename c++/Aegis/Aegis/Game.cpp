#include "Game.h"

#include <chrono>

#include "mathutils.h"

#include "AssetManager.h"
#include "Waveform.h"

#include "Quaternion.h"

void Game::Main()
{
	renderer.Init();
	window = new Window("Aegis", SCREEN_HIGH_WIDTH, SCREEN_HIGH_HEIGHT, false);
	window->SelectForRendering();
    renderer.PostWindowInit();
	int fullwidth;
	int fullheight;
	window->GetWindowDimensions(&fullwidth, &fullheight);
	glViewport(0, 0, fullwidth, fullheight);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);  // Enable depth test
    glDepthFunc(GL_LEQUAL);   // Specify the depth function

    float d = 300;
    camp = { d, d, d };
    camf = { 0, 0, 0 };

    camera.position = camp;
    camera.rotation = { 0, -60.0 * DEG2RAD, -45.0 * DEG2RAD };
    camera.ReconstructMatrices();
    
    wad.Load("valve/halflife.wad");

    map.Load("valve/maps/test.bsp");
    map.SetCameraPosition({ camp.x, camp.y, camp.z });
    map.cameraforward = NormalizeVector3({ camf.x - camp.x, camf.y - camp.y, camf.z - camp.z });
    map.cameraup = { 0, 0, 1 };
    map.sky.campos = camp;

    font.Load("FONT0", "valve/fonts.wad");

    long long lastFrame = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    window->SetKeyCallback(Game::KeyCallback);

    audiomanager.PlaySound("valve/sound/barney/ba_bring.wav", 1);

    float lastcheck = -1.0;

    while (!window->ShouldWindowClose())
    {
        long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        long long delta = now - lastFrame;

        if (this->Time() - lastcheck >= 1.0)
        {
            lastcheck = this->Time();
            fps = 1.0 / ((float)delta / 1000.0);
        }

        // Don't tick while paused
        if(!paused)
        {
            tickinterp += (float) delta / 1000.0 * ENGINE_TICKRATE;

            while (tickinterp >= 1.0)
            {
                Tick();
                tickinterp -= 1.0;
            }
        }

        Render();

        lastFrame = now;
    }

    window->Kill();
    glfwTerminate();

    AssetManager::getInst().cleanup();
}

void Game::Tick()
{
    map.Think(1.0 / fps);
}

void Game::Render()
{
    window->MakeFullscreenViewport((float)SCREEN_HIGH_WIDTH / (float)SCREEN_HIGH_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30, (float)SCREEN_HIGH_WIDTH / (float)SCREEN_HIGH_HEIGHT, 1.0, 10000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.position.x, camera.position.y, camera.position.z,
        camera.position.x + camera.forward.x, camera.position.y + camera.forward.y, camera.position.z + camera.forward.z,
        0.0, 0.0, 1.0);

    glClear(GL_DEPTH_BUFFER_BIT);
    
    map.Draw();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, SCREEN_MED_WIDTH, 0.0, SCREEN_MED_HEIGHT, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    glColor4f(1, 1, 1, 0.5);
    font.DrawString(std::to_string((int) fps) + std::string("FPS"), 0, SCREEN_MED_HEIGHT - font.GetHeight());
    glColor4f(1, 1, 1, 1);

    glEnable(GL_DEPTH_TEST);

    window->SwapBuffers();
    glfwPollEvents();
}

Renderer* Game::GetRenderer()
{
	return &renderer;
}

bool Game::IsPaused()
{
    return paused;
}

void Game::Pause()
{
    paused = true;
}

void Game::Unpause()
{
    paused = false;
}

void Game::TogglePause()
{
    if (paused)
        Unpause();
    else
        Pause();
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

void Game::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == CONTROLS_PAUSE && action == GLFW_PRESS)
        Game::GetGame().TogglePause();
}