#include"stdafx.h"
#include "Game.h"

void Game::initWindow()
{
	this->window.create(sf::VideoMode(1366, 768), "Dead Paradise", sf::Style::Close | sf::Style::Titlebar);
	this->window.setFramerateLimit(60);
	sf::View view = this->window.getDefaultView();
	view.setCenter(1366 / 2, 768 / 2);
	this->window.setView(view);
}

void Game::initInput()
{
	//Mouse bindings
	this->mouseMappings["BTN_ADD_TILE"] = sf::Mouse::Button::Left;
	this->mouseMappings["BTN_REMOVE_TILE"] = sf::Mouse::Button::Right;

	//Keyboard bindings
	this->keyboardMappings["KEY_MOVE_LEFT"] = sf::Keyboard::Key::A;
	this->keyboardMappings["KEY_MOVE_RIGHT"] = sf::Keyboard::Key::D;
	this->keyboardMappings["KEY_MOVE_UP"] = sf::Keyboard::Key::W;
	this->keyboardMappings["KEY_MOVE_DOWN"] = sf::Keyboard::Key::S;
	this->keyboardMappings["KEY_JUMP"] = sf::Keyboard::Key::Space;
}

void Game::initTileSheet()
{
	if (!this->tileSheet.loadFromFile("Textures/tile_sheet.png"))
	{
		std::cout << "ERROR::GAME::Could not load the tile sheet!" << "\n";
	}
}

void Game::initPlayer()
{
	this->player = new Player();
}

void Game::initTileMap()
{
	this->tileMap = new TileMap(200, 20, &this->tileSheet, 50);
}

Game::Game()
{
	this->initWindow();
	this->initInput();
	this->initTileSheet();
	this->initPlayer();
	this->initTileMap();
}

Game::~Game()
{
	delete this->player;
	delete this->tileMap;
}

void Game::updateView()
{
	// Obtenemos la vista actual
	sf::View view = this->window.getDefaultView();

	// Calculamos la posici�n x del jugador (centro)
	float playerCenterX = this->player->getPosition().x + this->player->getGlobalBounds().width / 2.f;

	// Calculamos la posici�n y inicial de la vista (no cambia durante los saltos)
	float viewCenterY = view.getCenter().y;

	// Actualizamos solo la coordenada X de la vista para seguir al jugador horizontalmente
	view.setCenter(playerCenterX, viewCenterY);

	// Aplicamos la vista actualizada
	this->window.setView(view);
}


void Game::updateInput()
{
	// Convertimos la posici�n del mouse a coordenadas del juego
	sf::Vector2i mousePos = sf::Mouse::getPosition(this->getWindow());
	sf::Vector2f worldPos = this->window.mapPixelToCoords(mousePos);

	// Calculamos la posici�n del tile donde est� el mouse
	const int mouseX = int(worldPos.x) / int(this->tileMap->getTileSize());
	const int mouseY = int(worldPos.y) / int(this->tileMap->getTileSize());

	// Movimiento del jugador
	if (sf::Keyboard::isKeyPressed(this->keyboardMappings["KEY_MOVE_LEFT"]))
	{
		this->player->move(-1.f, 0.f);
	}
	else if (sf::Keyboard::isKeyPressed(this->keyboardMappings["KEY_MOVE_RIGHT"]))
	{
		this->player->move(1.f, 0.f);
	}

	if (sf::Keyboard::isKeyPressed(this->keyboardMappings["KEY_JUMP"]) && this->player->getCanJump())
	{
		this->player->jump();
	}

	// Funciones de los tiles
	if (sf::Mouse::isButtonPressed(this->mouseMappings["BTN_ADD_TILE"]))
	{
		this->tileMap->addTile(mouseX, mouseY);
	}
	else if (sf::Mouse::isButtonPressed(this->mouseMappings["BTN_REMOVE_TILE"]))
	{
		this->tileMap->removeTile(mouseX, mouseY);
	}
}


void Game::updatePlayer()
{
	this->player->update();
}

void Game::updateCollision()
{
	//Collision bottom of screen
	if (this->player->getPosition().y + this->player->getGlobalBounds().height > this->window.getSize().y)
	{
		this->player->setCanJump(true);
		this->player->resetVelocityY();
		this->player->setPosition(
			this->player->getPosition().x,
			this->window.getSize().y - this->player->getGlobalBounds().height
		);
	}
}

void Game::updateTileMap()
{
	this->tileMap->update();
}

void Game::update()
{
	//Polling window events
	while (this->window.pollEvent(this->ev))
	{
		if (this->ev.type == sf::Event::Closed)
			this->window.close();
		else if (this->ev.type == sf::Event::KeyPressed && this->ev.key.code == sf::Keyboard::Escape)
			this->window.close();
	}

	this->updateInput();

	this->updatePlayer();

	this->updateCollision();

	this->updateTileMap();

	this->updateView();
}

void Game::renderPlayer()
{
	this->player->render(this->window);
}

void Game::renderTileMap()
{
	this->tileMap->render(this->window);
}

void Game::render()
{
	this->window.clear();


	this->renderTileMap();
	this->renderPlayer();

	this->window.display();
}

const sf::RenderWindow& Game::getWindow() const
{
	// TODO: insert return statement here
	return this->window;
}
