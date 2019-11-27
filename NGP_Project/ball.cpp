#include "ball.h"
#include "tower.h"
#include <iostream>
extern int Game_state;
extern Tower tower;
extern int g_myIdx;
#define GRAVITY 9.8

#define EMPTY_TILE 0
#define NORMAL_TILE 1
#define KILL_TILE 2
#define GRAVITY_FREE_TILE 3
#define BLIND_TILE 4
#define BLINK_TILE 5
#define ROTATE_TILE 6
#define END_TILE 9

//현재 좌표계는 -1.0부터 1.0사이
//어디까지 이 좌표계를 이용하는지 찾아봐야 한다.
//현재 Ball클래스는 전부 이 좌표계를 기준으로 동작한다.

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
	camera_follow = false; // 어지럼증을 방지하기 위함인듯한 변수, 항상 참으로 놓으면 카메라가 공을 따라 위아래로 계속 움직인다.
	life = true;
	immotal = false;
	wire_size = 0.07;
}

void Ball::Draw()
{
	Draw_ink();
	glPushMatrix();
	{
		if (immotal == false)
		{
			glColor3f(1, 0.9 - (speed * 2), 0.9 - (speed * 2));
		}
		else if (immotal == true)
		{
			glColor3f(0.5 - (speed * 2), 0.5 - (speed * 2),1);
		}
		glTranslated(x, y, z);
		glScaled(1, Squeeze_timer, 1);
		glutSolidSphere(size, 10, 10);
		glutWireSphere(wire_size, 20, 20);
	}
	glPopMatrix();
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

void Ball::Draw_ink()
{
	glPushMatrix();
	{
		glLoadIdentity();
		for (int j = 0; j < 3; j++)
		{
			if (ink[j].exist == true)
			{
				glBegin(GL_POLYGON);
				for (int i = 0; i < 360; i++)
				{
					glColor3f(0, 0, 0);
					glVertex3f(ink[j].x1 + 0.1*cos(i), ink[j].y1 + 0.1*sin(i), 0);
				}
				glEnd();
				glBegin(GL_POLYGON);
				for (int i = 0; i < 360; i++)
				{
					glColor3f(0, 0, 0);
					glVertex3f(ink[j].x2 + 0.2*cos(i), ink[j].y2 + 0.2*sin(i), 0);
				}
				glEnd();
				glBegin(GL_POLYGON);
				for (int i = 0; i < 360; i++)
				{
					glColor3f(0, 0, 0);
					glVertex3f(ink[j].x3 + 0.3*cos(i), ink[j].y3 + 0.3*sin(i), 0);
				}
				glEnd();
				glBegin(GL_POLYGON);
				for (int i = 0; i < 360; i++)
				{
					glColor3f(0, 0, 0);
					glVertex3f(ink[j].x4 + 0.4*cos(i), ink[j].y4 + 0.4*sin(i), 0);
				}
				glEnd();
			}
		}
	}
	glPopMatrix();
}

void Ball::Fail()
{
	PlaySound("Sound/fail.wav", NULL, SND_ASYNC);
	Game_state = 4;
}

void Ball::Victory()
{
	PlaySound("Sound/win.wav", NULL, SND_ASYNC);
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
	Ball_Packet bPacket{ y, floor, state, speed, camera_follow };
	bPacket.state = state;
	return bPacket;
}
void Ball::Update(Ball_Packet bPack, int idx) {
	y = bPack.y;
	floor = bPack.floor;
	state = bPack.state;
	speed = bPack.speed;
	camera_follow = bPack.camera;

	if (idx == g_myIdx) PlaySoundEffect();
}
void Ball::PlaySoundEffect() {
	if (state == WIN) 
		PlaySound("Sound/win.wav", NULL, SND_SYNC);
	if (state == Collide_NORMAL) 
		PlaySound("Sound/bounce.wav", NULL, SND_ASYNC);
	if (state == Collide_ROTATE)
		PlaySound("Sound/rotate.wav", NULL, SND_ASYNC);
	if (state == Collide_BLIND)
		PlaySound("Sound/spit.wav", NULL, SND_ASYNC);
	if (state == Collide_KILL)
		PlaySound("Sound/fail.wav", NULL, SND_ASYNC);

	state = 0;
}