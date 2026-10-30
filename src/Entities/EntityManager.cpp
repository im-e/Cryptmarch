#include "Entities/EntityManager.h"
#include <cmath>

EntityManager::EntityManager(AssetManager& am, float* _gameTime)
{
	factory.init(&am.enemyImage, &am.itemsImage);
	gameTime = _gameTime;
	itemsImage = &am.itemsImage;
}

EntityManager::~EntityManager()
{
	clear();
}

void EntityManager::clear()
{
	for (int i = 0; i < players.getSize(); i++) delete players[i];
	for (int i = 0; i < enemies.getSize(); i++) delete enemies[i];
	for (int i = 0; i < spawners.getSize(); i++) delete spawners[i];
	for (int i = 0; i < pickups.getSize(); i++) delete pickups[i];
	for (int i = 0; i < doors.getSize(); i++) delete doors[i];
	for (int i = 0; i < plates.getSize(); i++) delete plates[i];
	for (int i = 0; i < traps.getSize(); i++) delete traps[i];
	players.clear();
	enemies.clear();
	projectiles.clear();
	spawners.clear();
	pickups.clear();
	doors.clear();
	plates.clear();
	traps.clear();
	allPlayersDead = false;
	hasKey = false;
	levelComplete = false;
}

void EntityManager::update(GamesEngineeringBase::Window& window, Map& map, float deltaTime)
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
	Player* firstPlayer = getFirstPlayer();
	for (int i = 0; i < spawners.getSize() && firstPlayer != nullptr; i++) //skip if there is no player to measure range from
	{
		//work out range of spawner
		Vector2f sp = spawners[i]->getCentrePoint(); //spawner
		Vector2f pl = firstPlayer->getCentrePoint(); //player

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
	updatePickups(window, map, deltaTime);
	updateLevelObjects(window, map, deltaTime);
}

void EntityManager::spawnEnemy(Vector2f pos)
{
	EnemyType type = factory.weightedRandomEnemyType(gameTime); //get a weighted random enemy type
	enemies.add(factory.createEnemy(type, pos, getFirstPlayer())); //spawn that enemy at pos
}

void EntityManager::updateProjectiles(Map& map, float deltaTime)
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

		//check if its blocked by a wall (closed doors count as walls too)
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

			//if it didn't hit an enemy, check spawners (no knockback, spawners don't move)
			if (!projectile.dead)
			{
				for (int j = 0; j < spawners.getSize(); j++)
				{
					if (spawners[j]->isDead()) continue; //skip already destroyed spawners

					if (Collisions::checkAABBCollision(projectile.getAABB(), spawners[j]->getHitbox())) //projectile + spawner hitbox
					{
						spawners[j]->takeDamage(projectile.damage); //spawner takes damage
						projectile.dead = true; //arrow is used up
						break; //stop checking other spawners
					}
				}
			}
		}
		else if (projectile.owner == ProjectileOwner::Enemy) //if the owner is an enemy (trap arrows count as enemy arrows too)
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

//rolls the dice when an enemy dies, and maybe drops a potion where it died
void EntityManager::tryDropPickup(Vector2f pos)
{
	if (rand() % 100 >= dropChance) return; //rand() % 100 gives 0-99, so this fails (100 - dropChance)% of the time

	Pickup* pickup;
	if (rand() % 2 == 0) //50/50 between health and mana
	{
		//                     hp speed x      y      sprite column, row in items.png
		EntityStats stats = { 1, 0, pos.x, pos.y, 1, 19 }; //red potion
		pickup = new Pickup(stats, itemsImage, PickupType::Health, healthPotionAmount);
		Logger::Debug("Dropped health potion");
	}
	else
	{
		EntityStats stats = { 1, 0, pos.x, pos.y, 3, 20 }; //blue potion
		pickup = new Pickup(stats, itemsImage, PickupType::Mana, manaPotionAmount);
		Logger::Debug("Dropped mana potion");
	}
	pickups.add(pickup);
}

void EntityManager::updatePickups(GamesEngineeringBase::Window& window, Map& map, float deltaTime)
{
	for (int i = 0; i < pickups.getSize(); i++)
	{
		Pickup* pickup = pickups[i];
		pickup->update(window, map, deltaTime); //count down its lifetime
		if (!pickup->canBeCollected()) continue; //just dropped, give the player a moment to see it

		for (int j = 0; j < players.getSize(); j++)
		{
			Player* player = players[j];
			if (!player->checkEntityCollision(pickup)) continue; //not touching

			if (pickup->getType() == PickupType::Health)
			{
				if (player->getHealth() >= player->getMaxHealth()) continue; //already full, leave it for later
				player->heal(pickup->getAmount());
				Logger::Debug("Picked up health potion, health now " + std::to_string(player->getHealth()));
			}
			else if (pickup->getType() == PickupType::Mana)
			{
				if (player->getMana() >= player->getMaxMana()) continue; //already full, leave it for later
				player->restoreMana(pickup->getAmount());
				Logger::Debug("Picked up mana potion, mana now " + std::to_string(player->getMana()));
			}
			else if (pickup->getType() == PickupType::Key)
			{
				hasKey = true;

				//unlock every exit door, this swaps its sprite to the open version
				for (int k = 0; k < doors.getSize(); k++)
				{
					if (doors[k]->getIsExit()) doors[k]->open(map);
				}
				Logger::Info("Key collected, exit unlocked");
			}

			pickup->collect(); //mark it for removal
			break; //only one player can take it
		}
	}

	//remove collected or expired pickups, backwards so removeSwap doesnt skip any
	for (int i = pickups.getSize() - 1; i >= 0; i--)
	{
		if (pickups[i]->shouldRemove())
		{
			delete pickups[i];
			pickups.removeSwap(i);
		}
	}
}

//pressure plates, spike traps and the exit
void EntityManager::updateLevelObjects(GamesEngineeringBase::Window& window, Map& map, float deltaTime)
{
	//PRESSURE PLATES
	for (int i = 0; i < plates.getSize(); i++)
	{
		PressurePlate* plate = plates[i];
		plate->update(window, map, deltaTime); //fake plates count down until they reset

		for (int j = 0; j < players.getSize(); j++)
		{
			if (!players[j]->checkEntityCollision(plate)) continue; //not standing on it

			if (plate->press()) //true only on the frame the plate goes down
			{
				if (plate->getIsFake())
				{
					Logger::Debug("Fake pressure plate triggered, firing arrows");
					fireArrowTrap(plate->getCentrePoint(), map); //its a trap!
				}
				else
				{
					Logger::Debug("Pressure plate " + std::to_string(plate->getId()) + " pressed");
					openDoorsWithId(plate->getId(), map);
				}
			}
		}
	}

	//SPIKE TRAPS
	for (int i = 0; i < traps.getSize(); i++)
	{
		SpikeTrap* trap = traps[i];
		trap->update(window, map, deltaTime); //raise and lower the spikes

		if (!trap->canHit()) continue; //spikes are down, or already hit someone this time

		for (int j = 0; j < players.getSize(); j++)
		{
			if (players[j]->checkEntityCollision(trap))
			{
				players[j]->takeDamage(trap->getDamage());
				trap->markHit(); //only hurts once each time the spikes come up
				break;
			}
		}
	}

	//EXIT - walking into an open exit door finishes the level
	for (int i = 0; i < doors.getSize(); i++)
	{
		Door* door = doors[i];
		if (!door->getIsExit() || !door->isOpen()) continue;

		for (int j = 0; j < players.getSize(); j++)
		{
			if (players[j]->checkEntityCollision(door))
			{
				levelComplete = true;
				Logger::Info("Level complete");
			}
		}
	}
}

void EntityManager::openDoorsWithId(int id, Map& map)
{
	for (int i = 0; i < doors.getSize(); i++)
	{
		//exit doors are only opened by the key, never by a plate
		if (!doors[i]->getIsExit() && doors[i]->getId() == id)
		{
			doors[i]->open(map);
		}
	}
}

void EntityManager::fireArrowTrap(Vector2f target, Map& map)
{
	//the 4 directions the arrows come from: left, right, above, below
	const int dirX[4] = { -1, 1, 0, 0 };
	const int dirY[4] = { 0, 0, -1, 1 };
	const int tile = 32;

	Projectile arrow; //template arrow, each direction copies it then changes position and direction
	arrow.speed = trapArrowSpeed;
	arrow.damage = trapArrowDamage;
	arrow.owner = ProjectileOwner::Enemy; //enemy owned so it hurts the player
	arrow.knockbackForce = 0.0f;
	arrow.knockbackDuration = 0.0f;
	arrow.image = itemsImage;
	arrow.tileX = 0; //arrow sprite in items.png
	arrow.tileY = 23;

	for (int d = 0; d < 4; d++)
	{
		//start far away and walk towards the target until we find a spot that isnt inside a wall
		//this way arrows come out of the room rather than spawning inside the wall and dying instantly
		for (int distance = trapLaunchDistance; distance >= 2; distance--)
		{
			float startX = target.x + dirX[d] * distance * tile - arrow.width / 2.0f;
			float startY = target.y + dirY[d] * distance * tile - arrow.height / 2.0f;

			if (map.isBlockedByWall(startX, startY, arrow.width, arrow.height)) continue; //inside a wall, move closer

			Projectile shot = arrow;
			shot.x = startX;
			shot.y = startY;
			shot.dirX = (float)-dirX[d]; //fly back towards the plate
			shot.dirY = (float)-dirY[d];
			shot.lifetime = (distance * tile) / trapArrowSpeed + 0.5f; //long enough to reach the plate and a bit past it
			projectiles.add(shot);
			break; //found a launch spot for this direction
		}
	}
}

int EntityManager::cleanupKills()
{
	int killed = 0;

	//update all enemies by using a backwards loop to prevent skipping elements
	for (int i = enemies.getSize() - 1; i >= 0; i--)
	{
		if (enemies[i]->isDead())
		{
			tryDropPickup({ enemies[i]->getX(), enemies[i]->getY() }); //maybe drop a potion where it died
			delete enemies[i];
			enemies.removeSwap(i);
			killed++;
		}
	}

	//remove destroyed spawners, backwards loop for the same reason
	for (int i = spawners.getSize() - 1; i >= 0; i--)
	{
		if (spawners[i]->isDead())
		{
			delete spawners[i]; //free the memory
			spawners.removeSwap(i);
		}
	}

	//update all players by using a backwards loop to prevent skipping elements
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

void EntityManager::draw(GamesEngineeringBase::Window& window, Camera& cam)
{
	//floor objects first so everything else is drawn on top of them
	for (int i = 0; i < traps.getSize(); i++) traps[i]->draw(window, cam);
	for (int i = 0; i < plates.getSize(); i++) plates[i]->draw(window, cam);
	for (int i = 0; i < doors.getSize(); i++) doors[i]->draw(window, cam);
	for (int i = 0; i < pickups.getSize(); i++) pickups[i]->draw(window, cam);

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

void EntityManager::drawProjectiles(GamesEngineeringBase::Window& window, Camera& cam)
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
		Graphics::DrawRectOutline(window, cam.worldToScreenX(p.x), cam.worldToScreenY(p.y), p.width, p.height, 255, 0, 255);
#endif
	}
}
