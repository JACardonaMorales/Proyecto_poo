#include "stdafx.h"
#include "Physics.h"

b2WorldDef Physics::world{ b2Vec2(0.0f, -9.81f), // Gravedad
	true // Allow sleep
};

void Physics::Init()
{
	b2WorldId worldId = b2CreateWorld(&world);

}

void Physics::Update(float deltaTime)
{// Actualizar el mundo de Box2D
	
}
