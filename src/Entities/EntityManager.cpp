#include "Entities/EntityManager.h"
#include <cmath>
#include <algorithm> //for min() and max()

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


	//player leash calculation
	float maxApartX = window.getWidth() - 64; //max distance for players to be apart from each other
	float maxApartY = window.getHeight() - 64;
	if (players.getSize() < 2) //singleplayer, or one player left
	{
		for (int i = 0; i < players.getSize(); i++)
		{
			players[i]->clearLeash(); //no leash needed
		}
	}
	else //multiplayer
	{
		for (int i = 0; i < players.getSize(); i++)
		{
			float minX = -FLT_MAX;
			float maxX = FLT_MAX;
			float minY = -FLT_MAX;
			float maxY = FLT_MAX;

			for (int j = 0; j < players.getSize(); j++)
			{
				if (i == j) continue; //if i and j are equal skip. this ensures were doing each other player
				//i may be at most maxApart away from j on each axis
				minX = std::max(minX, players[j]->getX() - maxApartX);
				maxX = std::min(maxX, players[j]->getX() + maxApartX);
				minY = std::max(minY, players[j]->getY() - maxApartY);
				maxY = std::min(maxY, players[j]->getY() + maxApartY);

			}

			players[i]->setLeash(minX, maxX, minY, maxY);
		}
		
	}


	//update all players
	for (int i = 0; i < players.getSize(); i++)
	{
		players[i]->update(window, map, deltaTime);

		players[i]->processAttack(attack);
	}

	//update all enemies
	for (int i = 0; i < enemies.getSize(); i++)
	{
		Enemy* enemy = enemies[i]; //get enemy

		//set target for enemies
		Entity* current = enemy->getTarget(); //enemy current target
		bool keepCurrent = false; //if we are keeping the current player target or not
		Player* best = nullptr; //best is going to the closest player in range
		float currentDistSq = FLT_MAX; //current distance squared - used for comparing two players distance between each other
		float bestDistSq = enemy->getAggroRange() * enemy->getAggroRange(); //get the sq distance for aggro range
		float leashSq = enemy->getLeashRange() * enemy->getLeashRange(); //get the dq distance for leash range (de-aggro range)
		for (int j = 0; j < players.getSize(); j++) //for each player
		{
			Player* player = players[j]; //get player 
			float dx = player->getCentrePoint().x - enemy->getCentrePoint().x; //get dx and dy
			float dy = player->getCentrePoint().y - enemy->getCentrePoint().y;
			float distSq = (dx * dx) + (dy * dy); // dx*dx + dy*dy squared distance

			if (player == current && distSq < leashSq)
			{
				keepCurrent = true; //if player is current and within leash range, keep current
				currentDistSq = distSq; //set the current dist sq
			}
			if (distSq < bestDistSq) //if player distance sq is less than best dist sq
			{
				best = player; //set best to player
				bestDistSq = distSq; //set best dist sq to the player dist sq, to check if next player is closer
			}
		} //end of player loop

		if (keepCurrent && best != nullptr && best != current) //if current player is in range, but best isnt the current target
		{ 
			if (bestDistSq < currentDistSq)  //best is closer than current, so we should swap to best
			{
				keepCurrent = false; //keep current to false, which switches to best
			}
		}
		if (keepCurrent) enemy->setTarget(current); //keep target as current
		else { enemy->setTarget(best); } //set target to best

		enemy->update(window, map, deltaTime); //update enemies

		if (enemy->getFireRequested()) //if fire has been requested
		{
			projectiles.add(enemy->fireProjectile()); //add projectile to projectile list
			enemy->setFireRequested(false); //set fire requested to false
		}
	}


	//update all spawners (that have a player in range)
	for (int i = 0; i < spawners.getSize(); i++) //update spawners that have a player in range
	{
		//work out range of spawner
		Vector2f sp = spawners[i]->getCentrePoint(); //spawner
		float rangeSquared = spawners[i]->getSpawnerRange() * spawners[i]->getSpawnerRange(); //get spawner range
		bool anyPlayerInRange = false;

		for (int j = 0; j < players.getSize(); j++)
		{
			Vector2f pl = players[j]->getCentrePoint(); //player

			float dx = sp.x - pl.x; //find the difference of the two points
			float dy = sp.y - pl.y; //player to spawner
			float distanceSquared = (dx * dx) + (dy * dy); //If dx² + dy² is less than range², the player is in range
			if (distanceSquared < rangeSquared) //check if in range
			{
				anyPlayerInRange = true; //set player is in range to true;
				break;
			}

		}

		if (!anyPlayerInRange) //spawner ISNT in range to either player
		{
			continue; //skip to next spawner
		}

		//spawner is in range to a player
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
				bool tookDamage = players[i]->takeDamage(enemies[j]->getDamage()); //take damage and store if they did
				if (tookDamage) //if damage taken
				{ //apply knockback
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
	}
	//update things
	updateProjectiles(map, deltaTime);
	updatePickups(window, map, deltaTime);
	updateLevelObjects(window, map, deltaTime);
}

void EntityManager::spawnEnemy(Vector2f pos)
{
	EnemyType type = factory.weightedRandomEnemyType(gameTime); //get a weighted random enemy type
	enemies.add(factory.createEnemy(type, pos)); //spawn that enemy at pos
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
					bool tookDamage = enemies[j]->takeDamage(projectile.damage); //enemy take damage from projectile
					if (tookDamage) //if damage taken, apply knockback
					{
						enemies[j]->applyKnockback(projectile.dirX, projectile.dirY, projectile.knockbackForce, projectile.knockbackDuration);
					}
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
					{ //dont need to check for damage as spawners dont take knockback
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

					bool tookDamage = players[j]->takeDamage(projectile.damage); //take damage
					if (tookDamage) //if damage taken, knockback
					{
						players[j]->applyImpulse(projectile.dirX, projectile.dirY, projectile.knockbackForce, projectile.knockbackDuration);
					}
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
	if (rand() % 100 >= dropChance) return; //if 0-99 is more than or equal to drop chance

	Pickup* pickup; //create a new pickup
	if (rand() % 2 == 0) //50/50 between health and mana
	{ // 0 = health potion
		//                     hp speed x      y      sprite column, row in items.png
		EntityStats stats = { 1, 0, pos.x, pos.y, 1, 19 }; //red potion
		pickup = new Pickup(stats, itemsImage, PickupType::Health, healthPotionAmount);
		Logger::Debug("Dropped health potion");
	}
	else // 1 = mana potion
	{
		EntityStats stats = { 1, 0, pos.x, pos.y, 3, 20 }; //blue potion
		pickup = new Pickup(stats, itemsImage, PickupType::Mana, manaPotionAmount);
		Logger::Debug("Dropped mana potion");
	}
	pickups.add(pickup);
}

//update pickups
void EntityManager::updatePickups(GamesEngineeringBase::Window& window, Map& map, float deltaTime)
{
	for (int i = 0; i < pickups.getSize(); i++) //for each pickup
	{
		Pickup* pickup = pickups[i]; //get pickup
		pickup->update(window, map, deltaTime); //count down its lifetime

		for (int j = 0; j < players.getSize(); j++) //for each player
		{
			Player* player = players[j]; //get player
			if (!player->checkEntityCollision(pickup)) continue; //check if player is touching a pickup

			if (pickup->getType() == PickupType::Health) //if its health
			{
				if (player->getHealth() >= player->getMaxHealth()) continue; //already full
				player->heal(pickup->getAmount()); //heal for pickup amount
				Logger::Debug("Picked up health potion, health now " + std::to_string(player->getHealth()));
			}
			else if (pickup->getType() == PickupType::Mana) //if its a mana potion
			{
				if (player->getMana() >= player->getMaxMana()) continue; //already full
				player->restoreMana(pickup->getAmount()); //heal for pickup amount
				Logger::Debug("Picked up mana potion, mana now " + std::to_string(player->getMana()));
			}
			else if (pickup->getType() == PickupType::Key) //if its a key
			{
				hasKey = true; //signal that key has been picked up

				//unlock the exit
				for (int k = 0; k < doors.getSize(); k++) //get all the doors
				{
					if (doors[k]->getIsExit()) doors[k]->open(map); //if its an exit door, open the door
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
	for (int i = 0; i < plates.getSize(); i++) //for each plate
	{
		PressurePlate* plate = plates[i]; //get the plate
		plate->update(window, map, deltaTime); //update the fake plates for reset

		for (int j = 0; j < players.getSize(); j++) //get players
		{
			if (!players[j]->checkEntityCollision(plate)) continue; //check if being stood on

			if (plate->press()) //true only on the frame the plate goes down
			{
				if (plate->getIsFake()) //if its a fake
				{
					Logger::Debug("Fake pressure plate triggered, firing arrows");
					fireArrowTrap(plate->getCentrePoint(), map); //fire arrows
				}
				else
				{
					Logger::Debug("Pressure plate " + std::to_string(plate->getId()) + " pressed");
					openDoorsWithId(plate->getId(), map); //open doors related to id
				}
			}
		}
	}

	//SPIKE TRAPS
	for (int i = 0; i < traps.getSize(); i++) //for each spike trap
	{
		SpikeTrap* trap = traps[i]; //get the trap
		trap->update(window, map, deltaTime); //raise and lower the spikes

		if (!trap->canHit()) continue; //if the spikes are down, skip

		for (int j = 0; j < players.getSize(); j++) //get players
		{
			if (players[j]->checkEntityCollision(trap)) //check if colliding
			{
				players[j]->takeDamage(trap->getDamage()); //no knockback needed for spikes
				trap->markHit(); //only hurts once each time the spikes come up
				break;
			}
		}
	}

	//EXIT - walking into an open exit door finishes the level
	for (int i = 0; i < doors.getSize(); i++) //for each door
	{
		Door* door = doors[i]; //get the door
		if (!door->getIsExit() || !door->isOpen()) continue; //dont need to check non-exit doors or not open doors

		for (int j = 0; j < players.getSize(); j++) //get players
		{
			if (players[j]->checkEntityCollision(door)) //check if player has exited
			{
				levelComplete = true; //set level as complete
				Logger::Info("Level complete");
			}
		}
	}
}
//opens doors with corresponding id
void EntityManager::openDoorsWithId(int id, Map& map)
{
	for (int i = 0; i < doors.getSize(); i++) //get all doors
	{
		//if the door isnt an exit door and the door has the right id
		if (!doors[i]->getIsExit() && doors[i]->getId() == id) //this is because we want the key to open the final door, shouldnt happen anyways but i included it for safety
		{
			doors[i]->open(map); //open the door
		}
	}
}

void EntityManager::fireArrowTrap(Vector2f target, Map& map)
{
	//the 4 directions the arrows come from: left, right, above, below
	const int dirX[4] = { -1, 1, 0, 0 };
	const int dirY[4] = { 0, 0, -1, 1 };
	const int tile = 32; //tile size

	Projectile arrow; //template arrow, each direction copies it then changes position and direction
	arrow.speed = trapArrowSpeed; //set speed
	arrow.damage = trapArrowDamage; //set damage
	arrow.owner = ProjectileOwner::Enemy; //enemy owned so it hurts the player
	arrow.knockbackForce = 0.0f; //no knockback, as its a trap (just a design choice)
	arrow.knockbackDuration = 0.0f;
	arrow.image = itemsImage; //set to image which contains the arrows
	arrow.tileX = 0; //arrow sprite in items.png
	arrow.tileY = 23;

	for (int i = 0; i < 4; i++) //4 arrow trap
	{
		//start far away and walk towards the target until we find a spot that isnt inside a wall
		//this way arrows come out of the room rather than spawning inside the wall and dying instantly
		for (int distance = trapLaunchDistance; distance >= 2; distance--) 
			//try 6 tiles away, then 5, then 4... stopping at 2, just so that its not unreactable
			//this does make it so all the arrows are the same distance away so some traps are less forigivng than others depending on how close to the wall they are on one side
		{
			//work out the target start position in pixels
			//target.x is centre, dirx/y are the directions, multiply by distance and tile width and - arrow.w/h /2 to centre
			float startX = target.x + dirX[i] * distance * tile - arrow.width / 2.0f;
			float startY = target.y + dirY[i] * distance * tile - arrow.height / 2.0f;

			//if its blocked by a wall, skip and try again but closer
			if (map.isBlockedByWall(startX, startY, arrow.width, arrow.height)) continue; //inside a wall, move closer

			//if its not create the arrow
			Projectile shot = arrow;
			shot.x = startX;
			shot.y = startY;
			shot.dirX = (float)-dirX[i]; //fly back towards the plate
			shot.dirY = (float)-dirY[i];
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
