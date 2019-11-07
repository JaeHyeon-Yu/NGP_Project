#ifndef __TILE_H__
#define __TILE_H__
#include <GL/freeglut.h>

#define EASY_1 1
#define EASY_2 2
#define NORMAL_1 3
#define NORMAL_2 4
#define HARD_1 5
#define HARD_2 6
// 난이도

class Tile
{
private:
	GLUquadric *up_side, *down_side;	// 윗면과 아랫면을 glu객체로 생성
	float x, y, z;
	float half_height;
	int degree;	// 어디에 그려질지 결정해줌
	int state;
	int timer;
	bool blink;

	double r, g, b;

	double Distroy_timer;
	int distroy_degree;
	int rand_degree;

public:
	void Initialize(int degree);
	void Draw_Tile();
	void Draw_Side(int degree);
	void Draw_Surface(double left_degree, double right_degree);
	int Get_state();
	double Your_Radian(int degree);
	void Load_Data(int data);

	void Update();
	void Distroy(int i);
};
#endif // !__TILE_H__