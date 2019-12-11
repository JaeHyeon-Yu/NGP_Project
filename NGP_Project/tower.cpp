#include "tower.h"
#include <iostream>
extern int Game_state;
#define NORMAL_TILE 1

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
		stage[i].Initialize(i);	// ���κ��� ���° ���ΰ�

	Load_Map();	// ������ �ε�
}

void Tower::Draw_Tower()//���⼭ Ÿ���� ���� ���� �׸���.
{
	glPushMatrix();
	glTranslated(xpos, 0, 0);

	glPushMatrix();
	glColor3f(1, 1, 1);
	glPushMatrix();
		glRotated(90, 1, 0, 0);
		glutSolidCylinder(0.3, 2.0 * num_of_stages, 20, 20);
	glPopMatrix();
	glRotated(-current_degree - rotate_degree, 0, 1, 0);
	for (int i = 0; i < num_of_stages; i++)
		stage[i].Draw_Stage();
	glPopMatrix();
	ball->Draw();
	// ��ձ׸���

	glPopMatrix();
}


void Tower::Update()
{
	int current_floor = ball->Get_floor();
	rotate_degree = rotate_degree % 360;

	
	for (int i = 0; i < num_of_stages; i++)
	{
		stage[i].Update();
	}
}

//�� �Ʒ��δ� ���� �Լ�, �Ŀ� ������ ����.
double Tower::Get_ball_y()//���� y��ǥ ã��
{
	return ball->Get_y();
}

bool Tower::Get_ball_camera_follow()//���� ī�޶� ���󰡱�
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

Rotate_Packet Tower::MakePacket() {
	Rotate_Packet rPacket{ rotate_degree, current_degree };
	return rPacket;
}
void Tower::Update(Tower_Packet tPacket, int idx) {
	int current_floor = ball->Get_floor();
	current_degree = tPacket.current_degree;
	Rotate_by_mouse(tPacket.rotate_degree);
	ball->Update(tPacket.bPack, idx);
	
	for (int i = 0; i < num_of_stages; i++)
	{
		stage[i].Update();
	}
}
void Tower::StageUpdate(Destroy_Packet dPacket) {
	stage[dPacket.stageIdx].Destroy();
}
int Tower::GetBallLife() {
	return ball->GetLife();
}
void Tower::SetTile(Change_Packet cPack) {
	stage[cPack.stageIdx].Load_Tile_Data(NORMAL_TILE, cPack.tileIdx);
}
int Tower::GetBallState() {
	return ball->GetState();
}