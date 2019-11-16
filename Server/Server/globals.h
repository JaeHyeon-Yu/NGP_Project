#pragma once
#define SERVERPORT 9000
#define BUFSIZE 512
#define MAX_USERS 2

enum POS_INFO {
	player1_x, player1_y, player1_z,
	player2_x, player2_y, player2_z
};

struct Ball_Packet {
	// Ball Class
	double y;
	int floor;
	int life;
	int state;
	int player_index;
};

struct Tower_Packet {
	// Tower Class
	int current_degree;
	int rotate_degree;
	int tile_index;
	int tower_index;
	int distroy_timer[12];
	int distroy_dgree[12];
};

struct Mouse_Packet {
	bool state;
	int x;
};