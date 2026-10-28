#pragma once
#include "Entity.h"

class Spawner : public Entity
{
private:

	float spawnTimer = 0;
	float spawnInterval = 3.0f;

	bool readyToSpawn = false;

public:

	bool getReadyToSpawn() const { return readyToSpawn; }
	void setReadyToSpawn(bool _readyToSpawn) { readyToSpawn = _readyToSpawn; }

	Spawner(EntityStats _stats, GamesEngineeringBase::Image* _image) : Entity(_stats, _image) {}

	void draw(GamesEngineeringBase::Window& window, Camera& cam) override;
	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override;
};