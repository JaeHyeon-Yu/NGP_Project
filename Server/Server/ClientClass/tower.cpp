#include "tower.h"
#include <iostream>

void Tower::Initialize(int state)
{
	this->state = state;

	if (this->state == EASY_1 || this->state == EASY_2)
		num_of_stages = 20;
	else if (this->state == NORMAL_1 || this->state == NORMAL_2)
		num_of_stages = 30;
	else if (this->state == HARD_1 || this->state == HARD_2)
		num_of_stages = 40;

	stage = new Stage[num_of_stages];
	ball = new Ball();
	current_degree = 0;
	rotate_degree = 0;
	for (int i = 0; i < num_of_stages; i++)
		stage[i].Initialize(i);	// 위로부터 몇번째 층인가

	Load_Map();	// 맵파일 로딩
}

void Tower::Update()
{
	int current_floor = ball->Get_floor();
	rotate_degree = rotate_degree % 360;

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
	Tower_Packet tPacket{ current_degree, rotate_degree, 0, 0, 0,0 };
	return tPacket;
}
