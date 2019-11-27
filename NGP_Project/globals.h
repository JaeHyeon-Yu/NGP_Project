#pragma once
#define SERVERPORT 9000
#define MAX_USERS 2
#define BUFSIZE 512
enum Ball_State {
	EMPTY, Collide_BLINK, Collide_KILL,
	Collide_NORMAL, Collide_ROTATE, Collide_BLIND,
	Collide_GRAVITY, WIN, LOOSE
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
	Ball_Packet bPack;
};

struct Mouse_Packet {
	bool state;
	int x;
};