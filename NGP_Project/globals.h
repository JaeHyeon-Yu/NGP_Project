#pragma once
#define SERVERPORT 9000
#define MAX_USERS 2
#define BUFSIZE 512

struct Ball_Packet {
	// Ball Class
	double y;
	int floor;
	int life;
	int state;
	double speed;
	bool camera;
};

struct Tower_Packet {
	// Tower Class
	int current_degree;
	int rotate_degree;
	int tile_index;
	int tower_index;
	// int distroy_timer[12];
	// int distroy_dgree[12];
	Ball_Packet bPack;
};

struct Mouse_Packet {
	bool state;
	int x;
};