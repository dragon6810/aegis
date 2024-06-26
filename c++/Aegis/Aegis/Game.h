#pragma once

#define ENGINE_TICKRATE 50
#define ENGINE_TICKDUR 1.0 / ENGINE_TICKRATE

#define SCREEN_HIGH_WIDTH 1152
#define SCREEN_HIGH_HEIGHT 864

#define SCREEN_MED_WIDTH 640
#define SCREEN_MED_HEIGHT 480

#define SCREEN_LOW_WIDTH 320
#define SCREEN_LOW_HEIGHT 240

#include <GL/glew.h>
#include "Renderer.h"
#include "Window.h"

#include "AudioManager.h"

#include "Wad.h"
#include "BSPMap.h"

#include "Camera.h"

#include "Font.h"

#include "defs.h"

#define CONTROLS_PAUSE GLFW_KEY_ESCAPE

class Game
{
public:
	static Game& GetGame()
	{
		static Game instance;
		return instance;
	}

	// Heartbeat
	void Main();
	void Tick();
	void Render();

	float tickinterp = 0.0; // The interpolation factor for each dick
	float fps;

	Camera camera;

	// Heartbeat getters/setters
	Renderer* GetRenderer();
	AudioManager* GetAudioManager();

	bool IsPaused();
	void Pause();
	void Unpause();
	void TogglePause();

	// Utils
	float Time(); // Time, in seconds since start of program
	float R_Random(float min, float max); // Use for things like rendering, client based stuff

	// UI
	Font font;

	// Boring GLFW shit
	vec2_t cursorpos{};

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void CursorCallback(GLFWwindow* window, double xpos, double ypos);
private:
	Game() {}

	Game(Game const&);
	void operator=(Game const&);

	long long start;

	unsigned int r_seed = 1993;

	AudioManager audiomanager;

	bool paused = false;

	Renderer renderer;
	Window* window;

	vec3_t camp;
	vec3_t camf;

	Wad wad;
	BSPMap map;
};

