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
	life = 5;
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

}

void Ball::Victory()
{
	PlaySound("Sound/win.wav", NULL, SND_ASYNC);

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
	Ball_Packet bPacket{ y, floor, state, speed, camera_follow, life };
	bPacket.state = state;
	return bPacket;
}
void Ball::Update(Ball_Packet bPack, int idx) {
	y = bPack.y;
	floor = bPack.floor;
	state = bPack.state;
	speed = bPack.speed;
	camera_follow = bPack.camera;
	life = bPack.life;
	if (idx == g_myIdx) PlaySoundEffect();

	// 임시로 state의 값을 클라이언트에서 바꿔준다. 
	// 추후 GameState를 통한 예외처리로 수정하여 클라이언트에서 값을 변경하지 않도록 할것
	if (state == BLIND && idx == g_myIdx) {
		for (int i = 0; i < 3; i++)
		{
			if (ink[i].exist == false)
			{
				ink[i].exist = true;
				ink[i].x1 = (rand() / (double)RAND_MAX) * 2 + (3 * idx - 2);
				ink[i].y1 = (rand() / (double)RAND_MAX) * 2 - 1;
				ink[i].x2 = (rand() / (double)RAND_MAX) * 2 + (3 * idx - 2);
				ink[i].y2 = (rand() / (double)RAND_MAX) * 2 - 1;
				ink[i].x3 = (rand() / (double)RAND_MAX) * 2 + (3 * idx - 2);
				ink[i].y3 = (rand() / (double)RAND_MAX) * 2 - 1;
				ink[i].x4 = (rand() / (double)RAND_MAX) * 2 + (3 * idx - 2);
				ink[i].y4 = (rand() / (double)RAND_MAX) * 2 - 1;
				break;
			}
		}
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
}
void Ball::PlaySoundEffect() {
	if (Game_state == END_STATE) return;

	if (state == WIN)
		PlaySound("Sound/win.wav", NULL, SND_ASYNC);
	if (state == LOOSE) 
		PlaySound("Sound/fail.wav", NULL, SND_ASYNC);
	if (state == Collide_NORMAL)
		PlaySound("Sound/bounce.wav", NULL, SND_ASYNC);
	if (state == ROTATE)
		PlaySound("Sound/rotate.wav", NULL, SND_ASYNC);
	if (state == BLIND)
		PlaySound("Sound/spit.wav", NULL, SND_ASYNC);
	if (state == Collide_KILL || state == Collide_BLINK)
		PlaySound("Sound/die.wav", NULL, SND_ASYNC);
	if (state == TILE_BREAK)
		PlaySound("Sound/break.wav", NULL, SND_ASYNC);
}