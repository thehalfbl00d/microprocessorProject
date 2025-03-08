#include <stm32f031x6.h>
#include "display.h"
#define MAZE_BLOCK_COUNT 5
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "sound.h"
#include "musical_notes.h"

#define MAX_SPOTS_GOOD 10
#define MAX_SPOTS_BAD 4
uint32_t shift_register=0;
void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
int hitMaze(uint16_t playerx,uint16_t playery,uint16_t w, uint16_t h);
uint16_t readADC(void);
void randomize(void);
uint32_t prbs();
void generateGoodRandomSpots();
void generateBadRandomSpots();
void drawGoodSpots();
void drawBadSpots();
void removeGoodBanana();
void removebadBanana();
void drawHearts(int Hearts);
void harryPotterTheme();
void lostSound();
int collisions(int x, int y, int hearts);
void guide();
void menu();

int points = 0;
int *pointsPtr = &points;
int topScore = 0;
typedef struct {
	uint16_t x;
	uint16_t y;
} Spot;

Spot goodBananas[MAX_SPOTS_GOOD];
Spot badBananas[MAX_SPOTS_GOOD];


volatile uint32_t milliseconds;

const uint16_t mazeX[]={60,60,60,60,60};
const uint16_t mazeY[]={20,36,52,68,84};

const uint16_t monkey1[]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,4098,30300,30300,30300,30300,30300,30300,30300,30300,4098,0,0,0,0,30300,30300,4098,30300,0,30300,30300,30300,30300,0,30300,4098,30300,30300,0,0,30300,30300,4098,30300,30300,30300,30300,30300,30300,30300,30300,4098,30300,30300,0,0,30300,30300,4098,30300,0,30300,30300,30300,30300,0,30300,4098,30300,30300,0,0,30300,30300,4098,4098,0,0,0,0,0,0,4098,4098,30300,30300,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,0,0,0,4098,30300,30300,4098,0,0,0,0,30300,0,0,4098,4098,4098,4098,4098,4098,30300,30300,4098,4098,4098,4098,4098,4098,0,0,30300,0,0,0,0,4098,30300,30300,4098,0,0,0,0,0,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,4098,4098,0,0,0,0,0,0,4098,4098,0,0,0,0,0,30300,30300,30300,0,0,0,0,0,0,4098,4098,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30300,30300,30300,0,0};
const uint16_t monkey2[]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,4098,30300,30300,30300,30300,30300,30300,30300,30300,4098,0,0,0,0,30300,30300,4098,30300,0,30300,30300,30300,30300,0,30300,4098,30300,30300,0,0,30300,30300,4098,30300,30300,30300,30300,30300,30300,30300,30300,4098,30300,30300,0,0,30300,30300,4098,30300,0,30300,30300,30300,30300,0,30300,4098,30300,30300,0,0,30300,30300,4098,4098,0,0,0,0,0,0,4098,4098,30300,30300,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,30300,0,0,0,0,4098,30300,30300,4098,0,0,0,0,0,0,0,4098,4098,4098,4098,4098,4098,30300,30300,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,0,4098,30300,30300,4098,0,0,0,0,30300,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,4098,4098,0,0,0,0,0,0,4098,4098,0,0,0,0,0,0,4098,4098,0,0,0,0,0,0,30300,30300,30300,0,0,0,0,30300,30300,30300,0,0,0,0,0,0,0,0,0,0,0};
const uint16_t monkeyUp[]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,0,0,0,0,0,0,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,30300,30300,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,30300,30300,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,4098,30300,30300,30300,30300,30300,30300,30300,30300,4098,0,0,0,0,0,0,4098,30300,0,30300,30300,30300,30300,0,30300,4098,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

const uint16_t deco1[]= {0,0,0,0,4,4,4,4,4,0,0,0,0,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,65415,65415,65415,248,65415,0,0,0,0,0,0,0,65415,65415,65415,65415,65415,8068,0,0,0,0,0,0,65415,65415,65415,4096,4096,0,0,0,0,0,0,0,0,65415,65415,65415,0,0,0,0,0,0,0,0,0,7936,7936,7936,0,0,0,0,0,0,0,0,7936,7936,65535,7936,0,0,0,0,0,0,0,0,7936,7936,65535,7936,7936,7936,7936,0,0,0,0,0,7936,7936,65535,65535,65535,65535,7936,0,0,0,0,0,7936,7936,7936,7936,7936,7936,7936,0,0,0,0,0,7936,7936,7936,7936,0,0,0,0,0,0,0,0,0,7936,65535,7936,0,0,0,0,0,0,0,0,0,7936,65535,7936,0,0,0,0,0,0,0,0,0,7936,65535,7936,0,0,0,0,0,0,0,0,0,7940,7940,7940,7940,0,0,0,};
const uint16_t deco2[]= {0,0,0,0,0,4,4,4,4,4,0,0,0,0,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,65415,65415,65415,248,65415,0,0,0,0,0,0,0,65415,65415,65415,65415,65415,8068,0,0,0,0,0,0,65415,65415,65415,4096,4096,0,0,0,0,0,0,0,0,65415,65415,65415,0,0,0,0,0,0,0,0,7936,7936,7936,0,0,0,0,0,0,0,0,7936,7936,65535,7936,0,0,0,0,0,0,0,0,7936,7936,65535,7936,7936,7936,7936,0,0,0,0,0,7936,7936,65535,65535,65535,65535,7936,0,0,0,0,0,7936,7936,7936,7936,7936,7936,7936,0,0,0,0,0,7936,7936,7936,7936,0,0,0,0,0,0,0,0,0,40224,7936,65535,7936,0,0,0,0,0,0,0,40224,40224,7936,65535,7936,0,0,0,0,0,0,65315,40224,40224,7936,65535,40224,0,0,0,0,0,0,0,65315,0,65315,65315,65315,65315,0,0,};
const uint16_t deco3[]= 
{
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,0,0,0,0,0,0,0,0,0,0,4,4,0,0,0,0,0,0,0,0,0,4,4,4,4,0,0,0,0,0,0,0,4,4,4,4,4,4,0,0,0,0,7936,7936,4,4,4,4,4,4,7936,7936,0,0,65535,65535,4,4,4,4,4,4,65535,65535,0,0,7936,7936,4,4,4,4,4,4,7936,7936,0,0,0,0,0,4,4,4,4,0,0,0,0,0,0,0,0,0,24327,24327,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
const uint16_t goodBanana[]=
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,4098,0,0,0,0,0,0,0,0,0,0,4098,4098,0,0,0,0,0,0,0,0,0,0,0,65287,48141,0,0,0,0,0,0,0,0,0,65287,65287,48141,0,0,0,0,0,0,0,0,0,65287,65287,65287,0,0,0,0,0,0,0,0,65287,65287,65287,48141,0,0,4098,4098,0,0,0,0,65287,65287,65287,48141,0,0,4098,4098,0,0,65287,65287,65287,65287,48141,48141,0,0,65287,65287,65287,65287,65287,65287,65287,65287,48141,48141,0,0,0,48141,48141,65287,65287,65287,65287,48141,0,0,0,0,0,0,0,48141,48141,48141,48141,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const uint16_t deadMonkey[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,6342,6342,6342,6342,6342,6342,6342,6342,4098,0,0,0,0,0,0,6342,4228,4228,6342,6342,6342,6342,4228,4228,6342,0,0,0,0,30300,30300,6342,4228,4228,6342,6342,6342,6342,4228,4228,6342,30300,30300,0,0,30300,30300,4228,6342,6342,6342,4228,4228,6342,6342,6342,4228,30300,30300,0,0,30300,30300,4228,4228,6342,6342,4228,4228,6342,6342,4228,4228,30300,30300,0,0,30300,30300,4228,4228,65535,4228,65535,4228,65535,4228,4228,4228,30300,30300,0,0,0,0,4098,4228,4228,4228,4228,4228,4228,4228,4228,4098,0,0,0,0,0,0,0,0,4228,65535,4228,65535,4228,65535,0,0,0,0,0,30300,4098,4098,4098,4098,4098,4098,30300,30300,4098,4098,4098,4098,4098,4098,30300,0,0,0,0,0,0,4098,30300,30300,4098,0,0,0,0,0,0,0,30300,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,30300,0,0,30300,4098,4098,4098,0,0,
};

const uint16_t heart[]={
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7936,7936,7936,7936,0,0,0,0,7936,7936,7936,7936,0,0,0,7936,7936,7936,65535,7936,7936,0,0,7936,7936,7936,65535,7936,7936,0,0,7936,7936,7936,7936,65535,7936,7936,7936,7936,7936,7936,7936,65535,7936,0,46120,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,46120,46120,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,46120,46120,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,46120,46120,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,46120,46120,46120,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,46120,46120,0,46120,46120,7936,7936,7936,7936,7936,7936,7936,7936,7936,7936,46120,46120,0,0,0,46120,46120,7936,7936,7936,7936,7936,7936,7936,7936,46120,46120,0,0,0,0,0,46120,46120,7936,7936,7936,7936,7936,7936,46120,46120,0,0,0,0,0,0,0,46120,46120,7936,7936,7936,7936,46120,46120,0,0,0,0,0,0,0,0,0,46120,46120,7936,7936,46120,46120,0,0,0,0,0,0,0,0,0,0,0,46120,46120,46120,46120,0,0,0,0,0,0,0,0,0,0,0,0,0,46120,46120,0,0,0,0,0,0,0,
};

const uint16_t badBanana[]=
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,4098,0,0,0,0,0,0,0,0,0,0,4098,4098,0,0,0,0,0,0,0,0,0,0,0,65287,48141,0,0,0,0,0,0,0,0,0,65287,65287,48141,0,0,0,0,0,0,0,0,0,65287,65287,65287,0,0,0,0,0,0,0,0,65287,65287,65287,48141,0,0,4098,4098,0,0,0,0,65287,65287,65287,48141,0,0,4098,4098,0,0,65287,65287,65287,65287,48141,48141,0,0,65287,65287,65287,65287,65287,65287,65287,65287,48141,48141,0,0,0,48141,48141,65287,65287,65287,65287,48141,0,0,0,0,0,0,0,48141,48141,48141,48141,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
const uint16_t startup[]={
	24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,35602,29212,54044,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,20243,3603,52234,63525,29212,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,35602,30757,53523,59401,30245,30757,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,22053,55077,30757,55333,47141,37901,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,35602,24592,9233,47887,63502,47887,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,6158,6158,37901,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,37901,63502,47887,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,44299,63502,63502,55054,23055,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,47887,63502,13069,14863,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,63502,30991,44299,47887,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,63502,29966,37901,14863,20748,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,6158,37901,6158,6158,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,6158,37901,37901,47887,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,6158,37901,37901,47887,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,6158,37901,23055,55310,37901,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,6158,37901,23311,23311,6158,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,6158,37901,39439,14863,37901,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,6158,37901,23055,47887,37901,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,6158,63759,37901,47887,23055,29966,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,63502,29966,29966,47887,23055,29966,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,44299,63502,37901,6158,23055,39439,29966,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,37901,37901,23055,47887,6671,55310,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,47887,24592,63759,55823,29966,23055,29966,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,37901,24592,14863,55054,23055,47887,44299,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,63502,44299,24592,47887,63502,20748,47887,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,37901,24592,37901,47887,47887,37901,47887,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,63502,44299,24592,47887,6671,63759,47887,37901,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,37901,24592,37901,63759,47887,31503,39695,44299,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,47887,24592,44299,63502,55310,6671,47887,47887,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,63502,44299,20748,63502,29966,31503,55310,47887,55823,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,44299,63502,44299,63502,30991,44299,31503,23311,47887,39695,44299,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,35602,35602,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,63502,44299,23055,47887,29966,39439,47887,47887,39439,23311,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,35602,46108,11539,63525,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,63502,37901,37901,14863,47887,23055,39695,47887,37901,20748,31503,37901,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,30757,54556,41472,30237,24592,24592,24592,24592,44299,44299,37901,37901,37901,37901,37901,47887,37901,44299,6158,47887,39695,39695,31503,39695,55054,24592,47887,30991,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,37901,47887,47887,47887,30991,30991,30991,29966,29966,29966,6158,63759,37901,30991,47887,23311,39695,47887,39439,63759,29966,24592,6671,47887,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,63502,30991,23055,63502,30991,63502,23055,39439,39439,14863,55310,23055,6671,47887,47887,47887,23311,44299,63502,24592,44299,47887,39439,44299,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,63759,47887,63502,37901,6158,39439,23055,6671,23055,39439,64015,47887,6158,24592,37901,63502,44299,6158,47887,6671,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,37901,44299,37901,37901,63502,47887,47887,47887,14863,47887,29966,6158,23055,6158,64015,47887,6671,37901,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,37901,47887,63502,37901,37901,37901,37901,37901,55310,63502,23055,6671,47887,47887,55823,37901,44299,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,44299,37901,37901,37901,37901,37901,6158,44299,24592,44299,44299,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,52851,15326,52851,24592,24592,24592,24592,52851,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,57079,24592,24592,24592,24592,24592,24592,24592,24592,43602,43602,24592,24592,24592,24592,46773,46773,24592,36996,36996,46773,46773,24592,24592,24592,24592,24592,24592,24592,9513,22726,40431,15326,7399,24592,24592,43602,23783,48359,7399,24592,24592,24592,24592,24592,38053,20876,24592,24592,46773,46773,24592,24592,7399,15591,36996,24592,24592,24592,38053,57079,57079,24592,24592,24592,46773,46773,24592,29861,56551,46773,46773,46773,24592,24592,24592,24592,24592,24592,62108,56815,24592,52851,46773,46773,24592,6077,15855,24592,39110,46773,24592,24592,24592,24592,15855,56551,57079,46773,46773,46773,24592,46773,39110,15591,56551,52851,24592,24592,15326,15326,24592,57079,24592,24592,46773,46773,24592,16119,64478,24592,24592,15326,46773,24592,24592,24592,24592,24592,24592,57079,52851,7399,57079,24592,24592,15326,39902,20876,52851,57079,46773,24592,24592,46773,40431,52851,24592,46773,15326,15326,24592,15326,15326,24592,48359,48359,24592,24592,39110,40431,24592,24592,57079,24592,57079,24592,46773,40431,52851,52851,24592,52851,7399,24592,24592,24592,24592,24592,24592,40431,39110,24592,48359,52851,24592,16119,15855,15855,7399,57079,46773,24592,24592,46773,40431,52851,24592,52851,40431,46773,24592,15326,15326,24592,7399,48359,52851,24592,29596,24311,24592,24592,46773,39110,7399,24592,64478,16119,7399,7399,57079,7399,46773,46773,24592,24592,24592,24592,24592,48359,29596,52851,57079,24592,24592,7399,52851,24592,24592,46773,46773,24592,24592,52851,57079,24592,24592,24592,7399,52851,24592,24592,57079,46773,7399,52851,7399,24592,52851,7399,24592,24592,24592,57079,39110,24592,40431,39110,52851,24592,24592,52851,52851,7399,24592,24592,24592,24592,24592,7399,7399,7399,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,24592,7399,52851,24592,24592,24592,24592,24592,24592,24592,46773,52851,24592,24592,24592,24592,24592,24592,24592,24592,46773,40431,24592,40431,39110,24592,24592,24592,24592,24592,57079,24592,24592,24592,
};

void populate(){
	fillRectangle(0,0,128,160,0);
	randomize();
	generateGoodRandomSpots();
	randomize();
	generateBadRandomSpots();
	drawBadSpots();
	drawGoodSpots();
	
}

int main()
{	
	
	//initialize cocks
	initClock();
	initSysTick();
	setupIO();
	

	
	//startup screen
	fillRectangle(0,0,128,160,0);
	printText("Press Start Key!", 0, 0, RGBToWord(0xff,0xff,0),0);
	putImage(10,10,50,50,startup,0,0);
	initSound();
	harryPotterTheme();
	while((GPIOB -> IDR & (1 << 3)) != 0){
		int isplaying = 0;
	};
	
	
	fillRectangle(0,0,128,160,0);

	
	menu();
	
	
	return 0;
}

void menu(){
	fillRectangle(0,0,128,160,0);
	do {
		printTextX2("Menu", 40, 0, RGBToWord(0xff,0xff,0),0);
		drawRectangle(10, 20, 108, 130, RGBToWord(0xff,0xff,0));
		printText("^. Play", 20, 40, RGBToWord(0xff,0xff,0),0);
		printText("v. Guide", 20, 60, RGBToWord(0xff,0xff,0),0);
		printText(">. Credits", 20, 80, RGBToWord(0xff,0xff,0),0);

		if((GPIOA -> IDR & (1 << 8)) == 0){
			game();
			break;
		}
		if((GPIOA -> IDR & (1 << 11)) == 0){
			guide();
			while((GPIOB -> IDR & (1 << 3)) == 0);
			
			menu();
				
			break;
		}
	} while(1);
}

void game(){
	//yeye variables
	populate();
	int hinverted = 0;
	int vinverted = 0;
	int toggle = 0;
	int hmoved = 0;
	int vmoved = 0;
	int buttonPressed = 0;
	int ingame = 1;
	int hearts = 4;
	int isplaying = 1;
	uint16_t x = 0;
	uint16_t y = 20;
	uint16_t oldx = x;
	uint16_t oldy = y;

	while(isplaying)
	{	
		drawHearts(hearts);
		while(ingame)
		{	

			if (GPIOA ->IDR & (1 << 0)){
				menu();
				break;
			}
			printText("ingame", 0,150, RGBToWord(0xff,0xff,0),0);
			hearts = collisions(x,y,hearts);

			delay(50);

			//prints points in numbers
			printNumberX2(*pointsPtr, 0, 0, RGBToWord(0xff,0xff,0),0);

			//movement
			hmoved = vmoved = 0;
			hinverted = vinverted = 0;
			if ((GPIOB->IDR & (1 << 4))==0) // right pressed
			{				
				GPIOA->ODR |= (1 << 1);			
				if (x < 110)
				{
					x = x + 4;
					hmoved = 1;
					hinverted=0;
				}						
			}
			if ((GPIOB->IDR & (1 << 5))==0) // left pressed
			{	
				GPIOA->ODR |= (1 << 1);		
				
				if (x > 4)
				{
					x = x - 4;
					hmoved = 1;
					hinverted=1;
				}			
			}
			if ( (GPIOA->IDR & (1 << 11)) == 0) // down pressed
			{
				GPIOA->ODR |= (1 << 1);		
				if (y < 140)
				{
					y = y + 4;			
					vmoved = 1;
					vinverted = 0;
				}
			}
			if ( (GPIOA->IDR & (1 << 8)) == 0) // up pressed
			{		
				GPIOA->ODR |= (1 << 1);			
				if (y > 20)
				{
					y = y - 4;
					vmoved = 1;
					vinverted = 1;
				}
			}
			
			if ((vmoved) || (hmoved))
			{
				// only redraw if there has been some movement (reduces flicker)
				fillRectangle(oldx,oldy,18,16,0);
				oldx = x;
				oldy = y;					
				if (hmoved)
				{	
					
					if (toggle)
						putImage(x,y,16,16,monkey1,hinverted,0);
					else
						putImage(x,y,16,16,monkey2,hinverted,0);
					
					toggle = toggle ^ 1;
				}
				else
				{
					putImage(x,y,16,16,monkeyUp,0,vinverted);
				}
			}


				if(hearts == 0){
					ingame = 0;
				}
		}
		//game over screen
		if (hearts == 0){
			fillRectangle(0,0,128,160,0);
			putImage(10,10,16,16,deadMonkey,0,0);
			printTextX2("GAME OVER", 0,40, RGBToWord(0xff,0xff,0),0);
			printText("Restart Button", 0,60, RGBToWord(0xff,0xff,0),0);
			if (*pointsPtr > topScore){
				topScore = *pointsPtr;
			}
			printText("Score: ", 0, 100, RGBToWord(0xff,0xff,0),0);
			printNumberX2(*pointsPtr, 0, 110, RGBToWord(0xff,0xff,0),0);

			printText("Top Score: ", 0, 130, RGBToWord(0xff,0xff,0),0);
			printNumberX2(topScore, 0, 140, RGBToWord(0xff,0xff,0),0);
			lostSound();
			//wait for restart
			while ((GPIOB -> IDR & (1 << 3)) != 0);
			//restart game
			fillRectangle(0,0,128,160,0);
			putImage(x,y,16,16,monkey1,0,0);

			removebadBanana();
			removeGoodBanana();

			randomize();
			generateGoodRandomSpots();
			randomize();
			generateBadRandomSpots();
			drawBadSpots();
			drawGoodSpots();
			*pointsPtr = 0;
			ingame = 1;
			hearts = 3;
		};
		delay(50);
		
	}
}


void initSysTick(void)
{
	SysTick->LOAD = 48000;
	SysTick->CTRL = 7;
	SysTick->VAL = 10;
	__asm(" cpsie i "); // enable interrupts
}
void SysTick_Handler(void)
{
	milliseconds++;
}
void initClock(void)
{
// This is potentially a dangerous function as it could
// result in a system with an invalid clock signal - result: a stuck system
        // Set the PLL up
        // First ensure PLL is disabled
        RCC->CR &= ~(1u<<24);
        while( (RCC->CR & (1 <<25))); // wait for PLL ready to be cleared
        
// Warning here: if system clock is greater than 24MHz then wait-state(s) need to be
// inserted into Flash memory interface
				
        FLASH->ACR |= (1 << 0);
        FLASH->ACR &=~((1u << 2) | (1u<<1));
        // Turn on FLASH prefetch buffer
        FLASH->ACR |= (1 << 4);
        // set PLL multiplier to 12 (yielding 48MHz)
        RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
        RCC->CFGR |= ((1<<21) | (1<<19) ); 

        // Need to limit ADC clock to below 14MHz so will change ADC prescaler to 4
        RCC->CFGR |= (1<<14);

        // and turn the PLL back on again
        RCC->CR |= (1<<24);        
        // set PLL as system clock source 
        RCC->CFGR |= (1<<1);
}
void delay(volatile uint32_t dly)
{
	uint32_t end_time = dly + milliseconds;
	while(milliseconds != end_time)
		__asm(" wfi "); // sleep
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	/*
	*/
	uint32_t mode_value = Port->MODER;
	Mode = Mode << (2 * BitNumber);
	mode_value = mode_value & ~(3u << (BitNumber * 2));
	mode_value = mode_value | Mode;
	Port->MODER = mode_value;
}
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py)
{
	// checks to see if point px,py is within the rectange defined by x,y,w,h
	uint16_t x2,y2;
	x2 = x1+w;
	y2 = y1+h;
	int rvalue = 0;
	if ( (px >= x1) && (px <= x2))
	{
		// ok, x constraint met
		if ( (py >= y1) && (py <= y2))
			rvalue = 1;
	}
	return rvalue;
}

void setupIO()
{
	RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
	display_begin();
	pinMode(GPIOB,4,0);
	pinMode(GPIOB,5,0);
	pinMode(GPIOA,8,0);
	pinMode(GPIOA,11,0);
	pinMode(GPIOB,3,0);
	enablePullUp(GPIOB,4);
	enablePullUp(GPIOB,5);
	enablePullUp(GPIOA,11);
	enablePullUp(GPIOA,8);
	enablePullUp(GPIOB,3);
}

uint16_t readADC(void)
{
    // Configure ADC
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // Enable ADC clock
    ADC1->CR |= ADC_CR_ADEN;            // Enable ADC
    while (!(ADC1->ISR & ADC_ISR_ADRDY))
        ; // Wait for ADC to be ready

    // Start ADC conversion
    ADC1->CR |= ADC_CR_ADSTART;
    while (!(ADC1->ISR & ADC_ISR_EOC))
        ; // Wait for conversion to complete

    // Read ADC value
    uint16_t adc_value = ADC1->DR;

    // Disable ADC
    ADC1->CR |= ADC_CR_ADDIS;

    return adc_value;
}
void randomize(void)
{
    // uses ADC noise values to seed the shift_register
    while(shift_register==0)
    {
        for (int i=0;i<10;i++)
        {
            shift_register+=(readADC()<<i);
        }
    }
}

uint32_t prbs()
{
	// This is an unverified 31 bit PRBS generator
	// It should be maximum length but this has not been verified 
	unsigned long new_bit=0;	

    new_bit= ((shift_register & (1<<27))>>27) ^ ((shift_register & (1<<30))>>30);
    new_bit= ~new_bit;
    new_bit = new_bit & 1;
    shift_register=shift_register << 1;
    shift_register=shift_register | (new_bit);
		
	return shift_register & 0x7fffffff; // return 31 LSB's 
}

int collisions(int x, int y, int hearts){
	initSound();
	for (int i = 0; i < MAX_SPOTS_GOOD; i++)
	{
		if(	isInside(goodBananas[i].x,goodBananas[i].y,12,16,x,y) || 
			isInside(goodBananas[i].x,goodBananas[i].y,12,16,x+16,y) || 
			isInside(goodBananas[i].x,goodBananas[i].y,12,16,x,y+16) || 
			isInside(goodBananas[i].x,goodBananas[i].y,12,16,x+16,y+16))
		{
			fillRectangle(goodBananas[i].x,goodBananas[i].y,16,16,0);
			goodBananas[i].x = 0xFFFF; // Mark as removed
			goodBananas[i].y = 0xFFFF;
			(*pointsPtr)++;
			playNote(G4);
			if (*pointsPtr % 3 == 0){
				removebadBanana();
				removeGoodBanana();
				printNumberX2(*pointsPtr, 0, 0, RGBToWord(0xff,0xff,0),0);
				randomize();
				generateGoodRandomSpots();
				randomize();
				generateBadRandomSpots();
				drawBadSpots();
				drawGoodSpots();
			};
			printNumberX2(*pointsPtr, 0, 0, RGBToWord(0xff,0xff,0),0);
		}
		if(	isInside(badBananas[i].x,badBananas[i].y,12,16,x,y) ||
			isInside(badBananas[i].x,badBananas[i].y,12,16,x+16,y) ||
			isInside(badBananas[i].x,badBananas[i].y,12,16,x,y+16) ||
			isInside(badBananas[i].x,badBananas[i].y,12,16,x+16,y+16))
		{
			fillRectangle(badBananas[i].x,badBananas[i].y,16,16,0);

			playNote(E4);
			badBananas[i].x = 0xFFFF; // Mark as removed
			badBananas[i].y = 0xFFFF;
			if (hearts > 0) hearts--;
			drawHearts(hearts);
			
		};
	};
	return hearts;
}

int checkCollisionWithBananas(uint16_t px, uint16_t py, Spot *bananas, int count, int isBad) {
	for (int i = 0; i < count; i++) {
		if (bananas[i].x != 0xFFFF && // Ensure it's not already removed
			abs(px - bananas[i].x) < 12 && 
			abs(py - bananas[i].y) < 16) {
			
			fillRectangle(bananas[i].x, bananas[i].y, 16, 16, 0); // Erase banana
			bananas[i].x = 0xFFFF; // Mark as removed
			bananas[i].y = 0xFFFF; // Mark as removed
			
			if (isBad) {
				return 1; // Return 1 if a bad banana was eaten
			}
			return 0; // Return 0 if it was a good banana
		}
	}
	return 0; // No collision
}

void generateGoodRandomSpots() {
    for (int i = 0; i < MAX_SPOTS_GOOD; i++) {
        uint16_t newX, newY;
        int isOverlapping;
        do {
            newX = prbs() % 110; // Assuming screen width is 120
            newY = prbs() % 130 + 20; // Assuming screen height is 160
            isOverlapping = 0;
            for (int j = 0; j < i; j++) {
                if (abs(newX - goodBananas[j].x) < 12 && abs(newY - goodBananas[j].y) < 16) {
                    isOverlapping = 1; // Check overlap with previous good spots
                    break;
                }
            }
        } while (isOverlapping); // Keep generating until no overlap

        goodBananas[i].x = newX;
        goodBananas[i].y = newY;
    }
}

void removeGoodBanana() {
	for (int i = 0; i < MAX_SPOTS_GOOD; i++) {
		fillRectangle(goodBananas[i].x, goodBananas[i].y, 16, 16, 0); // Erase banana
	}
}
void removebadBanana() {
	for (int i = 0; i < MAX_SPOTS_BAD; i++) {
		fillRectangle(badBananas[i].x, badBananas[i].y, 16, 16, 0); // Erase banana
	}
}
void generateBadRandomSpots() {
    for (int i = 0; i < MAX_SPOTS_BAD; i++) {
        uint16_t newX, newY;
        int isOverlapping;
        do {
            newX = prbs() % 110; // Assuming screen width is 120
            newY = prbs() % 130 + 20; // Assuming screen height is 160
            isOverlapping = 0;

            // Check overlap with previously placed badBananas
            for (int j = 0; j < i; j++) {
                if (abs(newX - badBananas[j].x) < 12 && abs(newY - badBananas[j].y) < 16) {
                    isOverlapping = 1;
                    break;
                }
            }

            // Check overlap with goodBananas
            for (int j = 0; j < MAX_SPOTS_GOOD; j++) {
                if (abs(newX - goodBananas[j].x) < 12 && abs(newY - goodBananas[j].y) < 16) {
                    isOverlapping = 1;
                    break;
                }
            }
        } while (isOverlapping); // Keep generating until no overlap

        badBananas[i].x = newX;
        badBananas[i].y = newY;
    }
}

void drawGoodSpots() {
	for (int i = 0; i < MAX_SPOTS_GOOD; i++) {
		putImage(goodBananas[i].x, goodBananas[i].y, 12,16, goodBanana, 0, 0); // Assuming bananaImage is defined
	}
};
void drawBadSpots() {
	for (int i = 0; i < MAX_SPOTS_GOOD; i++) {
		putImage(badBananas[i].x, badBananas[i].y, 12,16, badBanana, 0, 0); // Assuming bananaImage is defined
	}
};

void drawHearts(int hearts) {
	fillRectangle(60,0,80,16,0);
	for (int i = 0; i < hearts; i++)
		{
			putImage(60+16*i,0,16,16,heart,0,0);
		};
}

void harryPotterTheme(){	
	initSound();
for (int i = 0; i < 1; i++)
{
    playNote(E5);
    delay(150);
    playNote(B4);
    delay(150);
    playNote(C5);
    delay(150);
    playNote(D5);
    delay(300);
    playNote(C5);
    delay(150);
    playNote(B4);
    delay(150);
    playNote(A4);
    delay(300);
    playNote(A4);
    delay(150);
    playNote(C5);
    delay(150);
    playNote(E5);
    delay(300);
    playNote(D5);
    delay(150);
    playNote(C5);
    delay(150);
    playNote(B4);
    delay(300);
    playNote(B4);
    delay(150);
    playNote(C5);
    delay(150);
    playNote(D5);
    delay(300);
    playNote(E5);
    delay(300);
    playNote(C5);
    delay(300);
    playNote(A4);
    delay(300);
    playNote(A4);
    delay(300);
    
    playNote(D5);
    delay(300);
    playNote(F5);
    delay(150);
    playNote(A5);
    delay(300);
    playNote(G5);
    delay(150);
    playNote(F5);
    delay(150);
    playNote(E5);
    delay(300);
    playNote(C5);
    delay(150);
    playNote(E5);
    delay(300);
    playNote(D5);
    delay(150);
    playNote(C5);
    delay(150);
    playNote(B4);
    delay(300);
    playNote(B4);
    delay(150);
    playNote(C5);
    delay(150);
    playNote(D5);
    delay(300);
    playNote(E5);
    delay(300);
    playNote(C5);
    delay(300);
    playNote(A4);
    delay(300);
    playNote(A4);
    delay(300);
	playNote(0);
}
};

void lostSound(){
	initSound();
	for (int i = 0; i < 1; i++)
	{
		playNote(E4);
		delay(300);
		playNote(C4);
		delay(300);
		playNote(G3);
		delay(600);
		playNote(0);
	}
}

void guide(){
	fillRectangle(0,0,128,160,0);
	printText("Guide", 0, 0, RGBToWord(0xff,0xff,0),0);
	printText("Use the arrow keys ", 0, 20, RGBToWord(0xff,0xff,0),0);
	printText("to move the monkey ", 0, 40, RGBToWord(0xff,0xff,0),0);
	printText("Collect the good ", 0, 60, RGBToWord(0xff,0xff,0),0);
	printText("bananas to gain ", 0, 80, RGBToWord(0xff,0xff,0),0);
	printText("points Avoid the bad bananas to not lose points You have 3 lives Press the restart button to restart the game", 0, 100, RGBToWord(0xff,0xff,0),0);
	printText("Press the start button to go back to the menu", 0, 120, RGBToWord(0xff,0xff,0),0);
}