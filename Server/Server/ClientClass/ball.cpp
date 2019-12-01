#include "ball.h"
#include "tower.h"
#include <iostream>
extern int g_gameState;
extern Tower g_towerArr[MAX_USERS];
#define GRAVITY 9.8

#define EMPTY_TILE 0
#define NORMAL_TILE 1
#define KILL_TILE 2
#define GRAVITY_FREE_TILE 3
#define BLIND_TILE 4
#define BLINK_TILE 5
#define ROTATE_TILE 6
#define END_TILE 9
int OtherTndex(int myidx) {
	if (myidx == 0) return 1;
	else if (myidx == 1) return 0;
	else return -1;
}
Ball::Ball(int idx)
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
	life = MAX_LIFE;
	immotal = false;
	index = idx;
}

void Ball::Update()
{	
	if (state == WIN || state == LOOSE) g_gameState = END_STATE;
	if (life <= 0) state = LOOSE;
	if (state == WIN) {
		int other = OtherTndex(index);
		if (other == -1) return;

		g_towerArr[other].SetState(LOOSE);
	}
	else if (state == LOOSE) {
		int other = OtherTndex(index);
		if (other == -1) return;

		g_towerArr[other].SetState(WIN);
	}
	if (life > 0)
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

	if (life <= 0)
	{
		wire_size += 0.1;
		if (wire_size > 5)
		{
			Fail();
		}
	}

	// 승리 상태에 들어가면 spd를 0으로 초기화시킨다.
	if (state == WIN) speed = 0;
}

bool Ball::Collide(int floor, int tile_state)
{
	if (g_gameState == END_STATE) return false;
	if (life <= 0) return false;

	if (life > 0)
	{
		if (speed < 0.25)
		{
			if (y < floor + 0.2 && tile_state == NORMAL_TILE)
			{
				speed = -0.1;
				camera_follow = false;
				Squeeze = 2;
				state = Collide_NORMAL;
			}
			else if (y < floor + 0.2 && tile_state == ROTATE_TILE)
			{
				y = floor + 0.2;
				speed = -0.1;
				camera_follow = false;
				Squeeze = 2;
				state = Collide_ROTATE;

				int other = OtherTndex(index);
				if (other == -1) return false;

				g_towerArr[other].SetState(Collide_ROTATE);
				g_towerArr[other].Rotate_half();
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
				state = Collide_BLIND;
			}
			else if (y < floor + 0.4 && tile_state == GRAVITY_FREE_TILE)
			{
				speed = -0.0007*GRAVITY;
				state = Collide_GRAVITY;
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
				state = Collide_KILL;
				speed = -0.1;
				life -= 1;
				
				g_towerArr[index].SetCollideTile();
			}
			else if (y < floor + 0.2 && tile_state == BLINK_TILE)
			{
				if (immotal == false)
				{
					if (life == true)
					{
					}
					speed = 0;
					life -= 1;
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
				state = WIN;
				int other = OtherTndex(index);
				if (other == -1) return false;

				g_towerArr[other].SetState(LOOSE);
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
				state = WIN;
				int other = OtherTndex(index);
				if (other == -1) return false;

				g_towerArr[other].SetState(LOOSE);
				// Victory();
			}
			else if (y < floor + 0.2&&tile_state != EMPTY_TILE)
			{
				speed = -0.1;
				camera_follow = true;
				state = TILE_BREAK;
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
	//Game_state = 4;
}

void Ball::Victory()
{
	//Game_state = 5;
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
	Ball_Packet pPacket{ y, floor, state, speed, camera_follow, life };
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
	if (bPack.state == WIN || bPack.state == LOOSE)
		state = bPack.state;
	else state = EMPTY;
}