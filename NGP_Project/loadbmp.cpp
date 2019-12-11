#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "main.h"
#include <iostream>

GLubyte * LoadDIBitmap(const char *file_name, BITMAPINFO **info)
{
	FILE *fp;
	GLubyte *bits;
	int bitsize, infosize;
	BITMAPFILEHEADER header;

	// 바이너리 읽기 모드
	if ((fp = fopen(file_name, "rb")) == NULL)
		return NULL;

	// 비트맵파일 헤더 읽기
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1)
	{
		fclose(fp);
		return NULL;
	}

	// BMP 파일인지 확인
	if (header.bfType != 'MB')
	{
		fclose(fp);
		return NULL;
	}

	// BITMAPINFOHEADER 위치로
	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);

	// 비트맵 이미지 데이터를 넣을 메모리할당
	if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL)
	{
		fclose(fp);
		exit(0);
		return NULL;
	}

	// 비트맵 인포헤더를 읽는다.
	if (fread(*info, 1, infosize, fp) < (unsigned int)infosize)
	{
		free(*info);
		fclose(fp);
		return NULL;
	}

	// 비트맵의 크기 설정
	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth*(*info)->bmiHeader.biBitCount + 7) / 8.0 *
		abs((*info)->bmiHeader.biHeight);

	// 비트맵의 크기만큼 메모리 할당
	if ((bits = (unsigned char *)malloc(bitsize)) == NULL)
	{
		free(*info);
		fclose(fp);
		return NULL;
	}

	// 비트맵 데이터를 bit에 저장 (GLubyte 타입)
	if (fread(bits, 1, bitsize, fp) < (unsigned int)bitsize)
	{
		free(*info);
		free(bits);
		fclose(fp);
		return NULL;
	}
	fclose(fp);
	return bits;
}
void Init_Texture(GLubyte *pBytes, BITMAPINFO *info, GLuint *textures)
{
	glGenTextures(1, textures);

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	pBytes = LoadDIBitmap("Image/background2.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 504, 300, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	// 일반 배경
	
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	pBytes = LoadDIBitmap("Image/title.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 800, 800, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	// 타이틀
}
void Init_EndTexture(GLubyte* pBytes, BITMAPINFO* info, GLuint* textures)
{
	glGenTextures(1, textures);

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	pBytes = LoadDIBitmap("Image/win.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 1600, 800, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	// Win

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	pBytes = LoadDIBitmap("Image/loose.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 1600, 800, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	// Loose
}