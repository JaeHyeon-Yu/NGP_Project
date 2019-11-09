#pragma once
struct Player_Packet {
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