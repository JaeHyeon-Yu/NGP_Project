#include "stage.h"
#include <iostream>
#define DISTANCE_OF_FLOOR 2


void Stage::Initialize(int floor)
{
	this->floor = floor;
	floor_height = 1.0;
	double degree = 360.0 / NUM_OF_TILES;
	tile = new Tile[NUM_OF_TILES];
	y = -DISTANCE_OF_FLOOR * floor;

	for(int i=0; i<NUM_OF_TILES; i++)
		tile[i].Initialize(i*degree);


	distroyed_stage = false;

}
void Stage::Load_Tile_Data(int data, int n)
{
	tile[n].Load_Data(data);
	// 타일에 데이터 삽입
}


int Stage::Get_y()
{
	return y;
}

int Stage::Get_state_of_tile(int tower_degree)
{
	tower_degree = ((tower_degree % 360) + 360) % 360;
	
	if (330 <= tower_degree && tower_degree < 360)
	{
		return tile[0].Get_state();
	}
	else if (0 <= tower_degree && tower_degree < 30)
	{
		return tile[1].Get_state();
	}
	else if (30 <= tower_degree && tower_degree < 60)
	{
		return tile[2].Get_state();
	}
	else if (60 <= tower_degree && tower_degree < 90)
	{
		return tile[3].Get_state();
	}
	else if (90 <= tower_degree && tower_degree < 120)
	{
		return tile[4].Get_state();
	}
	else if (120 <= tower_degree && tower_degree < 150)
	{
		return tile[5].Get_state();
	}
	else if (150 <= tower_degree && tower_degree < 180)
	{
		return tile[6].Get_state();
	}
	else if (180 <= tower_degree && tower_degree < 210)
	{
		return tile[7].Get_state();
	}
	else if (210 <= tower_degree && tower_degree < 240)
	{
		return tile[8].Get_state();
	}
	else if (240 <= tower_degree && tower_degree < 270)
	{
		return tile[9].Get_state();
	}
	else if (270 <= tower_degree && tower_degree < 300)
	{
		return tile[10].Get_state();
	}
	else if (300 <= tower_degree && tower_degree < 330)
	{
		return tile[11].Get_state();
	}

	return 0;
}

void Stage::Distroy()
{
	distroyed_stage = true;
}

void Stage::Update()
{
	for (int i = 0; i < NUM_OF_TILES; i++)
	{
		tile[i].Update();
		if (distroyed_stage == true && tile[i].Get_state() != 0)
		{
			tile[i].Distroy(i);
		}
	}
}
int Stage::GetTileIdx(int tower_degree)
{
	tower_degree = ((tower_degree % 360) + 360) % 360;

	if (330 <= tower_degree && tower_degree < 360) return 0;
	else if (0 <= tower_degree && tower_degree < 30) return 1;
	else if (30 <= tower_degree && tower_degree < 60) return 2;
	else if (60 <= tower_degree && tower_degree < 90) return 3;
	else if (90 <= tower_degree && tower_degree < 120) return 4;
	else if (120 <= tower_degree && tower_degree < 150) return 5;
	else if (150 <= tower_degree && tower_degree < 180) return 6;
	else if (180 <= tower_degree && tower_degree < 210) return 7;
	else if (210 <= tower_degree && tower_degree < 240) return 8;
	else if (240 <= tower_degree && tower_degree < 270) return 9;
	else if (270 <= tower_degree && tower_degree < 300) return 10;
	else if (300 <= tower_degree && tower_degree < 330) return 11;

	return -1;
}