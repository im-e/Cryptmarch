#pragma once
#include "Entity.h"
#include "../Utilities/Vector.h"
#include "Projectile.h"

class EntityManager {

private:
	Vector<Player*> players;
	Vector<Enemy*> enemies;
	Vector<Projectile> projectiles;

	bool allPlayersDead = false;


public:
	~EntityManager()
	{
		clear();
	}

	void clear()
	{
		for (int i = 0; i < players.getSize(); i++) delete players[i];
		for (int i = 0; i < enemies.getSize(); i++) delete enemies[i];
		players.clear(); 
		enemies.clear();
		allPlayersDead = false;
	}

	Player* getFirstPlayer() 
	{
		if (players.getSize() == 0) return nullptr;
		return players[0]; 
	}
	
	bool getAllPlayersDead() { return allPlayersDead; }
	void addPlayer(Player* player) { players.add(player); }

	void addEnemy(Enemy* enemy) { enemies.add(enemy); }

	void deletePlayer(Player* player) { players.removeSwap(player);	}

	void deleteEnemy(Enemy* enemy) { enemies.removeSwap(enemy);	}

	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) 
	{
		//update all players
		for (int i = 0; i < players.getSize(); i++)
		{
			players[i]->update(window, map, deltaTime);

			players[i]->processAttack(enemies);
		}

		//update all enemies
		for (int i = 0; i < enemies.getSize(); i++)
		{
			enemies[i]->update(window, map, deltaTime);
		}

		//check for collisions between players and enemies
		for (int i = 0; i < players.getSize(); i++)
		{
			for (int j = 0; j < enemies.getSize(); j++)
			{
				if (players[i]->checkEntityCollision(enemies[j]))
				{
					players[i]->takeDamage(enemies[j]->getDamage());

					//for knockback calculation
					AABB player = players[i]->getHitbox();
					AABB enemy = enemies[j]->getHitbox();
					//calcualate the direction away from the enemy
					float dirX = player.x - enemy.x;
					float dirY = player.y - enemy.y;
					float length = sqrt(dirX * dirX + dirY * dirY);
					if (length > 0.0f)
					{
						dirX /= length; //normalise
						dirY /= length;
					}

					//apply knockback
					players[i]->applyKnockback(dirX, dirY, enemies[i]->getKnockbackForce(), enemies[i]->getKnockbackDuration());


				}
			}
		}

	}

	void updateProjectiles(Map& map, float deltaTime)
	{
		//loop backwards so removeSwap() doesn't break our index tracking
		for (int i = projectiles.getSize() - 1; i >= 0; i--)
		{
			Projectile& projectile = projectiles[i]; //get a reference for easy manipulation

			//move the projectile based on direction, speed, and deltaTime
			projectile.x += projectile.dirX * projectile.speed * deltaTime;
			projectile.y += projectile.dirY * projectile.speed * deltaTime;

			//count down the lifetime of the projectile
			projectile.lifetime -= deltaTime;
			if (projectile.lifetime <= 0.0f) //if the lifetime is up
			{
				projectiles.removeSwap(i); //remove it from the list
				continue;
			}

			//check if its blocked by a wall
			if (map.isBlockedByWall(projectile.x, projectile.y, projectile.width, projectile.height))
			{
				projectiles.removeSwap(i); //if its blocked remove the projectile
				continue;
			}

			//if the owner of the projectile is the player
			if (projectile.owner == ProjectileOwner::Player)
			{
				//we want to check player projectiles check against all enemies
				for (int j = 0; j < enemies.getSize(); j++)
				{
					if (Collisions::checkAABBCollision(projectile.getAABB(), enemies[j]->getHitbox())) //projectile + enemy hitbox
					{
						enemies[j]->takeDamage(projectile.damage); //enemy take damage from projectile
						projectile.dead = true; //mark projectile is dead
						//apply knockback
						break; //hit an enemy so we should stop checking other enemies for this projectile
					}
				}
			}
			else if (projectile.owner == ProjectileOwner::Enemy) //if the owner is an enemy
			{
				//check enemy projectile against all players
				for (int j = 0; j < players.getSize(); j++)
				{
					if (Collisions::checkAABBCollision(projectile.getAABB(), players[j]->getHitbox()))
					{
						players[j]->takeDamage(projectile.damage); //take damage
						//apply knockback
						projectile.dead = true; //mark projectile as dead
						break; //hit so we should stop checking
					}
				}
			}

			//remove if marked dead
			if (projectile.dead)
			{
				projectiles.removeSwap(i);
			}
		}
	}


	int cleanupKills()
	{
		int killed = 0;

		//update all enemies by using a backwards loop to prevent skipping elements
		for (int i = enemies.getSize() - 1; i >= 0; i--)
		{
			if (enemies[i]->isDead())
			{
				delete enemies[i];
				enemies.removeSwap(i);
				killed++;
			}
		}

		//update all enemies by using a backwards loop to prevent skipping elements
		for (int i = players.getSize() - 1; i >= 0; i--)
		{
			if (players[i]->isDead())
			{
				delete players[i];
				players.removeSwap(i);
			}
		}

		if (players.getSize() == 0)
		{
			allPlayersDead = true;
		}
		return killed;
	}

	void draw(GamesEngineeringBase::Window& window, Camera& cam)
	{
		for (int i = 0; i < players.getSize(); i++)
		{
			players[i]->draw(window, cam);
		}

		for (int i = 0; i < enemies.getSize(); i++)
		{
			enemies[i]->draw(window, cam);
		}
	}

	void addProjectile(const Projectile& projectile)
	{
		projectiles.add(projectile);
	}
	
};