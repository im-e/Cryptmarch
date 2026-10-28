#pragma once
#include "Enemy.h"
#include "EnemyDefinition.h"
#include "Entities/Players/Player.h"
#include "EnemyTypes.h"
#include "GamesEngineeringBase.h"

class EnemyFactory {
private:

	GamesEngineeringBase::Image* enemyImage;
	GamesEngineeringBase::Image* projectileImage;
	inline static EnemyDefinition enemyTypes[6]
	{
		    //name, health, speed, damage, x y knockback isRanged
		EnemyDefinition(150, 125, 15, { 0, 2 }, 150, false), //slime - healthy slow low dmg 
		EnemyDefinition(250, 80, 45, { 0, 1 }, 500, false), //ogre - high health high dmg but slow
		EnemyDefinition(75, 130, 25, { 1, 4 }, 200, true), //skeleton - weak but fast ranged 
		EnemyDefinition(50, 250, 20, { 0, 5 }, 50, false), //ghost - very weak but fast and med dmg 
		EnemyDefinition(100, 200, 15, { 4, 6 }, 200, false), //ant - healthy, fast, low dmg
		EnemyDefinition(300, 50, 35, { 9, 7 }, 350, true), //medusa - high health, slow, high dmg ranged
	};

public:

	void init(GamesEngineeringBase::Image* _enemyImage, GamesEngineeringBase::Image* _projectileImage)
	{
		enemyImage = _enemyImage;
		projectileImage = _projectileImage;
	}

	Enemy* createEnemy(EnemyType type, Vector2f pos, Entity* player)
	{
		int enemy = static_cast<int>(type); //convert enum class into int
		if (enemy > 5) //out of range
		{
			Logger::Error("Error creating enemy, enum out of scope");
			return nullptr;
		}

		EnemyDefinition& def = enemyTypes[enemy]; //get the definition

		EntityStats stats = { def.maxHealth, def.speed, pos.x, pos.y, def.spriteOffset.x, def.spriteOffset.y }; //fill in stats
		return new Enemy(stats, enemyImage, player, def.contactDamage, def.knockbackForce, def.canShoot);
	}
};