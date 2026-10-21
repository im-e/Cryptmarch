#pragma once
#include "Entity.h"
#include "../Utilities/Vector.h"

class EntityManager {

private:
	Vector<Entity*> players;
	Vector<Entity*> enemies;

public:
	void addPlayer(Entity* player)
	{
		players.add(player);
	}

	void addEnemy(Entity* enemy)
	{
		enemies.add(enemy);
	}

	void deleteEnemy(Entity* enemy)
	{
		enemies.removeSwap(enemy);
	}

	void deletePlayer(Entity* player)
	{
		players.removeSwap(player);
	}

	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) 
	{
		for (int i = 0; i < players.getSize(); i++)
		{
			players[i]->update(window, map, deltaTime);
		}

		for (int i = 0; i < enemies.getSize(); i++)
		{
			enemies[i]->update(window, map, deltaTime);
		}
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