#include "stdafx.h"
#include "Game.h"


//Funciones Estaticas

//Incializadores

void Game::initVariables()
{

}

void Game::initWindow()
{
	unsigned int framerateLimit = 120;
	this->window.create(sf::VideoMode::getDesktopMode(), "Dead Paradise", sf::Style::Close | sf::Style::Titlebar);
	this->window.setFramerateLimit(framerateLimit);
}


void Game::initPlayer()
{
	this->player = new Player();
}

void Game::initInput()
{

}


//Constructores&Destructores


Game::Game()
{
	this->initWindow();
}

Game::~Game()
{
	delete this->player;
}

//Funciones

void Game::updateSFMLEvents()
{

}

void Game::updatePlayer()
{
	this->player->update();

}

void Game::update()
{
	//Polling Window Events
	while (this->window.pollEvent(this->ev))
	{
		if (this->ev.type == sf::Event::Closed)
			this->window.close();

		else if (this->ev.type == sf::Event::KeyPressed && this->ev.key.code == sf::Keyboard::Escape)
			this->window.close();
	}
	this->updatePlayer();
}

const sf::RenderWindow& Game::getWindow() const
{
	return this->window;
}

void Game::render()
{
	this->window.clear();

	// Renderiza al jugador
	if (this->player)
		this->player->render(this->window);

	this->window.display();
}


void Game::run()
{
	
	
}
