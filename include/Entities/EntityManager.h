#pragma once
#include "Entity.h"
#include "Utilities/Vector.h"
#include "Spawner.h"
#include "Projectile.h"
#include "Enemies/EnemyFactory.h"

class EntityManager {

private:
	Vector<Player*> players;
	Vector<Enemy*> enemies;
	Vector<Spawner*> spawners;
	Vector<Projectile> projectiles;

	bool allPlayersDead = false;
	float* gameTime;

	EnemyFactory factory;


public:
	EntityManager(AssetManager& am, float*_gameTime)
	{
		factory.init(&am.enemyImage, &am.itemsImage);
		gameTime = _gameTime;
	}

	~EntityManager()
	{
		clear();
	}

	void clear()
	{
		for (int i = 0; i < players.getSize(); i++) delete players[i];
		for (int i = 0; i < enemies.getSize(); i++) delete enemies[i];
		for (int i = 0; i < spawners.getSize(); i++) delete spawners[i];
		players.clear(); 
		enemies.clear();
		projectiles.clear();
		spawners.clear();
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

	void addProjectile(const Projectile& projectile) { projectiles.add(projectile);	}

	void addSpawner(Spawner* spawner) { spawners.add(spawner); }
	
	void deleteSpawner(Spawner* spawner) { spawners.removeSwap(spawner); }

	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) 
	{
		AttackContext attack = { enemies, projectiles, spawners };

		//update all players
		for (int i = 0; i < players.getSize(); i++)
		{
			players[i]->update(window, map, deltaTime);

			players[i]->processAttack(attack);
		}

		//update all enemies
		for (int i = 0; i < enemies.getSize(); i++)
		{
			enemies[i]->update(window, map, deltaTime); //update enemies
			if (enemies[i]->getFireRequested()) //if fire has been requested
			{
				projectiles.add(enemies[i]->fireProjectile()); //add projectile to projectile list
				enemies[i]->setFireRequested(false); //set fire requested to false
			}
		}

		//update all spawners
		for (int i = 0; i < spawners.getSize(); i++)
		{
			//work out range of spawner
			Vector2f sp = spawners[i]->getCentrePoint(); //spawner 
			Vector2f pl = getFirstPlayer()->getCentrePoint(); //player

			float dx = sp.x - pl.x; //find the difference of the two points
			float dy = sp.y - pl.y; //player to spawner
			float distanceSquared = (dx * dx) + (dy * dy); //If dx² + dy² is less than range², the player is in range
			float rangeSquared = spawners[i]->getSpawnerRange() * spawners[i]->getSpawnerRange();
			if (distanceSquared >= rangeSquared) //spawner ISNT in range
			{
				continue; //skip to next spawner
			}
			spawners[i]->update(window, map, deltaTime); //update spawners
			if (spawners[i]->getReadyToSpawn()) //if they are ready to spawn
			{
				spawnEnemy({ spawners[i]->getX(), spawners[i]->getY() }); //spawn enemy
				spawners[i]->setReadyToSpawn(false); //set ready to spawn to false
			}

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
					players[i]->applyImpulse(dirX, dirY, enemies[j]->getKnockbackForce(), enemies[j]->getKnockbackDuration());


				}
			}
		}

		updateProjectiles(map, deltaTime);
	}

	void spawnEnemy(Vector2f pos)
	{
		EnemyType type = factory.weightedRandomEnemyType(gameTime); //get a weighted random enemy type
		enemies.add(factory.createEnemy(type, pos, getFirstPlayer())); //spawn that enemy at pos
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
						enemies[j]->applyKnockback(projectile.dirX, projectile.dirY, projectile.knockbackForce, projectile.knockbackDuration);
						projectile.dead = true; //mark projectile is dead
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

		for (int i = 0; i < spawners.getSize(); i++)
		{
			spawners[i]->draw(window, cam);
		}


		drawProjectiles(window, cam);
	}

	void drawProjectiles(GamesEngineeringBase::Window& window, Camera& cam)
	{
		for (int i = 0; i < projectiles.getSize(); i++)
		{
			Projectile& p = projectiles[i]; //get projectile
			if (p.image == nullptr) continue; //if projectile is null go to next element

			//hitbox centre in world space
			float centreX = p.x + p.width / 2.0f; // width/2 finds the centre in world space
			float centreY = p.y + p.height / 2.0f;

			//move the X/Y back half a tile so the 32x32 sprite is centred on the hitbox
			const int halfTile = p.tileSize / 2; // 16
			int worldX = centreX - halfTile;
			int worldY = centreY - halfTile;

			int screenX = cam.worldToScreenX(worldX);
			int screenY = cam.worldToScreenY(worldY);

			Graphics::DrawTilemapImage(window, *p.image, p.tileSize, p.tileX, p.tileY, screenX, screenY);

#ifdef _DEBUG
			Graphics::DrawRectOutline(window, cam.worldToScreenX(p.x), cam.worldToScreenY(p.y),	p.width, p.height, 255, 0, 255);
#endif
		}
	}

	
};