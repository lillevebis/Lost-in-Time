#ifndef LAYER_H
#define LAYER_H

#include "object.h"

// Class representing a tile layer
class Layer : public Object
{
	// Map needs to access protected/private data
	friend class Map;
    friend class Game;

public:
    // Constructor
	Layer(TileSize tileSize) : Object(tileSize) { }

	void draw(sf::RenderWindow& window);

	int collidableID;

	bool collidable = false;


protected:
	int count = 0;
    // Lazy, but ram is cheap!
    int tilemap[1000][1000];


    // Size in tiles
    int width, height;
};

#endif
