#include <GL/freeglut.h>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include "main.h"
#include "tower.h"

#define DISTANCE_OF_FLOOR 2

int level = 1;
int Game_state = TITLE_STATE;	//0 = 타이틀 1 = 고르기 2 = 줌인 3 = 게임 4 = 패배 5 = 승리
Tower tower;
mouse_ first_click;
mouse_ moved_mouse;
bool drag = false;
double zoom = 0;
int timer = 0;
int rotate_timer = 0;

GLubyte *pBytes;
BITMAPINFO *info;
GLuint textures[2];

bool bmp_load = false;

void main(int argc, char *argv[])
{
	//초기화 함수들
	tower.Initialize(EASY_1);
	Sound_SetUp();
	Play_Sound();
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정	// 더블버퍼링 && 3차원으로 그린다.
	glutInitWindowPosition(100, 100); // 윈도우의 위치지정
	glutInitWindowSize(800, 800); // 윈도우의 크기 지정
	glutCreateWindow("Spyral Helix"); // 윈도우 생성 (윈도우 이름)
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수의 지정
	glutMouseFunc(Mouse);	// 마우스 입력
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);		// 키보드 입력
	glutTimerFunc(100/3, Timerfunction, 1);
	glutMainLoop();
}
// 윈도우 출력 함수
GLvoid drawScene(GLvoid)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 바탕색 깜장색! 깜장색!
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기, 깊이버퍼 클리어

	if(!bmp_load)	
		Init_Texture(pBytes, info, textures);
	bmp_load = true;

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	GLfloat specref[] = { 1.0f,1.0f,1.0f,1.0f };

	glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	glMateriali(GL_FRONT, GL_SHININESS, 64);

	glEnable(GL_AUTO_NORMAL);

	if (Game_state == TITLE_STATE)
		Draw_Title();
	else
	{
		Draw_Background();
		glPushMatrix();
		{
			glTranslated(0, 0.5, -1);
			glRotated(30, 1, 0, 0);
		
			if (tower.Get_ball_camera_follow() == true)
			{
				glTranslated(0, -tower.Get_ball_y(), 0);
			}
			else
			{
				glTranslated(0, -tower.Get_ball_floor()*-DISTANCE_OF_FLOOR, 0);
			}
			if (Game_state == SELECT_STATE || Game_state == ZOOM_IN_STATE)
			{
				glRotated(rotate_timer - zoom * rotate_timer/20, 0, 1, 0);
				glRotated(15 - zoom*15/20, 0, 0, 1);
				glTranslated(0, 1.5 - zoom * 0.075, 0);
				glScaled(0.2 + zoom * 0.04, 0.05 + zoom * 0.0475, 0.2 + zoom * 0.04);
			}
			Control_light();
			tower.Draw_Tower();
		}
		glPopMatrix();

		Draw_Level();
		if (Game_state == DEFEAT_STATE)
		{
			Draw_defeat_state();
		}
		else if (Game_state == WIN_STATE)
		{
			Draw_win_state();
		}
		else if (Game_state == SELECT_STATE)
		{
			Draw_select_state();
		}
	}
	glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60.0, w / h, 1, 200.0);
	glTranslated(0.0, -1.0, -3);

	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);	// 은면제거!
	glEnable(GL_AUTO_NORMAL);
	glLoadIdentity();
}
void Timerfunction(int value)
{
	if (Game_state == TITLE_STATE)
	{
		timer++;
		if (timer == 20)
		{
			Game_state = SELECT_STATE;
			timer = 0;
		}
	}
	if (Game_state == ZOOM_IN_STATE)
	{
		zoom += 1;
		if (zoom > 19)
		{
			Game_state = MAIN_STATE;
			zoom = 0;
		}
	}

	if (Game_state == SELECT_STATE)
	{
		rotate_timer += 3;
		rotate_timer = rotate_timer % 360;
	}

	if (Game_state == MAIN_STATE)
	{
		tower.Update();
	}

	glutPostRedisplay();	// 화면 재출력
	glutTimerFunc(100/3, Timerfunction, 1);
}
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		if (Game_state == SELECT_STATE)
		{
			level = EASY_1;
			tower.Initialize(EASY_1);
		}
		break;
	case'2':
		if (Game_state == SELECT_STATE)
		{
			level = EASY_2;
			tower.Initialize(EASY_2);
		}
		break;
	case'3':
		if (Game_state == SELECT_STATE)
		{
			level = NORMAL_1;
			tower.Initialize(NORMAL_1);
		}
		break;
	case'4':
		if (Game_state == SELECT_STATE)
		{
			level = NORMAL_2;
			tower.Initialize(NORMAL_2);
		}
		break;
	case'5':
		if (Game_state == SELECT_STATE)
		{
			level = HARD_1;
			tower.Initialize(HARD_1);
		}
		break;
	case'6':
		if (Game_state == SELECT_STATE)
		{
			level = HARD_2;
			tower.Initialize(HARD_2);
		}
		break;
	case's':
		if (Game_state == SELECT_STATE)
		{
			Game_state = ZOOM_IN_STATE;
		}
		break;
	case'x':
		if (Game_state == DEFEAT_STATE || Game_state == WIN_STATE || Game_state == MAIN_STATE)
		{
			Game_state = SELECT_STATE;
			tower.Initialize(level);
		}
		break;
	case'r':
		if (Game_state == DEFEAT_STATE || Game_state == WIN_STATE || Game_state == MAIN_STATE)
		{
			tower.Initialize(level);
			Game_state = ZOOM_IN_STATE;
		}
		break;
	case'p':
		tower.Power_overwhelming();
		break;
	}
	glutPostRedisplay();	// 화면 재출력!
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		drag = true;
		first_click.x = x;
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		drag = false;
		tower.Fix_degree();
	}
}

void Motion(int xp, int yp)
{
	moved_mouse.x = xp;

	if (drag == true)
	{
		int moving = moved_mouse.x - first_click.x;
		tower.Rotate_by_mouse(-moving);
	}

}

void Control_light()
{
	GLfloat AmbientLight[] = { 0.2f,  0.2f,  0.2f,  1.0f };
	GLfloat DiffuseLight[] = { 1.0, 1 , 1, 1.0 };
	GLfloat SpecularLight[] = { 0.5,0.5,0.5,1.0 };
	GLfloat lightPos_1[] = { 1, 1, 1, 1.0 };

	GLfloat AmbientLight2[] = { 0.2f,  0.2f ,  0.2f,  1.0f };
	GLfloat DiffuseLight2[] = { 1 , 1 , 1.0 , 1.0 };
	GLfloat SpecularLight2[] = { 0.5 ,0.5 ,0.5 ,1.0 };
	GLfloat lightPos_2[] = { -1, 1, 1, 1.0 };

	

	GLfloat gray[] = { 0.2f,0.2f,0.2f,1.0f };


	glEnable(GL_LIGHTING);

	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos_1);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, AmbientLight2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, DiffuseLight2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, SpecularLight2);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos_2);
	glEnable(GL_LIGHT1);

	
	
}
void Draw_Background()
{
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexGeni(GL_S, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glPushMatrix();
	glTranslated(0, 0, -10);

	glBegin(GL_POLYGON);
	glNormal3f(0, 0, 1);
		glColor3f(1, 1, 1);
		glTexCoord2i(0, 1);
		glVertex3f(-10, 10, 0);

		glTexCoord2i(1, 1);
		glVertex3f(10, 10, 0);

		glTexCoord2i(1, 0);
		glVertex3f(10, -10, 0);

		glTexCoord2i(0, 0);
		glVertex3f(-10, -10, 0);
	glEnd();

	glPopMatrix();
}
void Draw_Level()
{
	char str[20];

	switch (level)
	{
	case EASY_1:
		strcpy_s(str, "Level : EASY_1");
		break;
	case EASY_2:
		strcpy_s(str, "Level : EASY_2");
		break;
	case NORMAL_1:
		strcpy_s(str, "Level : NORMAL_1");
		break;
	case NORMAL_2:
		strcpy_s(str, "Level : NORMAL_2");
		break;
	case HARD_1:
		strcpy_s(str, "Level : HARD_1");
		break;
	case HARD_2:
		strcpy_s(str, "Level : HARD_2");
		break;
	}

	glEnable(GL_COLOR_MATERIAL);
	glColor3f(0, 0, 0);
	glRasterPos2f(1, 2.5);

	int len = (int)strlen(str);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
}
void Draw_Title()
{
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexGeni(GL_S, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

	glPushMatrix();
	glTranslatef(0, 1, 0);
	glScaled(2, 2, 2);
		glBegin(GL_POLYGON);
			glColor3f(1, 1, 1);
			glTexCoord2i(0, 1);
			glVertex3f(-1, 1, 0);

			glTexCoord2i(1, 1);
			glVertex3f(1, 1, 0);

			glTexCoord2i(1, 0);
			glVertex3f(1, -1, 0);

			glTexCoord2i(0, 0);
			glVertex3f(-1, -1, 0);

		glEnd();
	glPopMatrix();
}

void Draw_defeat_state()
{
	char str[] ="Press 'r' To Restart";
	//strcpy_s(str, "");
	

	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1, 0, 0);
	glRasterPos2f(-0.5, 1);

	int len = (int)strlen(str);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);

	char str2[] = "Hold 'x' to Pay Respects";
	//strcpy_s(str, "");


	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1, 0, 0);
	glRasterPos2f(-0.5, 1.2);

	int len2 = (int)strlen(str2);
	for (int i = 0; i < len2; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str2[i]);
}

void Draw_win_state()
{
	char str[] = "Press 'r' To Restart";
	//strcpy_s(str, "");

	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1, 0, 0);
	glRasterPos2f(-0.5, 1.2);

	int len = (int)strlen(str);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);


	char str2[] = "Press 'x' To Title";
	//strcpy_s(str, "");

	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1, 0, 0);
	glRasterPos2f(-0.5, 1.4);

	int len2 = (int)strlen(str2);
	for (int i = 0; i < len2; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str2[i]);

}

void Draw_select_state()
{
	char str[] = "Press Number To Select Level";
	//strcpy_s(str, "");

	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1, 0, 0);
	glRasterPos2f(-0.5, 2.2);

	int len = (int)strlen(str);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);

	char str2[] = "Press 's' To Start";
	//strcpy_s(str, "");

	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1, 0, 0);
	glRasterPos2f(-0.5, 2.1);

	int len2 = (int)strlen(str2);
	for (int i = 0; i < len2; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str2[i]);
}