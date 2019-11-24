#include "ball.h"
#include "tower.h"
#include <iostream>
int Game_state;
Tower tower;
#define GRAVITY 9.8

#define EMPTY_TILE 0
#define NORMAL_TILE 1
#define KILL_TILE 2
#define GRAVITY_FREE_TILE 3
#define BLIND_TILE 4
#define BLINK_TILE 5
#define ROTATE_TILE 6
#define END_TILE 9

Ball::Ball()
{
	x = 0;
	y = 0.5;
	z = 0.5;
	size = 0.07;
	floor = 0;
	speed = 0;
	Squeeze_timer = 1;
	Squeeze = 0;
	camera_follow = false;
	life = true;
	immotal = false;
	wire_size = 0.07;
}

void Ball::Update()
{
	if (life == true)
	{
		speed += 0.0005*GRAVITY;
		y -= speed;
	}

	for (int i = 0; i < 3; i++)
	{
		if (ink[i].exist == true)
		{
			ink[i].timer++;
			if (ink[i].timer > 60)
			{
				ink[i].timer = 0;
				ink[i].exist = false;
			}
		}
	}

	if (Squeeze == 2)
	{
		Squeeze_timer -= 0.1;
		if (Squeeze_timer < 0.5)
		{
			Squeeze = 1;
		}
	}
	else if (Squeeze == 1)
	{
		Squeeze_timer += 0.1;
		if (Squeeze_timer > 1)
		{
			Squeeze = 0;
			Squeeze_timer = 1;
		}
	}

	if (life == false)
	{
		wire_size += 0.1;
		if (wire_size > 5)
		{
			Fail();
		}
	}
}

bool Ball::Collide(int floor, int tile_state)
{
	if (life == true)
	{
		if (speed < 0.25)
		{
			if (y < floor + 0.2 && tile_state == NORMAL_TILE)
			{
				speed = -0.1;
				camera_follow = false;
				Squeeze = 2;
			}
			else if (y < floor + 0.2 && tile_state == ROTATE_TILE)
			{
				y = floor + 0.2;
				speed = -0.1;
				camera_follow = false;
				Squeeze = 2;
				tower.Rotate_half();
			}

			else if (y < floor + 0.2 && tile_state == BLIND_TILE)
			{
				speed = -0.1;
				camera_follow = false;
				Squeeze = 2;

				for (int i = 0; i < 3; i++)
				{
					if (ink[i].exist == false)
					{
						ink[i].exist = true;
						ink[i].x1 = (rand() / (double)RAND_MAX) * 2 - 1;
						ink[i].y1 = (rand() / (double)RAND_MAX) * 2 - 1;
						ink[i].x2 = (rand() / (double)RAND_MAX) * 2 - 1;
						ink[i].y2 = (rand() / (double)RAND_MAX) * 2 - 1;
						ink[i].x3 = (rand() / (double)RAND_MAX) * 2 - 1;
						ink[i].y3 = (rand() / (double)RAND_MAX) * 2 - 1;
						ink[i].x4 = (rand() / (double)RAND_MAX) * 2 - 1;
						ink[i].y4 = (rand() / (double)RAND_MAX) * 2 - 1;
						break;
					}
				}
			}
			else if (y < floor + 0.4 && tile_state == GRAVITY_FREE_TILE)
			{
				speed = -0.0007*GRAVITY;

			}
			else if (y < floor + 0.1 && tile_state == EMPTY_TILE)
			{
				this->floor += 1;
				camera_follow = true;
			}
			else if (y < floor + 0.1 && tile_state == 8)
			{
				this->floor += 1;
				camera_follow = true;
			}
			else if (y < floor + 0.2 && tile_state == KILL_TILE)
			{
				if (immotal == false)
				{
					if (life == true)
					{
					}
					speed = 0;
					life = false;
				}
				else if (immotal == true)
				{
					speed = -0.1;
					camera_follow = false;
					Squeeze = 2;
				}
			}
			else if (y < floor + 0.2 && tile_state == BLINK_TILE)
			{
				if (immotal == false)
				{
					if (life == true)
					{
					}
					speed = 0;
					life = false;
				}
				else if (immotal == true)
				{
					speed = -0.1;
					camera_follow = false;
					Squeeze = 2;
				}
			}
			else if (y < floor + 0.2 && tile_state == END_TILE)
			{
				Victory();
			}
		}
		if (speed >= 0.25)
		{
			if (y < floor + 0.1 && tile_state == EMPTY_TILE)
			{
				this->floor += 1;
				camera_follow = true;
			}
			else if (y < floor + 0.2 && tile_state == END_TILE)
			{
				Victory();
			}
			else if (y < floor + 0.2&&tile_state != EMPTY_TILE)
			{
				speed = -0.1;
				camera_follow = true;
				return true;
			}
		}
	}

			
	return false;
}

int Ball::Get_floor()
{
	return floor;
}

double Ball::Get_y()
{
	return y;
}

bool Ball::Get_camera()
{
	return camera_follow;
}

void Ball::Fail()
{
	Game_state = 4;
}

void Ball::Victory()
{
	Game_state = 5;
}

void Ball::Power_overwhelming()
{
	if (immotal == true)
	{
		immotal = false;
	}
	else if (immotal == false)
	{
		immotal = true;
	}
}
Ball_Packet Ball::MakePacket() {
	Ball_Packet pPacket{ y, floor, life, state, speed, camera_follow };
	return pPacket;
}
void Ball::Initialize(int idx) {
	if (idx == 0) {
		x = player1_x;
		y = player1_y;
		z = player1_z;
	}
	else if (idx == 1) {
		x = player2_x;
		y = player2_y;
		z = player2_z;
	}
}
void Ball::Update(Ball_Packet bPack) {

}