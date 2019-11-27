#include "tile.h"
#include <iostream>
#include <cmath>

#define ANGLE -30
#define LEFT_SIDE_DEGREE 90
#define RIGHT_SIDE_DEGREE 120
#define MID_CTRL_DEGREE 7.5

#define EMPTY_TILE 0
#define NORMAL_TILE 1
#define KILL_TILE 2
#define GRAVITY_FREE_TILE 3
#define BLIND_TILE 4
#define BLINK_TILE 5
#define ROTATE_TILE 6
#define END_TILE 9

const double mid_degree = (LEFT_SIDE_DEGREE + RIGHT_SIDE_DEGREE) / 2;

void Tile::Initialize(int degree)
{
	x = 0, z = 0;
	y = 0;
	r = 0, g = 0, b = 0;
	rand_degree = rand() % 5;
	timer = 0;
	blink = true;
	this->degree = degree;
	half_height = 0.1;

	// up_side = gluNewQuadric();
	// down_side = gluNewQuadric();

	Distroy_timer = 0;
	distroy_degree = 0;
}
double Tile::Your_Radian(int degree)
{
	const double PI = 3.141592;
	return degree * PI / 180;
}

void Tile::Load_Data(int data)
{
	state = data;
	// 데이터값 대입
}


int Tile::Get_state()
{
	return state;
}

void Tile::Update()
{
	
}

void Tile::Distroy(int i)
{
	state = 8;
	distroy_degree += rand_degree;
	double t = Distroy_timer / 100;
	
	z = (2 * t*t - 3 * t + 1) * 0 + (-4 * t*t + 4 * t)*2.7 + (2 * t*t - t) * 4;
	y = (2 * t*t - 3 * t + 1) * 0 + (-4 * t*t + 4 * t)*-2.2 + (2 * t*t - t)*-4;
	Distroy_timer += 2;
	if (Distroy_timer == 100)
	{
		state = 0;
	}
	
}