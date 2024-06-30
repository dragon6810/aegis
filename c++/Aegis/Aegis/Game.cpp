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

    float d = 60;
    camp = { d, d, d };
    camf = { 0, 0, 0 };

    camera.position = camp;
    //camera.position = { 0.0, 0.0, 0.0 };
    camera.rotation = { 0, -45.0 * DEG2RAD, 135.0 * DEG2RAD };
   // camera.rotation = { 0.0, 0.0, 0.0 };
    camera.vfov = 65.0;
    camera.aspect = 4.0f / 3.0f;
    camera.ortho = false;
    camera.ReconstructMatrices();
    
    map.Load("valve/maps/test.bsp");
    map.SetCameraPosition({ camp.x, camp.y, camp.z });
    map.cameraforward = NormalizeVector3({ camf.x - camp.x, camf.y - camp.y, camf.z - camp.z });
    map.cameraup = { 0, 0, 1 };
    map.sky.campos = camp;

    font.Load("FONT1", "valve/fonts.wad");

    Mpeg3::LoadCD("valve/media/Half-Life02.mp3");

    long long lastFrame = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    window->SetKeyCallback(Game::KeyCallback);
    window->SetCursorPosCallback(Game::CursorCallback);

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

        glfwSwapInterval(0);
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
    map.Think(1.0 / fps);
}

void Game::Render()
{
    camera.ReconstructMatrices();
    
    window->MakeFullscreenViewport(camera.aspect);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(camera.vfov, camera.aspect, 1.0, 10000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.position.x, camera.position.y, camera.position.z,
        camera.position.x + camera.forward.x, camera.position.y + camera.forward.y, camera.position.z + camera.forward.z,
        0.0, 0.0, 1.0);

    glClear(GL_DEPTH_BUFFER_BIT);
    
    map.Draw();

    glDisable(GL_DEPTH_TEST);

    glPointSize(15.0f);

    glBegin(GL_POINTS);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glEnd();

    vec3_t p;
    vec3_t dir = camera.DirFromScreen(cursorpos) * 32.0;

    glColor3f(1, 0, 1);
    glBegin(GL_LINES);
    glVertex3f(camera.position.x, camera.position.y, camera.position.z);
    glVertex3f(camera.position.x + dir.x, camera.position.y + dir.y, camera.position.z + dir.z);
    glEnd();

    if (map.FineRaycast(camera.position, camera.position + (dir * 64.0), &p))
    {
        glBegin(GL_POINTS);
        glColor3f(0, 0, 1);
        glVertex3f(p.x, p.y, p.z);
        glEnd();
    }
    glColor3f(1, 1, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, SCREEN_MED_WIDTH, 0.0, SCREEN_MED_HEIGHT, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glColor4f(1, 1, 1, 0.5);
    font.DrawString(std::to_string((int) fps) + std::string(" FPS"), 0, SCREEN_MED_HEIGHT - font.GetHeight());
    glColor4f(1, 1, 1, 1);

    glEnable(GL_DEPTH_TEST);
}

Renderer* Game::GetRenderer()
{
	return &renderer;
}

AudioManager* Game::GetAudioManager()
{
    return &audiomanager;
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

void Game::CursorCallback(GLFWwindow* window, double xpos, double ypos)
{
    int width;
    int height;
    Game::GetGame().window->GetWindowDimensions(&width, &height);

    float windowAspect = (float)width / (float)height;
    float contentAspect = (float)SCREEN_HIGH_WIDTH / (float)SCREEN_HIGH_HEIGHT;

    float contentWidth;
    float contentHeight;
    float xOffset = 0.0f;
    float yOffset = 0.0f;

    if (windowAspect > contentAspect) 
    {
        contentHeight = height;
        contentWidth = height * contentAspect;
        xOffset = (width - contentWidth) / 2.0f;
    }
    else 
    {
        contentWidth = width;
        contentHeight = width / contentAspect;
        yOffset = (height - contentHeight) / 2.0f;
    }

    vec2_t pos = { (float)(xpos - xOffset) / contentWidth, (float)(ypos - yOffset) / contentHeight };

    if (pos.x > 1.0)
        pos.x = 1.0;
    if (pos.x < 0.0)
        pos.x = 0.0;

    if (pos.y > 1.0)
        pos.y = 1.0;
    if (pos.y < 0.0)
        pos.y = 0.0;

    Game::GetGame().cursorpos = pos;
}
