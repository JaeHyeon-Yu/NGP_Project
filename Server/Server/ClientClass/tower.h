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

	int xpos;
	int index;
public:
	void Load_Map();
	void Initialize(int state, int idx);

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

	// Server's Function
	Tower_Packet MakePacket();
	void Update(Tower_Packet tPacket);
	void SetState(int s);
	int GetDegree();
	void SetCollideTile();
	int GetTileIdx(int);
};
#endif // !__TOWER_H__
