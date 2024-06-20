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
	float Time();
private:
	Game() {}

	Game(Game const&);
	void operator=(Game const&);

	long long start;
};

