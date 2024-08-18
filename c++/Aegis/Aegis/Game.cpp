#include "Game.h"

#include <chrono>

#include "mathutils.h"

#include "Targa.h"

#include "AssetManager.h"
#include "Waveform.h"

#include "Quaternion.h"

void Game::Main(int argc, char** argv)
{
    start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	renderer.Init();
	window = new Window("Aegis", SCREEN_MED_WIDTH, SCREEN_MED_HEIGHT, false);
	window->SelectForRendering();
    renderer.PostWindowInit();
	int fullwidth;
	int fullheight;
	window->GetWindowDimensions(&fullwidth, &fullheight);
	glViewport(0, 0, fullwidth, fullheight);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

    gui.Reload();

    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);  // Enable depth test
    glDepthFunc(GL_LEQUAL);   // Specify the depth function

    camera.position = { 100, 100, 100 };
    camera.rotation = { 0, -45.0 * DEG2RAD, 135.0 * DEG2RAD };
    camera.vfov = 90.0;
    camera.aspect = 4.0f / 3.0f;
    camera.ortho = false;
    camera.ReconstructMatrices();

    for (int i = 1; i < argc; i += 2)
    {
        std::string key, val;

        key = argv[i];
        val = argv[i + 1];

        console.Execute(key + " " + val);
    }

    long long lastFrame = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    window->SetKeyCallback(Game::KeyCallback);
    window->SetCursorPosCallback(Game::CursorCallback);
    window->SetMouseBtnCallback(Game::MouseBtnCallback);

    float lastcheck = -1.0;

    while (!window->ShouldWindowClose())
    {
        long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        long long delta = now - lastFrame;
        lastFrame = now;

        cursorshape = GLFW_ARROW_CURSOR;

        gui.MouseDrag(cursorpos.x * SCREEN_MED_WIDTH, SCREEN_MED_HEIGHT - cursorpos.y * SCREEN_MED_HEIGHT);

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

        // Presentation
        window->SetCursor(cursorshape);
        window->SwapBuffers();
        glfwPollEvents();
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
          camera.up.x, camera.up.y, camera.up.z);

    glClear(GL_DEPTH_BUFFER_BIT);

    map.Draw();
    
    glColor3f(0, 0, 1);
    vec3_t p, n;
    vec3_t start = camera.position;
    vec3_t end = start + (camera.DirFromScreen(cursorpos) * 2048.0);
    if (map.FineRaycast(start, end, &p, &n, 0))
    {
        glPointSize(10000.0 / Vector3Dist(start, p));
        glBegin(GL_POINTS);
        glVertex3f(p.x, p.y, p.z);
        glEnd();
    }

    glPointSize(15.0f);

    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, SCREEN_MED_WIDTH, 0.0, SCREEN_MED_HEIGHT, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3ub(255, 255, 255);

    gui.DrawScreen();

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

float Game::P_Random(float min, float max)
{
    p_seed = (p_seed * 1664525 + 1013904223) & 0xFFFFFFFF;
    
    float normalized = static_cast<float>(p_seed) / 0xFFFFFFFF;
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

void Game::MouseBtnCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        Game::GetGame().gui.MouseDown(Game::GetGame().cursorpos.x * SCREEN_MED_WIDTH, SCREEN_MED_HEIGHT - Game::GetGame().cursorpos.y * SCREEN_MED_HEIGHT);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        Game::GetGame().gui.MouseUp(Game::GetGame().cursorpos.x * SCREEN_MED_WIDTH, SCREEN_MED_HEIGHT - Game::GetGame().cursorpos.y * SCREEN_MED_HEIGHT);
}
