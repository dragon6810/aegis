#pragma once
class Game
{
public:
	static Game& GetGame()
	{
		static Game instance;
		return instance;
	}

	// Heartbeat
	void Init();
	void Tick();
	void Render();

	// Utils
	float Time(); // Time, in seconds since start of program
	float R_Random(float min, float max); // Use for things like rendering, client based stuff
private:
	Game() {}

	Game(Game const&);
	void operator=(Game const&);

	long long start;

	unsigned int r_seed = 1993;
};

