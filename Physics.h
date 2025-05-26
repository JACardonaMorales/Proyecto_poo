#pragma once
class Physics
{
public:
	static void Init();
	static void Update(float deltaTime);
private:
	static b2WorldDef world;
};

