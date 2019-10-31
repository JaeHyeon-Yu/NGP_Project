#include <Windows.h>
#include <conio.h>
#include "main.h"
#include "fmod/inc/fmod.hpp"
#pragma comment(lib, "fmod/fmodex_vc.lib")

using namespace FMOD;

enum SOUND_KIND
{
	SOUND_1 = 0, SOUND_END =1
};

System *system_s;
Sound * sound[SOUND_END];
Channel *channel;

void Sound_SetUp()
{
	System_Create(&system_s);

	system_s->init(1, FMOD_INIT_NORMAL, 0);
	system_s->createSound("Sound/Tetris_Theme.mp3", FMOD_LOOP_NORMAL, 0, &sound[SOUND_1]);
}
void Play_Sound()
{
	channel->setVolume(0.1);
	system_s->playSound(FMOD_CHANNEL_FREE, sound[SOUND_1], 0, &channel);
}