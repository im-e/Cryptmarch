#pragma once
#include "Entities/Players/Player.h"

class Wizard : public Player
{
private:
	//image used for attack 
	GamesEngineeringBase::Image* magicImage;
	int magicTileX;
	int magicTileY;

	//primary attack wave nova
	float novaRadius = 96.0f; //how far the blast reaches in pixels
	int novaDamage = 20; //damage of the spell
	bool novaRequested = false; //tracks if spell has been activated from player input
	float novaVisualDuration = 0.18f; //how long the cirlce stays on screen
	float novaVisualTimer = 0.0f; //timer for the screen display
	float novaCooldownTimer = 0.0f; //cooldown timer tracker
	float novaCooldown = 1.0f; //cooldown of the wave nova
	float novaKnockbackForce = 400.0f; //force for knockback of the melee attack
	float novaKnockbackDuration = 0.2f; //duration of knockback for the melee attack

	//selection spell of highest health targets (secondary skill)
	int manaCost = 20; //cost of secondary skill
	int selectDamage = 20; //damage of select skill
	int selectRange = 256.0f; //how far the select spell can reach in pixels (8 tiles)
	bool selectRequested = false; //tracks if the spell has been activated
	float secondaryCooldown = 0.5f; //cooldown of select skill
	float secondaryCooldownTimer = 0.0f; //cooldown timer of select skill
	float secondaryVisualDuration = 0.2f; //time for visual to show on screen
	float secondaryVisualTimer = 0.0f; //timer for visual duration
	bool keyWasReleased = true; //fire once per press
	

public:

	Wizard(EntityStats _stats, GamesEngineeringBase::Image* _image, Controls _controls, GamesEngineeringBase::Image* _magicImage, int _magicXOffset, int _magicYOffset) :
		Player(_stats, _image, _controls)
	{
		maxHealth = 100; //wizard is a normal healht is less tanky than other heros
		maxMana = 150; //has more mana than other heroes
		health = maxHealth;
		mana = maxMana;

		magicImage = _magicImage;
		magicTileX = _magicXOffset;
		magicTileY = _magicYOffset;
	}

	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override;

	void draw(GamesEngineeringBase::Window& window, Camera& cam) override;

	void processAttack(AttackContext& attack) override;
};