#pragma once
#include "Enemy.h"
#include "EnemyDefinition.h"
#include "Entities/Players/Player.h"
#include "EnemyTypes.h"
#include "GamesEngineeringBase.h"
#include <cstdlib>

class EnemyFactory {
private:

	GamesEngineeringBase::Image* enemyImage;
	GamesEngineeringBase::Image* projectileImage;
	inline static EnemyDefinition enemyTypes[6]
	{
		    //name, health, speed, damage, x y knockback isRanged
		EnemyDefinition(150, 125, 15, { 0, 2 }, 150, false, 35, 0), //slime - healthy slow low dmg 
		EnemyDefinition(100, 200, 15, { 4, 6 }, 200, false, 25, 15), //ant - healthy, fast, low dmg
		EnemyDefinition(50, 250, 20, { 0, 5 }, 50, false, 15, 30), //ghost - very weak but fast and med dmg 
		EnemyDefinition(75, 130, 25, { 1, 4 }, 200, true, 12, 45), //skeleton - weak but fast ranged 
		EnemyDefinition(250, 80, 45, { 0, 1 }, 500, false, 8, 60), //ogre - high health high dmg but slow
		EnemyDefinition(300, 50, 35, { 9, 7 }, 350, true, 5, 90), //medusa - high health, slow, high dmg ranged
	};

public:

	void init(GamesEngineeringBase::Image* _enemyImage, GamesEngineeringBase::Image* _projectileImage)
	{
		enemyImage = _enemyImage;
		projectileImage = _projectileImage;
	}

	Enemy* createEnemy(EnemyType type, Vector2f pos, Entity* player)
	{
		int enemy = static_cast<int>(type);
		if (enemy > 5) //out of range
		{
			Logger::Error("Error creating enemy, enum out of scope");
			return nullptr;
		}

		EnemyDefinition& def = enemyTypes[enemy]; //get the definition

		EntityStats stats = { def.maxHealth, def.speed, pos.x, pos.y, def.spriteOffset.x, def.spriteOffset.y }; //fill in stats
		return new Enemy(stats, enemyImage, player, def.contactDamage, def.knockbackForce, def.isRanged, projectileImage, 2, 23); //fill in constructor
	}

	EnemyType weightedRandomEnemyType(float* gameTime) //returns a weighted randomised enemy
	{
		int unlockedWeight = 0; //weight we will use to divide weight by

		for (EnemyDefinition& enemy : enemyTypes) //for each enemy in enemy types
		{
			if (*gameTime >= enemy.timeUnlocked) //if the game time has surpassed the unlock time of the enemy
			{
				unlockedWeight += enemy.weight; //add its weight to the unlocked weight value
			}
			else {
				break; //if it hasnt, break as this enemy is not unlocked yet
			}

		}

		int weight = rand() % unlockedWeight; //get a random number based on enemies unlocked
		int weightSum = 0; //sum of weights
		int randomEnemy = 0; //the random enemy we will pick at the end

		for (int i = 0; i < 6; i++) //for each enemy type
		{
			weightSum += enemyTypes[i].weight; //add up the sum of their weights
			if (weightSum > weight) //if the sum exceeds the weight of the enemy
			{
				randomEnemy = i; //spawn that enemy
				break;
			}
		}

		return static_cast<EnemyType>(randomEnemy);
	}
};