#ifndef __TOWER_H__
#define __TOWER_H__
#include "stage.h"
#include "ball.h"

class Tower
{
private:
	Stage *stage;
	Ball *ball;
	int current_degree;
	int rotate_degree;
	int state;

	int num_of_stages;
public:
	void Load_Map();
	void Initialize(int state);
	void Draw_Tower();

	void Update();
	void Rotate_plus();
	void Rotate_minus();
	void Rotate_by_mouse(int degree);
	void Fix_degree();
	void Rotate_half();

	bool Get_ball_camera_follow();
	double Get_ball_y();
	int Get_ball_floor();
	void Power_overwhelming();
};
#endif // !__TOWER_H__
