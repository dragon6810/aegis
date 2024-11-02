#include "Input.h"

#include "Game.h"

std::unordered_map<int, std::string> Input::keycommands;

const std::unordered_map<std::string, int> Input::stringkeycodes =
{
	{ "escape", GLFW_KEY_ESCAPE },
	{ "f1", GLFW_KEY_F1 },
	{ "f2", GLFW_KEY_F2 },
	{ "f3", GLFW_KEY_F3 },
	{ "f4", GLFW_KEY_F4 },
	{ "f5", GLFW_KEY_F5 },
	{ "f6", GLFW_KEY_F6 },
	{ "f7", GLFW_KEY_F7 },
	{ "f8", GLFW_KEY_F8 },
	{ "f9", GLFW_KEY_F9 },
	{ "f10", GLFW_KEY_F10 },
	{ "f11", GLFW_KEY_F11 },
	{ "f12", GLFW_KEY_F12 },
	{ "printscreen", GLFW_KEY_PRINT_SCREEN },
	{ "scrolllock", GLFW_KEY_SCROLL_LOCK },
	{ "pause", GLFW_KEY_PAUSE },
	{ "grave", GLFW_KEY_GRAVE_ACCENT },
	{ "1", GLFW_KEY_1 },
	{ "2", GLFW_KEY_2 },
	{ "3", GLFW_KEY_3 },
	{ "4", GLFW_KEY_4 },
	{ "5", GLFW_KEY_5 },
	{ "6", GLFW_KEY_6 },
	{ "7", GLFW_KEY_7 },
	{ "8", GLFW_KEY_8 },
	{ "9", GLFW_KEY_9 },
	{ "0", GLFW_KEY_0 },
	{ "-", GLFW_KEY_MINUS },
	{ "=", GLFW_KEY_EQUAL },
	{ "backspace", GLFW_KEY_BACKSPACE },
	{ "insert", GLFW_KEY_INSERT },
	{ "home", GLFW_KEY_HOME },
	{ "pageup", GLFW_KEY_PAGE_UP },
	{ "numlock", GLFW_KEY_NUM_LOCK },
	{ "tab", GLFW_KEY_TAB },
	{ "q", GLFW_KEY_Q },
	{ "w", GLFW_KEY_W },
	{ "e", GLFW_KEY_E },
	{ "r", GLFW_KEY_R },
	{ "t", GLFW_KEY_T },
	{ "y", GLFW_KEY_Y },
	{ "u", GLFW_KEY_U },
	{ "i", GLFW_KEY_I },
	{ "o", GLFW_KEY_O },
	{ "p", GLFW_KEY_P },
	{ "[", GLFW_KEY_LEFT_BRACKET },
	{ "]", GLFW_KEY_RIGHT_BRACKET },
	{ "\\", GLFW_KEY_BACKSLASH },
	{ "delete", GLFW_KEY_DELETE},
	{ "end", GLFW_KEY_END },
	{ "pagedown", GLFW_KEY_PAGE_DOWN },
	{ "capslock", GLFW_KEY_CAPS_LOCK },
	{ "a", GLFW_KEY_A },
	{ "s", GLFW_KEY_S },
	{ "d", GLFW_KEY_D },
	{ "f", GLFW_KEY_F },
	{ "g", GLFW_KEY_G },
	{ "h", GLFW_KEY_H },
	{ "j", GLFW_KEY_J },
	{ "k", GLFW_KEY_K },
	{ "l", GLFW_KEY_L },
	{ "semicolon", GLFW_KEY_SEMICOLON },
	{ "'", GLFW_KEY_APOSTROPHE },
	{ "enter", GLFW_KEY_ENTER },
	{ "lshift", GLFW_KEY_LEFT_SHIFT },
	{ "z", GLFW_KEY_Z },
	{ "x", GLFW_KEY_X },
	{ "c", GLFW_KEY_C },
	{ "v", GLFW_KEY_V },
	{ "b", GLFW_KEY_B },
	{ "n", GLFW_KEY_N },
	{ "m", GLFW_KEY_M },
	{ ",", GLFW_KEY_COMMA },
	{ ".", GLFW_KEY_PERIOD },
	{ "/", GLFW_KEY_SLASH },
	{ "rshift", GLFW_KEY_RIGHT_SHIFT },
	{ "up", GLFW_KEY_UP },
	{ "lcontrol", GLFW_KEY_LEFT_CONTROL },
	{ "lalt", GLFW_KEY_LEFT_ALT },
	{ "space", GLFW_KEY_SPACE },
	{ "ralt", GLFW_KEY_RIGHT_ALT },
	{ "rcontrol", GLFW_KEY_RIGHT_CONTROL },
	{ "left", GLFW_KEY_LEFT },
	{ "down", GLFW_KEY_DOWN },
	{ "right", GLFW_KEY_RIGHT },
};

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	std::string cmd;

	if(Game::GetGame().console.IsDown())
	{
		if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			Game::GetGame().console.Toggle();
		else if(key == GLFW_KEY_ENTER && action == GLFW_PRESS)
			Game::GetGame().console.KeyInput('\n');
		else if(key == GLFW_KEY_LEFT && action != GLFW_RELEASE)
			Game::GetGame().console.DecCursor();
		else if(key == GLFW_KEY_RIGHT && action != GLFW_RELEASE)
			Game::GetGame().console.IncCursor();
		else if(key == GLFW_KEY_BACKSPACE && action != GLFW_RELEASE)
			Game::GetGame().console.Backspace();
		else if(key == GLFW_KEY_DELETE && action != GLFW_RELEASE)
			Game::GetGame().console.Delete();
		
		return;
	}

	if (action == GLFW_REPEAT)
		return;

	if (keycommands.find(key) == keycommands.end())
		return;

	cmd = keycommands[key];
	
	if (cmd[0] == '+')
	{
		if (action == GLFW_RELEASE)
			cmd[0] = '-';
	}
	else
	{
		if (action == GLFW_RELEASE)
			return;
	}

	Game::GetGame().ParseCommands(cmd);
}

void Input::CharCallback(GLFWwindow* window, unsigned int codepoint)
{
	if(codepoint > 128)
	{
		Console::Print("Aegis currently only supports ASCII characters!\n");
		return;
	}

	if(!Game::GetGame().console.IsDown())
		return;

	Game::GetGame().console.KeyInput((char) codepoint);
}