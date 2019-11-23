#pragma comment(lib,"ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <GL/freeglut.h>
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include "main.h"
#include "tower.h"
#include "globals.h"

//�� �׷��ϱ� ���� openGL��ǥ��� �׸� �� �ۿ� ���� ������ ������ ���� ������ �ʾƼ��ε�,
//������ � ���� ��������� �����ε�, �̰� ��ǥ�� ��ȯ�� �� �ϴٺ��� ���� �� �ΰ� ����.
//�׷��� ���� �񶧸��Ե� �׸��� ���ؼ� -1.0���� 1.0������ ���� �����ϰ� �ȴ�.
//�̰� ��� ��ȯ�ظԾ���� �ϳ�

//�ϴ� ������ ũ�Ⱑ ��� ���������� �����ְ�
//�� ������ Ÿ���� ũ��� ���� ũ�⸦ ���� ��ǥ�� �������� �����ְ�
//�� ������ �׸��°� ������ �̰���;;


// -1~0 1P, 0~1 2
#define DISTANCE_OF_FLOOR 2

int level = 1;
int Game_state = TITLE_STATE;	//0 = Ÿ��Ʋ 1 = ���� 2 = ���� 3 = ���� 4 = �й� 5 = �¸�
Tower tower;
mouse_ first_click;
mouse_ moved_mouse;
bool drag = false;
double zoom = 0;
int timer = 0;
int rotate_timer = 0;

WSADATA wsa;
SOCKET sock;

GLubyte *pBytes;
BITMAPINFO *info;
GLuint textures[2];

bool bmp_load = false;

void InitConnect(WSADATA wsa, SOCKET sock, int GameState);

// For Network
Tower g_towers[MAX_USERS];
int g_myIdx = NULL;
// Ball g_balls[MAX_USERS]

//���� �Լ� ���� ��� �� ����
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

//���� �Լ� ���� ���
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << "[" << msg << "] " << (char*)lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
}


void main(int argc, char *argv[])
{
	//InitConnect(wsa, sock, Game_state);
	//�ʱ�ȭ �Լ���
	// tower.Initialize(EASY_1);
	// Test Code
	for (int i = 0; i < MAX_USERS; ++i)
		g_towers[i].Initialize(HARD_2);
	Sound_SetUp();
	Play_Sound();
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // ���÷��� ��� ����	// ������۸� && 3�������� �׸���.
	glutInitWindowPosition(100, 100); // �������� ��ġ����
	glutInitWindowSize(1600, 800); // �������� ũ�� ����
	glutCreateWindow("Spyral Helix"); // ������ ���� (������ �̸�)
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ��� ����
	glutMouseFunc(Mouse);	// ���콺 �Է�
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);		// Ű���� �Է�
	glutTimerFunc(100/3, Timerfunction, 1);
	glutMainLoop();
}
// ������ ��� �Լ�
GLvoid drawScene(GLvoid)
{
	//main�� �ƴ϶� ���⼭ �� �Ѵ�. �׸��⳪ ���� ���.
	//Game_state�� ���� �׷����°� �޶����� ����.
	//���⼱ �ܼ��� �׷����°� �޶������� ������ ���� ���� �ְ�޴� �����Ͱ� �ٲ� �ʿ伺�� �ִ�.
	//������ ������ �ʿ��ϱ� ������ Game_state�Լ��� �ٸ� �Լ����� ���� ������ �ʿ䰡 �ִ�.
	//�ݵ�� ������ ����->������ ���̾�� �ϱ� ������ ������� �ʿ��ϴ�.
	//�ݵ�� ������� ���� �ʿ䰡 �ִ°�?
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // ������ �����! �����!
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�, ���̹��� Ŭ����

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
	// else if (Game_state == STANDBY_STATE)
	// 	InitConnect(wsa, sock, Game_state);
	else
	{
		//���⵵ -1.0�� 1.0������ ��ǥ�踦 �̿��Ѵ�.
		Draw_Background();
		glPushMatrix();
		{
			glTranslated(0, 0.5, -1);
			glRotated(30, 1, 0, 0);
			// 1�ο��϶��� ����� ���µ� 2�ο����� ���ʿ� �׷����Բ� �ϴϱ� �� �̻��ϰ� �׷�����.
			// �ϴ� �ּ�ó���ϰ� ���߿� �����ؾ���

			
			if (g_towers[g_myIdx].Get_ball_camera_follow() == true)
			{
				glTranslated(0, -g_towers[g_myIdx].Get_ball_y(), 0);
			}
			else
			{
				//????? �̰� ����
				//ī�޶� ���󰡴� �Ŵ�.
				glTranslated(0, -g_towers[g_myIdx].Get_ball_floor()*-DISTANCE_OF_FLOOR, 0);
			}
			if (Game_state == SELECT_STATE || Game_state == ZOOM_IN_STATE)
			{
				//Ȯ���ϴ� �� ��� ó���ؾ� �ұ�....
				glRotated(rotate_timer - zoom * rotate_timer/20, 0, 1, 0);
				glRotated(15 - zoom*15/20, 0, 0, 1);
				glTranslated(0, 1.5 - zoom * 0.075, 0);
				glScaled(0.2 + zoom * 0.04, 0.05 + zoom * 0.0475, 0.2 + zoom * 0.04);
			}
			
			g_towers[0].SetXpos(-2.f);
			g_towers[1].SetXpos(2.f);

			for (int i = 0; i < MAX_USERS; ++i) {
				Control_light();
				g_towers[i].Draw_Tower();
			}
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
	glEnable(GL_DEPTH_TEST);	// ��������!
	glEnable(GL_AUTO_NORMAL);
	glLoadIdentity();
}

void Timerfunction(int value)
{
	if (Game_state == TITLE_STATE) //Ÿ��Ʋ ���´� �ð��� ������ ��ȯ�ȴ�.
	{
		timer++;
		if (timer == 20)
		{
			Game_state = STANDBY_STATE;
			timer = 0;
		}
	}
	if (Game_state == ZOOM_IN_STATE) //Ÿ�̸� ���� ��ſ� zoom������ �̿��� Ȯ���Ų �� ���� ���·� ����.
	{
		zoom += 1;
		if (zoom > 19)
		{
			Game_state = MAIN_STATE;
			zoom = 0;
		}
	}

	if (Game_state == SELECT_STATE) //���� â���� ��ü ���� ȸ����Ű�� ���� rotate_timer�� ����ߴ�.
	{
		rotate_timer += 3;
		rotate_timer = rotate_timer % 360;
	}

	if (Game_state == MAIN_STATE) //���� ���¿��� Ÿ���� ������ �������ش�.
	{
		tower.Update();
	}

	glutPostRedisplay();	// ȭ�� �����
	glutTimerFunc(100/3, Timerfunction, 1);
}

void Keyboard(unsigned char key, int x, int y)
{
	//���⼱ Game_state�� �ٲ� �� ������, ������ �������� ������ ���̱⿡ ���⵵ �����Ѵ�.
	//��� �ٵ� �׷��� �Ǹ� ������ �����ؾ��� ������ �������� ��������!
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
			Game_state = ZOOM_IN_STATE; //�� �� zoom������ �̿��Ѵ�. Ȯ�뿡 ���.
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
	glutPostRedisplay();	// ȭ�� �����!
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
		g_towers[g_myIdx].Rotate_by_mouse(-moving);
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
	glRasterPos2f(1, 2.5);//�ƴ� �̰� �� �� �Լ���

	int len = (int)strlen(str);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
}

void Draw_Title()
{
	//��͵� �׸��� �Ŵϱ� -1.0�� 1.0 ����
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

void InitConnect(WSADATA wsa, SOCKET sock, int GameState)
{
	int retval;
	std::string serverip;

	std::cout << "������ IP�ּҸ� �Է��ϼ���. : ";
	std::getline(std::cin, serverip);

	//���� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)return;

	//socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)err_quit("socket()");

	//connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(serverip.c_str());
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)err_quit("connect()");

	// �ʱ� �����͸� Ŭ���� ������ �޾ƿ´�.
	retval = recv(sock, (char*)& g_towers, sizeof(g_towers), 0);
	if (retval == SOCKET_ERROR) err_display("recv()");
	retval = recv(sock, (char*)& g_myIdx, sizeof(int), 0);
	if (retval == SOCKET_ERROR) err_display("recv()");

	Game_state = EASY_1;
}

//template<class T>
//bool recvn(SOCKET client_sock, const char* name, T File)//���ø����� ���ϰ� �޾ƿ´�
//{
//	int retval;	//return value
//	char buf[1025];	//����[ũ��]
//	size_t len{ 0 };	//������ �� ũ�⸦ �����ϴ� ����
//
//	//���� ���� ����
//	retval = recv(client_sock, (char*)&len, sizeof(size_t), 0);
//
//	if (retval == SOCKET_ERROR)
//	{
//		err_quit("recv()");
//		return false;
//	}
//	else if (retval == 0)
//		return false;
//
//	size_t remainData{ len };	//�����ִ� �������� ũ��(�ʱ�ȭ �ÿ��� len�� ���� ũ���)
//	size_t readsize{ 0 };	//�о���� ũ��
//	size_t sentData{ 0 };
//
//	int prev_per{ 0 };	//������Ʈ ���� percentage
//	int curr_per{ 0 };	//���� percentage
//
//	while (remainData)
//	{
//		if (remainData > BUFSIZE)
//			readsize = BUFSIZE;
//		else
//			readsize = remainData;
//
//		//���� ���� ����
//		retval = recv(client_sock, buf, readsize, 0);
//		if (retval == SOCKET_ERROR)
//		{
//			err_quit("recv()");
//			return false;
//		}
//		else if (retval == 0)
//			return false;
//
//		//std::copy(buffer, &buffer[�о� �� ����Ʈ ��], File_pos)�� ���ٽ����� ����.
//		File(buf, &buf[readsize]);
//
//		remainData -= readsize;
//		sentData += readsize;
//		curr_per = 100 * ((len * 1.f - remainData * 1.f) / len);
//		if (name == "Getting file name.....")
//			std::cout << name << std::endl;
//		else if (prev_per != curr_per)
//		{
//			prev_per = curr_per;
//			std::cout << "\r" << "[" << name << " : " << curr_per << "%]";
//		}
//	}
//
//	std::cout << "Complete." << std::endl;
//
//	return true;
//}