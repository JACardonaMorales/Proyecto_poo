#pragma once
#include "Player.h"

class Game
{
private:
	sf::RenderWindow window;
	sf::Event ev;

	Player *player;

	void initVariables();
	void initWindow();
	void initPlayer();
	void initInput();

public:
	Game();
	virtual ~Game();

	void updateSFMLEvents();
	void updatePlayer();
	void update();
	void render();
	const sf::RenderWindow& getWindow() const;
	void run();

	void renderPlayer();

};