#pragma once
#define SERVERPORT 9000
#define BUFSIZE 512
#define MAX_USERS 2

enum POS_INFO {
	player1_x, player1_y, player1_z,
	player2_x, player2_y, player2_z
};
enum Ball_State {
	EMPTY, Collide_BLINK, Collide_KILL,
	Collide_NORMAL, Collide_ROTATE, Collide_BLIND,
	Collide_GRAVITY, WIN, LOOSE, TILE_BREAK, END
};
struct Ball_Packet {
	// Ball Class
	double y;
	int floor;
	int state;
	double speed;
	bool camera;
};

struct Tower_Packet {
	// Tower Class
	int current_degree;
	int rotate_degree;
	Ball_Packet ball;
};

struct Mouse_Packet {
	bool state;
	int x;
};
