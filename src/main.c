#include <stm32f031x6.h>
#include "display.h"
#define MAZE_BLOCK_COUNT 5
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SPOTS_GOOD 5
#define MAX_SPOTS_BAD 5
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

typedef struct {
	uint16_t x;
	uint16_t y;
} Spot;



  

Spot goodBananas[MAX_SPOTS_GOOD];
Spot badBananas[MAX_SPOTS_GOOD];

void generateGoodRandomSpots() {
    for (int i = 0; i < MAX_SPOTS_GOOD; i++) {
        goodBananas[i].x = prbs() % 110; // Assuming screen width is 120
        goodBananas[i].y = prbs() % 130 + 20 ; // Assuming screen height is 160
    }
}
void generateBadRandomSpots() {
    for (int i = 0; i < MAX_SPOTS_BAD; i++) {
        badBananas[i].x = prbs() % 110; // Assuming screen width is 120
        badBananas[i].y = prbs() % 130 + 20; // Assuming screen height is 160
    }
}

volatile uint32_t milliseconds;

const uint16_t mazeX[]={60,60,60,60,60};
const uint16_t mazeY[]={20,36,52,68,84};

const uint16_t monkey1[]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,4098,30300,30300,30300,30300,30300,30300,30300,30300,4098,0,0,0,0,30300,30300,4098,30300,0,30300,30300,30300,30300,0,30300,4098,30300,30300,0,0,30300,30300,4098,30300,30300,30300,30300,30300,30300,30300,30300,4098,30300,30300,0,0,30300,30300,4098,30300,0,30300,30300,30300,30300,0,30300,4098,30300,30300,0,0,30300,30300,4098,4098,0,0,0,0,0,0,4098,4098,30300,30300,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,0,0,0,4098,30300,30300,4098,0,0,0,0,30300,0,0,4098,4098,4098,4098,4098,4098,30300,30300,4098,4098,4098,4098,4098,4098,0,0,30300,0,0,0,0,4098,30300,30300,4098,0,0,0,0,0,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,4098,4098,0,0,0,0,0,0,4098,4098,0,0,0,0,0,30300,30300,30300,0,0,0,0,0,0,4098,4098,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30300,30300,30300,0,0};
const uint16_t monkey2[]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,4098,30300,30300,30300,30300,30300,30300,30300,30300,4098,0,0,0,0,30300,30300,4098,30300,0,30300,30300,30300,30300,0,30300,4098,30300,30300,0,0,30300,30300,4098,30300,30300,30300,30300,30300,30300,30300,30300,4098,30300,30300,0,0,30300,30300,4098,30300,0,30300,30300,30300,30300,0,30300,4098,30300,30300,0,0,30300,30300,4098,4098,0,0,0,0,0,0,4098,4098,30300,30300,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,30300,0,0,0,0,4098,30300,30300,4098,0,0,0,0,0,0,0,4098,4098,4098,4098,4098,4098,30300,30300,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,0,4098,30300,30300,4098,0,0,0,0,30300,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,4098,4098,0,0,0,0,0,0,4098,4098,0,0,0,0,0,0,4098,4098,0,0,0,0,0,0,30300,30300,30300,0,0,0,0,30300,30300,30300,0,0,0,0,0,0,0,0,0,0,0};
const uint16_t monkeyUp[]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,0,0,0,0,0,0,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,30300,30300,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,30300,30300,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,0,0,0,0,0,4098,30300,30300,30300,30300,30300,30300,30300,30300,4098,0,0,0,0,0,0,4098,30300,0,30300,30300,30300,30300,0,30300,4098,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

const uint16_t deco1[]=
{
0,0,0,0,4,4,4,4,4,0,0,0,0,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,65415,65415,65415,248,65415,0,0,0,0,0,0,0,65415,65415,65415,65415,65415,8068,0,0,0,0,0,0,65415,65415,65415,4096,4096,0,0,0,0,0,0,0,0,65415,65415,65415,0,0,0,0,0,0,0,0,0,7936,7936,7936,0,0,0,0,0,0,0,0,7936,7936,65535,7936,0,0,0,0,0,0,0,0,7936,7936,65535,7936,7936,7936,7936,0,0,0,0,0,7936,7936,65535,65535,65535,65535,7936,0,0,0,0,0,7936,7936,7936,7936,7936,7936,7936,0,0,0,0,0,7936,7936,7936,7936,0,0,0,0,0,0,0,0,0,7936,65535,7936,0,0,0,0,0,0,0,0,0,7936,65535,7936,0,0,0,0,0,0,0,0,0,7936,65535,7936,0,0,0,0,0,0,0,0,0,7940,7940,7940,7940,0,0,0,
};
const uint16_t deco2[]= 
	{
0,0,0,0,0,4,4,4,4,4,0,0,0,0,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,65415,65415,65415,248,65415,0,0,0,0,0,0,0,65415,65415,65415,65415,65415,8068,0,0,0,0,0,0,65415,65415,65415,4096,4096,0,0,0,0,0,0,0,0,65415,65415,65415,0,0,0,0,0,0,0,0,7936,7936,7936,0,0,0,0,0,0,0,0,7936,7936,65535,7936,0,0,0,0,0,0,0,0,7936,7936,65535,7936,7936,7936,7936,0,0,0,0,0,7936,7936,65535,65535,65535,65535,7936,0,0,0,0,0,7936,7936,7936,7936,7936,7936,7936,0,0,0,0,0,7936,7936,7936,7936,0,0,0,0,0,0,0,0,0,40224,7936,65535,7936,0,0,0,0,0,0,0,40224,40224,7936,65535,7936,0,0,0,0,0,0,65315,40224,40224,7936,65535,40224,0,0,0,0,0,0,0,65315,0,65315,65315,65315,65315,0,0,
	};
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

const uint16_t badBanana[] ={
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,6342,6342,6342,6342,6342,6342,6342,6342,4098,0,0,0,0,0,0,6342,4228,4228,6342,6342,6342,6342,4228,4228,6342,0,0,0,0,30300,30300,6342,4228,4228,6342,6342,6342,6342,4228,4228,6342,30300,30300,0,0,30300,30300,4228,6342,6342,6342,4228,4228,6342,6342,6342,4228,30300,30300,0,0,30300,30300,4228,4228,6342,6342,4228,4228,6342,6342,4228,4228,30300,30300,0,0,30300,30300,4228,4228,65535,4228,65535,4228,65535,4228,4228,4228,30300,30300,0,0,0,0,4098,4228,4228,4228,4228,4228,4228,4228,4228,4098,0,0,0,0,0,0,0,0,4228,65535,4228,65535,4228,65535,0,0,0,0,0,30300,4098,4098,4098,4098,4098,4098,30300,30300,4098,4098,4098,4098,4098,4098,30300,0,0,0,0,0,0,4098,30300,30300,4098,0,0,0,0,0,0,0,30300,0,4098,4098,4098,4098,4098,4098,4098,4098,4098,4098,0,30300,0,0,30300,4098,4098,4098,0,0,
}; 

void drawGoodSpots() {
	for (int i = 0; i < MAX_SPOTS_GOOD; i++) {
		putImage(goodBananas[i].x, goodBananas[i].y, 12,16, goodBanana, 0, 0); // Assuming bananaImage is defined
	}
};
void drawBadSpots() {
	for (int i = 0; i < MAX_SPOTS_GOOD; i++) {
		putImage(badBananas[i].x, badBananas[i].y, 16,16, badBanana, 0, 0); // Assuming bananaImage is defined
	}
};



int main()
{	
	int hinverted = 0;
	int vinverted = 0;
	int toggle = 0;
	int hmoved = 0;
	int vmoved = 0;
	int buttonPressed = 0;
	uint16_t x = 50;
	uint16_t y = 50;
	uint16_t oldx = x;
	uint16_t oldy = y;
	initClock();
	initSysTick();
	setupIO();
	int banana = 0;
	randomize();
	generateGoodRandomSpots();
	randomize();
	generateBadRandomSpots();
	drawBadSpots();
	drawGoodSpots();
	int hearts = 3;
	
	while(1)
	{	

		if((GPIOB -> IDR & (1 << 3))==0){
			if(!buttonPressed){
				GPIOA->ODR |= (1 << 1);
				hearts == 0 ? 0 : hearts--;
				buttonPressed = 1;
			}
		} else{
			GPIOA->ODR &= ~(1 << 1);
			buttonPressed = 0;
		};

		

		for (int i = 0; i < hearts; i++)
		{
			drawRectangle(80,0,60,16,0);
			putImage(80+16*i,0,16,16,heart,0,0);
		};

		

		printNumberX2(hearts, 0,0, RGBToWord(0xff,0xff,0), 0);
		
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
		delay(50);
		
	}
	return 0;
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
	pinMode(GPIOA,1,1);
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