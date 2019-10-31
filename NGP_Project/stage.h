#ifndef __STAGE_H__
#define __STAGE_H__
#include "tile.h"
#define NUM_OF_TILES 12

class Stage
{
private:
	Tile *tile;
	int floor;
	float floor_height;
	int y;

	bool distroyed_stage;
public:
	void Initialize(int floor);
	void Draw_Stage();
	int Get_y();
	int Get_state_of_tile(int tower_drgree);
	void Load_Tile_Data(int data, int n);

	void Distroy();
	void Update();
};

#endif // !__STAGE_H__
