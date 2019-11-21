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

	up_side = gluNewQuadric();
	down_side = gluNewQuadric();

	Destroy_timer = 0;
	destroy_degree = 0;
}

void Tile::Draw_Tile()
{
	if (state == 5)
	{
		timer++;
		if (timer % 10 == 0)
		{
			if (blink == true)
			{
				blink = false;
			}
			else if (blink == false)
			{
				blink = true;
			}
		}
	}
	switch (state)
	{
	case EMPTY_TILE:
		return;
	case NORMAL_TILE:
		r = 0, g = 0, b = 0;
		break;
	case KILL_TILE:
		r = 1, g = 0, b = 0;
		break;
	case GRAVITY_FREE_TILE:
		r = 0, g = 0.5, b = 0;
		break;
	case BLIND_TILE:
		r = 0.5, b = 0.5, g = 0.5;
		break;
	case BLINK_TILE:
		if (blink == true)
		{
			r = 1, g = 0, b = 0;
		}
		else if (blink == false)
		{
			return;
		}
		break;
	case ROTATE_TILE:
		r = 1, g = 1, b = 0;
		break;
	case END_TILE:
		r = 0, g = 0, b = 1;
		break;
	}

	glPushMatrix();
	glColor3f(r, g, b);
		glRotated(degree, 0, 1, 0);	// y축 회전하여 제자리에 꽂는다
		glTranslated(x, y, z);
		glRotated(destroy_degree, 1, 0, 0);
		glRotated(90, 1, 0, 0);
		glPushMatrix();
		glTranslated(0, 0, half_height);	// 위쪽 먼저 그리기
			gluPartialDisk(up_side, 0, 1.0, 26, 13, 0.0, ANGLE);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 0, -half_height);	// 아래쪽 그리기
			gluPartialDisk(down_side, 0, 1.0, 26, 13, 0.0, ANGLE);
		glPopMatrix();
		

		Draw_Side(LEFT_SIDE_DEGREE);
		Draw_Side(RIGHT_SIDE_DEGREE);
		// 옆면 그리기

		Draw_Surface(LEFT_SIDE_DEGREE, LEFT_SIDE_DEGREE + MID_CTRL_DEGREE * 1);
		Draw_Surface(LEFT_SIDE_DEGREE + MID_CTRL_DEGREE * 1, LEFT_SIDE_DEGREE + MID_CTRL_DEGREE * 2);
		Draw_Surface(LEFT_SIDE_DEGREE + MID_CTRL_DEGREE * 2, LEFT_SIDE_DEGREE + MID_CTRL_DEGREE * 3);
		Draw_Surface(LEFT_SIDE_DEGREE + MID_CTRL_DEGREE * 3, RIGHT_SIDE_DEGREE);
		// 4개의 곡면으로 나누어서 뒤쪽 곡면을 그린다.

	glPopMatrix();
}

void Tile::Draw_Side(int degree)
{
	double radian = Your_Radian(degree);

	glPushMatrix();
	glTranslated(0, 0, 0);
		glBegin(GL_POLYGON);
			glVertex3f(cos(radian), sin(radian), half_height);
			glVertex3f(0.0, 0.0, half_height);
			glVertex3f(0.0, 0.0, -half_height);
			glVertex3f(cos(radian), sin(radian), -half_height);
		glEnd();
	glPopMatrix();
}

void Tile::Draw_Surface(double left_degree, double right_degree)
{
	double radian_L{ Your_Radian(left_degree) }, 
		   radian_R{ Your_Radian(right_degree) };
	double radian_mid = Your_Radian((left_degree + right_degree) / 2);	// 중간 제어점 라디안

	GLfloat ctrlpoints[3][3][3] = { {{cos(radian_L), sin(radian_L), half_height},  {cos(radian_mid), sin(radian_mid), half_height}, {cos(radian_R), sin(radian_R), half_height}},
									{{cos(radian_L), sin(radian_L), 0.0},  {cos(radian_mid), sin(radian_mid), 0.0}, {cos(radian_R), sin(radian_R), 0.0}},
									{{cos(radian_L), sin(radian_L), -half_height},  {cos(radian_mid), sin(radian_mid), -half_height}, {cos(radian_R), sin(radian_R), -half_height}}};
	// 컨트롤포인트 초기화

	glPushMatrix();
	glTranslated(0, 0, 0);
		glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 3, 0.0, 1.0, 9, 3, &ctrlpoints[0][0][0]);
		glEnable(GL_MAP2_VERTEX_3);
		glMapGrid2f(10, 0.0, 1.0, 10, 0.0, 1.0);
		glEvalMesh2(GL_FILL, 0, 10, 0, 10);
		// 곡면그리기
	glPopMatrix();
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

//for i in range(0, 100 + 1, 2) :
//	t = i / 100
//	x = (1 - t)*p1[0] + t * p2[0]
//	y = (1 - t)*p1[1] + t * p2[1]
//	draw_point((x, y))
void Tile::Destroy(int i)
{
	state = 8;
	destroy_degree += rand_degree;
	double t = Destroy_timer / 100;
	std::cout << t << std::endl;
	z = (2 * t*t - 3 * t + 1) * 0 + (-4 * t*t + 4 * t)*2.7 + (2 * t*t - t) * 4;
	y = (2 * t*t - 3 * t + 1) * 0 + (-4 * t*t + 4 * t)*-2.2 + (2 * t*t - t)*-4;
	Destroy_timer += 2;
	if (Destroy_timer == 100)
	{
		state = 0;
	}
	
}