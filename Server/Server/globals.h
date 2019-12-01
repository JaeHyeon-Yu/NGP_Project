#pragma once
#define SERVERPORT 9000
#define BUFSIZE 512
#define MAX_USERS 2
#define MAX_LIFE 5
// ===========Game State===========//
#define TITLE_STATE 0
#define STANDBY_STATE 1
#define SELECT_STATE 2
#define ZOOM_IN_STATE 3
#define MAIN_STATE 4
#define DEFEAT_STATE 5
#define WIN_STATE 6
#define END_STATE 7
// ================================//

enum POS_INFO {
	player1_x, player1_y, player1_z,
	player2_x, player2_y, player2_z
};
enum Ball_State {
	EMPTY, Collide_BLINK, Collide_KILL,
	Collide_NORMAL, Collide_ROTATE, Collide_BLIND,
	Collide_GRAVITY, WIN, LOOSE, TILE_BREAK, END, STANDBY_END
};
struct Ball_Packet {
	// Ball Class
	double y;
	int floor;
	int state;
	double speed;
	bool camera;
	int life;
};

struct Tower_Packet {
	// Tower Class
	int current_degree;
	int rotate_degree;
	Ball_Packet ball;
	int game_state;
};

struct Mouse_Packet {
	bool state;
	int x;
};

struct Destroy_Packet {
	int towerIdx;
	int stageIdx;
};

struct Change_Packet {
	int stageIdx;
	int tileIdx;
};