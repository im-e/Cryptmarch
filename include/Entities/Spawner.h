#pragma once
#include "Entity.h"

class Spawner : public Entity
{
private:

	float spawnTimer = 0; //timer for spawns 
	float spawnInterval = 7.5f; //interval of enemies spawning

	int spawnerRange = 600; //range of if spawner should spawn

	bool readyToSpawn = false; //if the spawner is ready to spawn

public:

	bool getReadyToSpawn() const { return readyToSpawn; }
	void setReadyToSpawn(bool _readyToSpawn) { readyToSpawn = _readyToSpawn; }

	int getSpawnerRange() const { return spawnerRange; }

	Spawner(EntityStats _stats, GamesEngineeringBase::Image* _image) : Entity(_stats, _image) {}

	void draw(GamesEngineeringBase::Window& window, Camera& cam) override;
	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override;
};