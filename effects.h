/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef _EFFECTS_H_
	#define _EFFECTS_H_ 1
	

typedef union rgb_t { 
	
	uint32_t rgb;
	
	struct rgb_tag { 
		uint8_t g,r,b,pad;
	} c;
	
} led_color;

typedef union hsv_t { 
	
	uint32_t hsv;
	
	struct hsv_tag { 
		uint8_t h,s,v,pad;
	} h;
	
} hsv_color;


#define ONOFF_RANDOM  0
#define ONOFF_SEQ     1
#define ONOFF_CHASE   2
#define ONOFF_ALLON   3

#define LEFT_ROTATION 0
#define RIGHT_ROTATION 1

#define EVEN_MASK  0xAAAAAAAA
#define ODD_MASK   0x55555555

#define CHASE0_MASK  0x6DB6
#define CHASE1_MASK  0xDB6D
#define CHASE2_MASK  0xB6DB

#define SPARKLE_ONE   (0)
#define SPARKLE_MULTI (1)

extern uint32 const StripLights_CLUT[];  // CLUT (Color lookup table array)

uint32 TweenerALLHSV( uint16_t count, uint32 from,uint32 to,uint32 delay);
uint32 TweenerHSV( uint16_t startx,  uint16_t count, uint32 from,uint32 to,uint32 delay,int direction);
void Stripe(void);
void SingleLEDPingPong( uint16_t count , uint8 fade_amount, uint32 c1);
void CandyCane ( uint16_t count , uint32 c1, uint32 c2 );
void Snake( uint16_t count );
void Twinkle( uint16_t count );
void ColorWheel( uint16_t count );
void Tweener( uint16_t count ,uint32 source);
void ColorFader( int count  , uint32 color);
void Tween1( void );
void Icicle (uint8 redraw, uint8 length, int fade_amount );           
void Sparkler ( uint16 runtime, int fade_amount , int num_sparkles ,char white );
void CandyCaneSmooth ( uint16_t count , led_color c1, led_color c2 );

uint32 TweenC1toC2(led_color c1, led_color c2,int amount);
int TweenC1toC2Range( uint16_t count, uint16_t x, uint32 source, uint32 target);
hsv_color RgbToHsv(led_color rgb);
led_color HsvToRgb(hsv_color hsv);
void StripLights_PixelHSV(int32 x,int32 y,hsv_color hsv );
void StripLights_DisplayClearHSV(hsv_color hsv );
led_color hsv_to_rgb(hsv_color hsv);
hsv_color rgb_to_hsv(led_color rgb);
uint8 TweenU8toU8(uint8 source, uint8 target,int amount);
void FadeToColor( uint16_t startx, uint16_t count, uint32 target, uint32 delay, int direction) ;
void FadeStrip(uint16 start, uint16 length, int percentage);
uint32 AddColor( led_color c1, led_color c2 );

void SingleLED(uint32 delay, uint32 mode, uint32 left_right, uint32 main_color, uint32 background);
void RgbChase(uint32 delay, uint32 mode, uint32 left_right);
void Rainbow(uint32 delay, uint32 mode);
void OppositeRings(uint32 delay, uint32 mode, uint32 color1, uint32 color2);
void OneColor(uint32 delay, uint32 mode);
void Sparkle(uint32 delay, uint32 mode);
void RingRainbow(uint32 delay, uint32 mode);
void  Gwiazdki(uint32 delay, uint32 count, uint32 mode, uint32 background);
void CaleLatanie(uint32 delay, uint32 count, uint32);

void CyDelay(uint32_t delay);
bool SW1_Read(void);
bool TimeOut(void);

uint8_t at_parse(const char* data, const char* data_expected);
uint16_t at_getstr( char *const buffer ,uint16_t length,uint16_t timeout);
/* [] END OF FILE */

#endif
