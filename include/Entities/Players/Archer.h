#pragma once
#include "Entities/Player.h"
#include "Entities/Projectile.h"

class Archer : public Player
{
private:
	//image used for attack (arrows)
	GamesEngineeringBase::Image* projectileImage;
	int projectileTileX;
	int projectileTileY;

	//primary attack
	float arrowSpeed = 450.0f; //speed that the arrow travels
	float arrowLifetime = 1.5f; //lifetime of the arrow
	int arrowDamage = 20; //damage of the arrow
	float fireCooldown = 0.4f; //cooldown to fire again
	float fireCooldownTimer = 0.0f; //cooldown tracker
	bool fireRequested = false; //tracks if fire has been activated
	float attackKnockbackForce = 150.0f; //force for knockback of the melee attack
	float attackKnockbackDuration = 0.10f; //duration of knockback for the melee attack

	//dash (secondary skill)
	float dashSpeed = 600.0f; //pixels per second while dashing
	int manaCost = 20; //cost of secondary skill
	float dashDuration = 0.2f; //seconds -> dash covers 600 * 0.2 = 120px (~4 tiles)
	float dashCooldown = 0.5f; //stops dashes being chained back to back
	float dashCooldownTimer = 0.0f; //cooldown timer for the dash
	float dashTimer = 0.0f; //time dash is active
	bool keyWasReleased = true; //fire once per press

	bool isDashing() { return dashTimer > 0.0f; }

public:

	Archer(EntityStats _stats, GamesEngineeringBase::Image* _image, Controls _controls, GamesEngineeringBase::Image* _projectileImage, int _projectileXOffet, int _projectileYOffset) :
		Player(_stats, _image, _controls)
	{
		maxHealth = 70; //archer is less tanky than other heros
		maxMana = 100; //
		health = maxHealth;
		mana = maxMana;

		projectileImage = _projectileImage;
		projectileTileX = _projectileXOffet;
		projectileTileY = _projectileYOffset;
	}

	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override;

	void draw(GamesEngineeringBase::Window& window, Camera& cam) override;

	Projectile fireProjectile();

	void processAttack(AttackContext& attack) override;

};