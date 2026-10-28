#pragma once
#include "Entities/Player.h"

class Wizard : public Player
{
private:
	//image used for attack (arrows)
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

	//dash (secondary skill)
	int manaCost = 20; //cost of secondary skill
	float secondaryCooldown = 0.5f; 
	float secondaryCooldownTimer = 0.0f; 
	bool keyWasReleased = true; //fire once per press


public:

	Wizard(EntityStats _stats, GamesEngineeringBase::Image* _image, Controls _controls, GamesEngineeringBase::Image* _magicImage, int _magicXOffet, int _magicYOffset) :
		Player(_stats, _image, _controls)
	{
		maxHealth = 100; //wizard is a normal healht is less tanky than other heros
		maxMana = 150; //has more mana than other heroes
		health = maxHealth;
		mana = maxMana;

		magicImage = _magicImage;
		magicTileX = _magicXOffet;
		magicTileY = _magicYOffset;
	}

	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override;

	void draw(GamesEngineeringBase::Window& window, Camera& cam) override;

	void processAttack(AttackContext& attack) override;
};