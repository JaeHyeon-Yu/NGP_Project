#ifndef __BALL_H__
#define __BALL_H__
#pragma comment(lib, "winmm")
#include "../globals.h"

class Ball
{
private:
	double x;
	double y;	//�Ƹ� 0.5
	double z;
	double speed;
	int floor;	//���� ���� �ִ� ��
	float size;
	float wire_size;
	bool life;
	bool camera_follow;	//ī�޶󹫺�
	bool immotal;

	double Squeeze_timer;
	int Squeeze;

	struct ink_
	{
		double x1 = 0;
		double y1 = 0;
		double x2 = 0;
		double y2 = 0;
		double x3 = 0;
		double y3 = 0;
		double x4 = 0;
		double y4 = 0;
		double size = 0;
		int timer = 0;
		bool exist = false;
	};
	ink_ ink[3];

	// �߰� ����
	int state;
	int index;

public:
	Ball(int idx);
	bool Collide(int floor, int tile_state);
	void Update();
	double Get_y();
	bool Get_camera();
	int Get_floor();

	// void Draw_ink();

	void Fail();
	void Victory();
	void Power_overwhelming();

	// Server's Function
	Ball_Packet MakePacket();
	void Initialize(int idx);
	void Update(Ball_Packet bPack);
	void SetState(int s) { state = s; };
};

#endif