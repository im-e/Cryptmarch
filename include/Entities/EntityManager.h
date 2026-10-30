#pragma once
#include "Entity.h"
#include "Utilities/Vector.h"
#include "Spawner.h"
#include "Projectile.h"
#include "Pickup.h"
#include "Door.h"
#include "PressurePlate.h"
#include "SpikeTrap.h"
#include "Enemies/EnemyFactory.h"
#include "Utilities/AssetManager.h"

class EntityManager {

private:
	Vector<Player*> players; //list of players
	Vector<Enemy*> enemies; //list of enemies
	Vector<Spawner*> spawners; //spawners of enemies
	Vector<Projectile> projectiles; //projectiles in the level
	Vector<Pickup*> pickups; //potions dropped by enemies, and the level key
	Vector<Door*> doors; //exit door and pressure plate doors
	Vector<PressurePlate*> plates; //real and fake pressure plates
	Vector<SpikeTrap*> traps; //spike traps

	bool allPlayersDead = false; //true once all players are dead
	bool hasKey = false; //true once the key has been picked up
	bool levelComplete = false; //true once a player walks through the open exit
	float* gameTime; //time of how long the level has been running

	EnemyFactory factory; //factory for creating enemies

	//drop settings
	GamesEngineeringBase::Image* itemsImage; //sprite sheet the potions, key and trap arrows come from
	const int dropChance = 30; //percent chance an enemy drops something when it dies
	const int healthPotionAmount = 25; //health restored by a red potion
	const int manaPotionAmount = 30; //mana restored by a blue potion

	//arrow trap settings (fake pressure plates)
	const int trapArrowDamage = 15; //damage per arrow
	const float trapArrowSpeed = 350.0f; //pixels per second
	const int trapLaunchDistance = 6; //how many tiles away the arrows start from

	//update helpers, called from update()
	void updateProjectiles(Map& map, float deltaTime);
	void updatePickups(GamesEngineeringBase::Window& window, Map& map, float deltaTime);
	void updateLevelObjects(GamesEngineeringBase::Window& window, Map& map, float deltaTime);

	void tryDropPickup(Vector2f pos); //maybe drop a potion where an enemy died
	void openDoorsWithId(int id, Map& map); //opens every door linked to a pressure plate
	void fireArrowTrap(Vector2f target, Map& map); //fake plate: shoot arrows at the target from 4 directions

	void drawProjectiles(GamesEngineeringBase::Window& window, Camera& cam); 

public:
	EntityManager(AssetManager& am, float* _gameTime);
	~EntityManager();

	void clear(); //deletes everything, used when starting a level

	Player* getFirstPlayer()
	{
		if (players.getSize() == 0) return nullptr;
		return players[0];
	}

	bool getAllPlayersDead() const { return allPlayersDead; }
	bool getHasKey() const { return hasKey; }
	bool getLevelComplete() const { return levelComplete; }

	void addPlayer(Player* player) { players.add(player); }
	void addEnemy(Enemy* enemy) { enemies.add(enemy); }
	void addSpawner(Spawner* spawner) { spawners.add(spawner); }
	void addProjectile(const Projectile& projectile) { projectiles.add(projectile); }
	void addPickup(Pickup* pickup) { pickups.add(pickup); }
	void addDoor(Door* door) { doors.add(door); }
	void addPlate(PressurePlate* plate) { plates.add(plate); }
	void addTrap(SpikeTrap* trap) { traps.add(trap); }

	void deletePlayer(Player* player) { players.removeSwap(player); }
	void deleteEnemy(Enemy* enemy) { enemies.removeSwap(enemy); }
	void deleteSpawner(Spawner* spawner) { spawners.removeSwap(spawner); }

	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime);
	void spawnEnemy(Vector2f pos);
	int cleanupKills(); //removes dead entities, returns how many enemies died
	void draw(GamesEngineeringBase::Window& window, Camera& cam);
};
