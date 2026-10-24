#pragma once
#include "Entity.h"
#include "../Utilities/Vector.h"

class EntityManager {

private:
	Vector<Player*> players;
	Vector<Enemy*> enemies;
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
				if (players[i]->checkCollision(enemies[j]))
				{
					(players[i]->takeDamage(enemies[j]->getDamage()));
				}
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
	
};