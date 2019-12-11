#pragma once
#ifndef __MAIN_H__
#define __MAIN_H__
#include <GL/freeglut.h>

#define TITLE_STATE 0
#define STANDBY_STATE 1
#define SELECT_STATE 2
#define ZOOM_IN_STATE 3
#define MAIN_STATE 4
#define DEFEAT_STATE 5
#define WIN_STATE 6
#define REPLAY_STATE 8
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Timerfunction(int value);
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int xp, int yp);
void Control_light();
void NetworkTimer(int value);

GLubyte* LoadDIBitmap(const char* file_name, BITMAPINFO** info);
void Init_Texture(GLubyte* pBytes, BITMAPINFO* info, GLuint* textures);
void Init_EndTexture(GLubyte* pBytes, BITMAPINFO* info, GLuint* textures);
void Draw_Background();
void Draw_Level();
void Sound_SetUp();
void Play_Sound();
void Draw_Title();
void Draw_defeat_state();
void Draw_win_state();
void Draw_select_state();

struct mouse_
{
	double x = 0;
};

#endif // !__MAIN_H__