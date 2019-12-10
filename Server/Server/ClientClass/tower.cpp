#include "tower.h"
#include <iostream>
extern int g_gameState;
#define NORMAL_TILE 1

void Tower::Initialize(int state, int idx)
{
	this->state = state;

	if (this->state == EASY_1 || this->state == EASY_2)
		num_of_stages = 20;
	else if (this->state == NORMAL_1 || this->state == NORMAL_2)
		num_of_stages = 30;
	else if (this->state == HARD_1 || this->state == HARD_2)
		num_of_stages = 40;

	stage = new Stage[num_of_stages];
	ball = new Ball(idx);
	current_degree = 0;
	rotate_degree = 0;
	for (int i = 0; i < num_of_stages; i++)
		stage[i].Initialize(i);	// 위로부터 몇번째 층인가

	Load_Map();	// 맵파일 로딩

	index = idx;
}

void Tower::Update()
{
	int current_floor = ball->Get_floor();
	rotate_degree = rotate_degree % 360;

	if (g_gameState == END_STATE) return;
	ball->Update();
	
	for (int i = 0; i < num_of_stages; i++)
	{
		stage[i].Update();
	}
	
	if (ball->Collide(stage[current_floor].Get_y(), stage[current_floor].Get_state_of_tile(current_degree + rotate_degree)) == true)
	{
		stage[current_floor].Distroy();
	}
	

}

void Tower::Rotate_plus()
{
	current_degree++;

}

void Tower::Rotate_minus()
{
	current_degree--;

}

double Tower::Get_ball_y()
{
	return ball->Get_y();
}

bool Tower::Get_ball_camera_follow()
{
	return ball->Get_camera();
}

int Tower::Get_ball_floor()
{
	return ball->Get_floor();
}

void Tower::Rotate_by_mouse(int degree)
{
	rotate_degree = degree;
}

void Tower::Fix_degree()
{
	current_degree = current_degree + rotate_degree;
	rotate_degree = 0;
}

void Tower::Rotate_half()
{
	current_degree += 180;
}

void Tower::Power_overwhelming()
{
	ball->Power_overwhelming();
}
Tower_Packet Tower::MakePacket() {
	Tower_Packet tPacket;
	tPacket.current_degree = current_degree;
	tPacket.rotate_degree = rotate_degree;
	tPacket.ball = ball->MakePacket();
	tPacket.game_state = g_gameState;
	return tPacket;
}
void Tower::Update(int moving) {
	Rotate_by_mouse(moving);
	Update();
}
void Tower::SetState(int s) {
	ball->SetState(s);
}
int Tower::GetDegree() {
	int tower_degree = current_degree + rotate_degree;
	
	for (int i = 0; i < 10; ++i) {
		if (tower_degree >= 0) break;
		else tower_degree += 360;
	}

	for (int i = 0; i < 10; ++i) {
		if (abs(tower_degree) < 360) break;
		else tower_degree = tower_degree % 360;
	}
	std::cout << tower_degree << std::endl;
	return tower_degree;
}
void Tower::SetCollideTile() {
	int floor = ball->Get_floor();
	int collideTileIdx = stage[floor].GetTileIdx(GetDegree());

	stage[floor].Load_Tile_Data(NORMAL_TILE, collideTileIdx);
}
int Tower::GetTileIdx(int sIdx) {
	return stage[sIdx].GetTileIdx(GetDegree());
}
