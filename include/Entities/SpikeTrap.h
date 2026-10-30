#pragma once
#include "Entity.h"

//spikes that go up and down on a timer, and hurt the player if they are standing on them while they are up
class SpikeTrap : public Entity
{
private:
	bool spikesUp = false; //if the spikes are up or not
	float timer; //time left in the current up/down phase
	const float upTime = 1.0f; //seconds the spikes stay up
	const float downTime = 1.5f; //seconds the spikes stay down (safe to cross)
	int damage = 20; //damage the spikes deal
	bool hasHit = false; //only hurt once each time the spikes come up, otherwise standing on it would drain health very quickly

	//sprite positions
	const int downTileX = 15;
	const int upTileX = 16;
	const int trapTileY = 16;

public:
	SpikeTrap(EntityStats _stats, GamesEngineeringBase::Image* _image);

	bool canHit() const { return spikesUp && !hasHit; } //dangerous right now
	void markHit() { hasHit = true; }
	int getDamage() const { return damage; }

	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override;
	void draw(GamesEngineeringBase::Window& window, Camera& cam) override;
};
