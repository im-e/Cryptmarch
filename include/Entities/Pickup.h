#pragma once
#include "Entity.h"

//what type of pickup it is
enum class PickupType { Health, Mana, Key };

class Pickup : public Entity
{
private:
	PickupType type; //health or mana
	int amount; //how much the potion restores

	float lifetime = 20.0f; //seconds before it disappears
	float pickupDelay = 0.75f; //delay before pickup can be picked up

	bool collected = false; //set when the player picks it up
	bool expires; //potions disappear after their lifetime, the key never does

public:
	//_expires has a default value, so the existing potion code doesnt need to change
	Pickup(EntityStats _stats, GamesEngineeringBase::Image* _image, PickupType _type, int _amount, bool _expires = true) : Entity(_stats, _image)
	{
		type = _type;
		amount = _amount;
		expires = _expires;
	}

	PickupType getType() const { return type; }
	int getAmount() const { return amount; }

	void collect() { collected = true; }

	//true once the pickup delay has run out
	bool canBeCollected() const { return pickupDelay <= 0.0f; }

	//ready to be deleted if it was picked up or ran out of time
	bool shouldRemove() const { return collected || (expires && lifetime <= 0.0f); }

	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override
	{
		lifetime -= deltaTime; //count down
		if (pickupDelay > 0.0f) pickupDelay -= deltaTime; //count down the delay
	}

	void draw(GamesEngineeringBase::Window& window, Camera& cam) override
	{
		drawSprite(window, cam);
	}
};
