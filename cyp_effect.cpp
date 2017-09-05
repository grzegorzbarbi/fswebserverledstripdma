/* ========================================
 *
 *  This project was written to have a little
 *  fun with the NeoPixel LED rings that are 
 *  sold by Adafruit.com.  They use the WS2812
 *  RGB LED modules that allow you to string
 *  a thousand or more RGB LEDs together on a
 *  single IO pin.
 * 
 *  This project takes the "P4_OneRing" project
 *  to the next level by allowing the control
 *  of multiple rings on the same string.
 *
 *  The StripLight component makes using these
 *  module very easy, so you can use your time
 *  to code fun light patterns instead of worrying
 *  about tight timing loops, so enjoy.
 *
 *  To use, make sure you have the "Din" pin to the
 *  GPIO pin that you have connected your LED ring
 *  to.  Also set the "LEDs_per_strip" parameter to
 *  the total number of LEDs in all your rings.  Also
 *  set the constant "RING_LEDS" to the number of LEDs
 *  in a single ring and the "RINGS" constant to the 
 *  number of rings on your string.  You can find 
 *  these constants in the code below. Build the project
 *  then impress your friends and co-workers.
 *
 *  The project will advance through the different 
 *  patterns automatically.  Set the constant
 *  "PATTERN_INTERVAL" for the desired interval.
 *
 *  Pin Din  P02  Serial data to WS2812 module/s
 *      SW1  P07  Mode Switch.
 *
 *  11-18-2014 Mark Hastings  Initial version
 *  11-24-2014 Mark Hastings  Minor changes and comments
 * ========================================
*/
 
#include "stdlib.h"
#include <ESP8266WiFi.h>
#include "effects.h"
#include "StripLights.h"



#define RING_LEDS        (24)    // Number of LEDs per ring
#define RINGS            (12)    // Number of rings on a string
#define COUNT_LEN        300  
#define PATTERN_INTERVAL (10000) // 10 seconds (10,000 msec)



// Chase pattern array
uint32 chaseArray[3] = {CHASE0_MASK, CHASE1_MASK, CHASE2_MASK };

// Pattern function prototypes
void SingleLED(uint32 delay, uint32 mode, uint32 left_right,uint32 main_color, uint32 background);
void RgbChase(uint32 delay, uint32 mode, uint32 left_right);
void Rainbow(uint32 delay, uint32 mode);
void OppositeRings(uint32 delay, uint32 mode, uint32 color1, uint32 color2);
void OneColor(uint32 delay, uint32 mode);
void Sparkle(uint32 delay, uint32 mode);
void RingRainbow(uint32 delay, uint32 mode);
void  Gwiazdki(uint32 delay, uint32 count, uint32 mode,uint32 background);
void CaleLatanie(uint32 delay, uint32 count, uint32);

// Misc function prototypes
void RingOnOff(uint32 mode);  // Different modes to flash the rings



int32 ticTocCounter = 0;      // Timeout counter
uint32 rand_visualisation;




uint8  StripLights_initvar = 0;

#if(StripLights_MEMORY_TYPE == StripLights_MEMORY_RGB)
uint32  StripLights_ledArray[StripLights_ARRAY_ROWS][StripLights_ARRAY_COLS];
#else
uint8   StripLights_ledArray[StripLights_ARRAY_ROWS][StripLights_ARRAY_COLS];
#endif

uint32  StripLights_ledIndex = 0;
uint32  StripLights_row = 0;
uint32  StripLights_refreshComplete;

uint32  StripLights_DimMask;
uint32  StripLights_DimShift;

// 

uint32 const StripLights_CLUT[] = {
	//xxBBRRGG (WS2812)
	//     (24)   [ Index = 0 ]
	0x0000FFFF,  // 0 Yellow
	0x0000CCFF,
	0x000099FF,
	0x000033FF,
	0x000000FF,  // 5  Green
	0x006600B3,
	0x00990099,
	0x00B30066,
	0x00CC0033,  // 9 Blue
	0x00B31919,
	0x00993300,
	0x00994000,
	0x00996600,
	0x00999900,
	0x0099CC00,
	0x0066E600,
	0x0000FF00,
	0x0000FF33,
	0x0000FF66,
	0x0000FF80,
	0x0000FF99,  // 20 Orange
	0x0000FFB2,
	0x0000FFCC,
	0x0000FFE5,
	// #24

	//xxBBRRGG  (64)  [ Index = 24 ]
	0x00000000, 0x00550000, 0x00AA0000, 0x00FF0000,  // 0, Black,  LtBlue, MBlue, Blue
	0x00000055, 0x00550055, 0x00AA0055, 0x00FF0055,  // 4, LtGreen
	0x000000AA, 0x005500AA, 0x00AA00AA, 0x00FF00AA,  // 8, MGreen
	0x000000FF, 0x005500FF, 0x00AA00FF, 0x00FF00FF,  // 12 Green

	0x00005500, 0x00555500, 0x00AA5500, 0x00FF5500,  // 16, LtRed
	0x00005555, 0x00555555, 0x00AA5555, 0x00FF5555,  // 20, LtYellow
	0x000055AA, 0x005555AA, 0x00AA55AA, 0x00FF55AA,  // 24, 
	0x000055FF, 0x005555FF, 0x00AA55FF, 0x00FF55FF,  // 28,

	0x0000AA00, 0x0055AA00, 0x00AAAA00, 0x00FFAA00,  // 32, MRed
	0x0000AA55, 0x0055AA55, 0x00AAAA55, 0x00FFAA55,  // 36, 
	0x0000AAAA, 0x0055AAAA, 0x00AAAAAA, 0x00FFAAAA,  // 55, 
	0x0000AAFF, 0x0055AAFF, 0x00AAAAFF, 0x00FFAAFF,  // 44, 

	0x0000FF00, 0x0055FF00, 0x00AAFF00, 0x00FFFF00,  // 48, Red, ??, ??, Magenta
	0x0000FF55, 0x0055FF55, 0x00AAFF55, 0x00FFFF55,  // 52,
	0x0000FFAA, 0x0055FFAA, 0x00AAFFAA, 0x00FFFFAA,  // 56,
	0x0000FFFF, 0x0055FFFF, 0x00AAFFFF, 0x00FFFFFF,  // 60, Yellow,??, ??, White

	// Misc ( 16 )  [ Index = 88 ]
	0x000080FF,  // SPRING_GREEN
	0x008000FF,  // TURQUOSE
	0x00FF00FF,  // CYAN
	0x00FF0080,  // OCEAN
	0x00FF8000,  // VIOLET
	0x0080FF00,  // RASPBERRY
	0x000000FF,  // GREEN
	0x00202020,  // DIM WHITE
	0x00200000,  // DIM BLUE
	0x10000000,  // INVISIBLE
	0x00000000,  // Spare
	0x00000000,  // Spare
	0x00000000,  // Spare
	0x00000000,  // Spare
	0x00000000,  // Spare
	0x00000000,  // Spare

	// Temperture Color Blue to Red (16) [ Index = 104 ]
	0x00FF0000, 0x00F01000, 0x00E02000, 0x00D03000,
	0x00C04000, 0x00B05000, 0x00A06000, 0x00907000,
	0x00808000, 0x00709000, 0x0060A000, 0x0050B000,
	0x0040C000, 0x0030D000, 0x0020E000, 0x0000FF00
};


int main_cyp()
{
	

	
	// Initialize StripLights


   
	
	//Dioda_1 = SW1_0;
	/*
	StripLights_YELLOW      
	StripLights_GREEN       
	StripLights_ORANGE      
	StripLights_BLACK       
	StripLights_OFF         
	StripLights_LTBLUE      
	StripLights_MBLUE       
	StripLights_BLUE        
	StripLights_LTGREEN     
	StripLights_MGREEN      
	StripLights_LTRED       
	StripLights_LTYELLOW    
	StripLights_MGRED       
	StripLights_RED         
	StripLights_MAGENTA     
	StripLights_WHITE       
	
	StripLights_SPRING_GREEN
	StripLights_TURQUOSE    
	StripLights_CYAN        
	StripLights_OCEAN       
	StripLights_VIOLET      
	StripLights_RASPBERRY   
	StripLights_DIM_WHITE   
	StripLights_DIM_BLUE    
	StripLights_INVISIBLE   
	
	ONOFF_RANDOM 
	ONOFF_SEQ    
	ONOFF_CHASE  
	ONOFF_ALLON  
	
	
	*/
			 
	int stan_led;
	
	stan_led = 0;
	
	rand_visualisation = 44;
	
	#define COUNT_TYPE_VISUALISATION 59
	
   //rand_visualisation = rand ();
	
	
	/*
	
	// Loop through the different patterns each time SW1 is pressed.
	for(;;)
	{   // StripLights_
		
		LED_NIEBIESKA_Write(stan_led);
		stan_led = !stan_led;
	   
		// zeremuj następną linię jeżeli chcesz kolejne ilumnacje         
	   rand_visualisation = rand() %  COUNT_TYPE_VISUALISATION;
		
	   // zaremuj następne 3 linijki jeśli chcesz przypadkowe ilumnacje
	   // rand_visualisation++;
		//if(rand_visualisation>COUNT_TYPE_VISUALISATION)
		//    rand_visualisation=4;
		
		
		switch(rand_visualisation)
		{
			//////////////////////////
			
			case 45:
				Icicle(6,9 , 6);
				break;
			case 46:
				Sparkler( 100,1,8 ,0);
				break;
			
			case 47:
				Sparkler(100,1,8 ,1);
				break;
			
			case 48:
				StripLights_DisplayClear( StripLights_LTGREEN );
				break;
			case 49:
				StripLights_DisplayClear( StripLights_WHITE );
				break;
			case 50:
				StripLights_DisplayClear( StripLights_BLUE );
				break;
			case 51:
				SingleLEDPingPong(1 ,6, StripLights_getColor(rand()%StripLights_COLOR_WHEEL_SIZE));
				break;
			case 52:
				Tween1();
				break;
			case 53:
				ColorFader(1, StripLights_getColor(rand()%StripLights_COLOR_WHEEL_SIZE));
				break;
			case 54:
				CandyCane(5, StripLights_RED, StripLights_WHITE );
				break;
			case 55:
				Snake(5);
				break;
			case 56:
				ColorWheel(10);
				break;
			case 57:
				CandyCane(5, StripLights_getColor(rand()%StripLights_COLOR_WHEEL_SIZE), StripLights_WHITE );
				break;
				
			case 58:
				CandyCaneSmooth(5, (led_color)StripLights_getColor(rand()%StripLights_COLOR_WHEEL_SIZE), 
								(led_color)StripLights_getColor(rand()%StripLights_COLOR_WHEEL_SIZE) );
				break;

			case 59:
				CandyCaneSmooth(5, (led_color)StripLights_getColor(rand()%StripLights_COLOR_WHEEL_SIZE), 
								(led_color)StripLights_getColor(rand()%StripLights_COLOR_WHEEL_SIZE) );
				break;
	 
				
				
				
			case 4:
				OppositeRings(50, ONOFF_ALLON,StripLights_BLUE,StripLights_RED );    
			break;
		
			case 0:
				OppositeRings(50, ONOFF_ALLON,StripLights_GREEN,StripLights_RED );
			break;

			case 2:
				RgbChase(20, ONOFF_ALLON, RIGHT_ROTATION);        
			break;
			case 3:
				RgbChase(20, ONOFF_ALLON, LEFT_ROTATION);        
			break;
		
			case 30:
				RgbChase(10, ONOFF_ALLON, RIGHT_ROTATION);        
			break;
			case 31:
				RgbChase(10, ONOFF_ALLON, LEFT_ROTATION);        
			break;
			
				
				
// gwiazdki                
			case 34:
				 Gwiazdki(25,5,0,StripLights_WHITE);   
			break;
				
			case 35:
				  Gwiazdki(25,5,0,StripLights_LTRED);
			break;
			
			case 36:
				  Gwiazdki(5,5,1,StripLights_CYAN);   
			break;
			case 37:
				  //długie zanikanie szybko
				  Gwiazdki(5,5,1,0);
			break;
			case 38:
				   Gwiazdki(25,5,0,0);  
			break;
			case 39:
				 Gwiazdki(25,5,0,StripLights_BLUE);   
			break;
			case 40:
				 Gwiazdki(25,5,0,StripLights_GREEN);   
			break;
			case 41:
				 Gwiazdki(25,5,0,StripLights_RED);   
			break;
				
			//losowy jeden kolor mało
			case 42:
				 Gwiazdki(25,1,0,rand() % StripLights_COLOR_WHEEL_SIZE);   
			break;
			//losowy jeden kolor mało
			case 43:
				 Gwiazdki(25,5,0,rand() % StripLights_COLOR_WHEEL_SIZE);   
			break;
		   
			case 44:
				 CaleLatanie(0,10);
			break;
				
				
				
// konie gwiazdek
				
			case 1:
				SingleLED(30, ONOFF_ALLON,LEFT_ROTATION,StripLights_BLUE,StripLights_BLACK);    
			break;
			
			case 5:
				SingleLED(20, ONOFF_ALLON,RIGHT_ROTATION,StripLights_GREEN,StripLights_BLACK);        
			break;
			
			case 6:
				SingleLED(30, ONOFF_ALLON,LEFT_ROTATION,StripLights_CYAN,StripLights_BLACK);        
			break;
			
			case 7:
				SingleLED(20, ONOFF_ALLON,RIGHT_ROTATION,StripLights_VIOLET,StripLights_BLACK);        
			break;

			case 25:
				SingleLED(20, ONOFF_ALLON,RIGHT_ROTATION,StripLights_RED,StripLights_BLACK);        
			break;

			case 21:
				SingleLED(20, ONOFF_ALLON,RIGHT_ROTATION,StripLights_WHITE,StripLights_BLACK);        
			break;
				
			case 32:
				SingleLED(20, ONOFF_ALLON,RIGHT_ROTATION,StripLights_WHITE,StripLights_BLACK);        
			break;

			case 33:
				SingleLED(20, ONOFF_ALLON,LEFT_ROTATION,StripLights_WHITE,StripLights_BLACK);        
			break;

				
			case 8:
				RgbChase(100, ONOFF_ALLON, RIGHT_ROTATION);        
			break;
			
			case 9:
				 RgbChase(100, ONOFF_ALLON, LEFT_ROTATION);        
			break;
			
			case 10:
				 OneColor(10, ONOFF_CHASE);         
			break;

			case 11:
				 OneColor(80, ONOFF_CHASE);         
			break;

			case 12:
				  RingRainbow(100, ONOFF_ALLON);         
			break;
				
			case 13:
				 RingRainbow(100, ONOFF_ALLON);         
			break;
				
			case 14:
				 RingRainbow(100, ONOFF_ALLON);         
			break;
				
			case 15:
				  RingRainbow(100, ONOFF_ALLON);        
			break;
				
			case 16:
				 Sparkle(100, SPARKLE_MULTI);         
			break;
				
			case 17:
				 Sparkle(150, SPARKLE_ONE);         
			break;
				
			case 18:
				OppositeRings(120, ONOFF_ALLON,StripLights_WHITE, StripLights_BLUE);          
			break;
				
			case 19:
				OneColor(250, ONOFF_CHASE);          
			break;
				
			case 20:
				OneColor(10, ONOFF_ALLON);          
			break;
				
						  
			case 22:
				Rainbow(100, ONOFF_CHASE);            
			break;
				
			case 23:
				Rainbow(100, ONOFF_ALLON);          
			break;
				
			case 24:
				OppositeRings(50, ONOFF_ALLON,StripLights_WHITE,StripLights_RED );           
			break;
				
			case 26:
				Rainbow(30, ONOFF_CHASE);            
			break;
				
			case 27:
				Rainbow(30, ONOFF_ALLON);          
			break;
				
			case 28:
				Rainbow(30, ONOFF_SEQ);
			break;
				
			case 29:
				OppositeRings(20, ONOFF_ALLON,StripLights_WHITE, StripLights_BLUE);          
			break;

				
				  
				
				
			default:
				
				break;
				
		}
		
		
		
		
	
		
		
		
		
		
		
		
		
		
		
		
		
		
		//SingleLED(50, ONOFF_ALLON,1,0);
		
		
		
	}


	*/
}



/************************************************
*                    TimeOut()
*
*  Check to see if the timer has expired.
*
* Return:  1 if timer has expired
*          0 if timer has not expired
*
************************************************/

/*
uint32 TimeOut(void)
{
	uint32 timeOut = 0;
	if (ticTocCounter > PATTERN_INTERVAL)
	{
		timeOut = 1;
		ticTocCounter = 0;
	}

	return(timeOut);
}
*/

/************************************************
 *                    CaleLatanie()
 *
 * Bieganie diody lub pasemka w kółka po dwóch liniach 
 * Params:  
 *   delay: Time in msec between loop iteration
 *   count: ilosc na raz gwiazdek w procentach 
 *   mode: 0 - proste szybkie gaszenie, 1- proste wolne gaszenie, 2-mieszane.
 *   background: kolor jeden wybrany tylko się pokazuje
 *  
 ************************************************/


void CaleLatanie(uint32 delay, uint32 count, uint32 color)
{
	
	   // uint32 ledPosition;
	   // uint32 color,fromColor,toColor;
		//uint32 oldColor;
	   // uint32 count_cur;
		int pct;
		uint32 whichRound;
		int par;
 
		// Wait for last update to finish
		while( StripLights_Ready() == 0);     

		if (!color)
			color = rand() % StripLights_COLOR_WHEEL_SIZE;
	
		if (!count)
			count = 1;
		
			// Clear all LEDs to background color
		//
 //       if(background)
 //           StripLights_MemClear(background);   
//		else
			StripLights_MemClear(StripLights_BLACK);   
		pct = 0;
		whichRound=1;
			
		for(;;)  
	  {           
		 while( StripLights_Ready() == 0);     
		
		if(whichRound)
			{
				 StripLights_MemClear(StripLights_BLACK);   
				 for (par = 0; par < count;par++)
				 {
					 StripLights_Pixel(pct+par, 0, color);
				 }
				 
				
				 pct+=count;
				if(pct>COUNT_LEN)
				{
					
					whichRound=0;
						
				}
		
			}
		else
			{
				 StripLights_MemClear(StripLights_BLACK);   
				 for (par = 0; par < count; par++)
				 {
					 StripLights_Pixel(pct - par, 1, color);
				 }

				pct -= count;
				if(pct<=0)
				{
					
					whichRound=1;
						
				}
	
			
			}
		
		
		
		// Trigger update of all LEDs at once
		//RingOnOff(mode);
		StripLights_Trigger(1);    
		
		// Loop delay
		CyDelay(delay);    
		
		
		// If SW1 is pressed, leave loop
		if(SW1_Read() == 0) break; 
		if(TimeOut()) break;
	   }	
	
	// Wait for button release and delay to debounce switch
	while(SW1_Read() == 0);   
	CyDelay(50); 
	 
	
	
	
}


/************************************************
 *                    Gwiazdki()
 *
 * Losowe diody zapalają się od najmocniejszej losowej wartości .
 * i sie wygaszaja
 * Params:  
 *   delay: Time in msec between loop iteration
 *   count: ilosc na raz gwiazdek w procentach 
 *   mode: 0 - proste szybkie gaszenie, 1- proste wolne gaszenie, 2-mieszane.
 *   background: kolor jeden wybrany tylko się pokazuje
 *  
 ************************************************/

void Gwiazdki(uint32 delay, uint32 count, uint32 mode,uint32 background)
{
		uint32 ledPosition;
		uint32 color,fromColor,toColor;
		uint32 oldColor;
		uint32 count_cur,pct;
 
		// Wait for last update to finish
		while( StripLights_Ready() == 0);     
	
		
			// Clear all LEDs to background color
		//
 //       if(background)
 //           StripLights_MemClear(background);   
//		else
			StripLights_MemClear(StripLights_BLACK);   
		
		for(;;)  
	  {        

		while( StripLights_Ready() == 0);   
		// wygaszanie istniejących
		
		if(mode==0)
			for(ledPosition=0;ledPosition<COUNT_LEN;ledPosition++)
			{
				oldColor = StripLights_GetPixel(ledPosition, 0);
				oldColor = (oldColor >> 1) & 0x007F7F7F;
				StripLights_Pixel(ledPosition, 0, oldColor );
				StripLights_Pixel(ledPosition, 1, oldColor );
			}
		else
			if(mode==1)
			{
				for(ledPosition=0;ledPosition<COUNT_LEN;ledPosition++)
				{
					oldColor = StripLights_GetPixel(ledPosition, 0);
					fromColor = oldColor;
					toColor = StripLights_BLACK;
					pct=8;
					oldColor  = (((pct * (toColor & 0x00FF0000)) + ((100-pct) * (fromColor & 0x00FF0000)))/100) & 0x00FF0000;
					oldColor |= (((pct * (toColor & 0x0000FF00)) + ((100-pct) * (fromColor & 0x0000FF00)))/100) & 0x0000FF00;
					oldColor |= (((pct * (toColor & 0x000000FF)) + ((100-pct) * (fromColor & 0x000000FF)))/100) & 0x000000FF;	

					StripLights_Pixel(ledPosition, 0, oldColor );
					StripLights_Pixel(ledPosition, 1, oldColor );
					
				}
			}

		// przypadkowe rysowanie punktów        
		
		for(count_cur=0;count_cur<count;count_cur++)
		{
			
			color = rand() % StripLights_COLOR_WHEEL_SIZE;
			ledPosition= rand()%COUNT_LEN;
			if(background)
			{
				StripLights_Pixel(ledPosition , 0, background );		
				StripLights_Pixel(ledPosition , 1, background );		
			}
			else
			{
				StripLights_Pixel(ledPosition , 0, StripLights_CLUT[color] );	
				StripLights_Pixel(ledPosition , 1, StripLights_CLUT[color] );	
			}
		   
		}

		// Trigger update of all LEDs at once
		//RingOnOff(mode);
		StripLights_Trigger(1);    
		
		// Loop delay
		CyDelay(delay);    
		
		
		// If SW1 is pressed, leave loop
		if(SW1_Read() == 0) break; 
		if(TimeOut()) break;
	   }	
	
	// Wait for button release and delay to debounce switch
	while(SW1_Read() == 0);   
	CyDelay(50); 
	
	
}


/************************************************
 *                    SingleLED()
 *
 *  Rotate a single LED around the ring.
 *
 * Params:  
 *   delay: Time in msec between loop iterations
 *   mode:  Flash mode.
 *
 ************************************************/
void SingleLED(uint32 delay, uint32 mode, uint32 left_right,uint32 main_color, uint32 background)
{
	uint32 ledPosition = 0;  // On LED position
	uint32 rings = 0;
	
	if(left_right==0)
		ledPosition = RING_LEDS;
	

	// Loop until SW1 pressed
	for(;;)  
	{
		// Wait for last update to finish
		while( StripLights_Ready() == 0);                 
		
		// Clear all LEDs to background color
		//
		if(background)
			StripLights_MemClear(background);   
		else
			StripLights_MemClear(StripLights_BLACK);   
		
		// Set the color of a single LED
		for(rings = 0; rings <= RINGS; rings++ )
		{
			StripLights_Pixel((ledPosition + (rings * RING_LEDS)), 0, main_color );	
			StripLights_Pixel(COUNT_LEN-(ledPosition + (rings * RING_LEDS)), 1, main_color );	
					 
		}
		 

		// Trigger update of all LEDs at once
		RingOnOff(mode);
		StripLights_Trigger(1);    
		
		// Loop delay
		CyDelay(delay);    

		// Advance to next position.  If too far, start back at 0
		if(left_right)
		{
			ledPosition++;  
			if(ledPosition >= RING_LEDS) ledPosition = 0;
		}
		else
		{
			if(ledPosition)
				ledPosition--;  
			else
				ledPosition = RING_LEDS-1;
		}
		

		// If SW1 is pressed, leave loop
		if(SW1_Read() == 0) break; 
		if(TimeOut()) break;
	}	
	
	// Wait for button release and delay to debounce switch
	while(SW1_Read() == 0);   
	CyDelay(50);
}



/************************************************
 *                   OppositeRings()
 *
 *  A green and red LEDs go around the ring in 
 *  opposite directions, with tails.
 *
 * Params:  
 *   delay: Time in msec between loop iterations
 *   mode:  Flash mode.
 ************************************************/
void OppositeRings(uint32 delay, uint32 mode, uint32 color1, uint32 color2)
{
	uint32 fwdLED = 0;  // Forward LED Position
	uint32 bwdLED = 0;  // Backward LED Postion
	uint32 rings = 0;

	uint32 oldColor;    // Temp Color
	uint32 loop;        // Loop counter
	
	// Loop until button pressed
	for(;;)
	{
		// Loop through one revolution
		for(fwdLED = 0; fwdLED < RING_LEDS; fwdLED++)
		{
			// Calculate backward LED position
			bwdLED = RING_LEDS - fwdLED;
			
			// Wait for last update to complete
			while( StripLights_Ready() == 0);
			
			
			for(rings = 0; rings <= RINGS; rings++ )
			{
			   
				// Loop around and dim all the LEDs to create tails
				for(loop = 0; loop < RING_LEDS; loop++)
				{
					oldColor = StripLights_GetPixel((loop + (rings * RING_LEDS)), 0);
					oldColor = (oldColor >> 1) & 0x007F7F7F;
					// stripLights_Pixel(loop, 0, oldColor );	
					StripLights_Pixel((loop + (rings * RING_LEDS)), 0, oldColor );
					StripLights_Pixel((loop + (rings * RING_LEDS)), 1, oldColor );
				}

				// Blend the forward LED with existing colors
				oldColor = StripLights_GetPixel((fwdLED + (rings * RING_LEDS)), 0) | color1;
				StripLights_Pixel((fwdLED + (rings * RING_LEDS)), 0, oldColor );
				StripLights_Pixel((fwdLED + (rings * RING_LEDS)), 1, oldColor );
				
				// Blend the backward LED with existing colors
				oldColor = StripLights_GetPixel((bwdLED + (rings * RING_LEDS)), 0) | color2;
				StripLights_Pixel((bwdLED + (rings * RING_LEDS)), 0,  oldColor );
				StripLights_Pixel((bwdLED + (rings * RING_LEDS)), 1,  oldColor );
			
			}
			// Udate all the LEDs at once
			RingOnOff(mode);
			StripLights_Trigger(1);
			
			// Loop delay
			CyDelay(delay);
		
			// Leave loop is SW1 pressed
			if(SW1_Read() == 0) break;
			
		}

		// Leave if SW1 pressed
		if(SW1_Read() == 0) break;
		if(TimeOut()) break;
	}
	
	// Wait for SW1 to be released then delay a bit for debouncing
	while(SW1_Read() == 0);
	CyDelay(50);
}

/************************************************
 *                    RgbChase()
 *
 *  Rotate three LED colors around the ring.
 *  Make tails by dimming the LEDs each time.
 *
 * Params:  
 *   delay: Time in msec between loop iterations
 *   mode:  Flash mode.
 ************************************************/
void RgbChase(uint32 delay, uint32 mode, uint32 left_right)
{
	uint32 redPosition;   
	uint32 greenPosition; 
	uint32 bluePosition;  
	uint32 rings;
	
	uint32 oldColor;   // Temp color value
	uint32 loop;       // Loop counter
	
	if(left_right)
	{
		redPosition   = 0; // Initial colo  
		greenPosition = RING_LEDS/3;
		bluePosition  = (RING_LEDS * 2)/3;
	}
	else
	{
		redPosition   = RING_LEDS-1; // Initial colo  
		greenPosition = RING_LEDS/3;
		bluePosition  = (RING_LEDS * 2)/3;
	}
	
	
	
	
	// Loop until SW1 pressed
	for(;;)
	{
		// Wait for last update to complete
		while( StripLights_Ready() == 0);
		
		for(rings = 0; rings <= RINGS; rings++ )
		{
		
			// Loop through and dim all LEDs by half
			for(loop = 0; loop < RING_LEDS; loop++)
			{
				oldColor = StripLights_GetPixel(( loop + (rings * RING_LEDS)), 0);
				oldColor = (oldColor >> 1) & 0x007F7F7F;
				StripLights_Pixel((loop + (rings * RING_LEDS)), 0, oldColor );	
				StripLights_Pixel(COUNT_LEN - (loop + (rings * RING_LEDS)), 1, oldColor);
			}
			
	
			// Set the three LEDs at their new position
			StripLights_Pixel((redPosition   + (rings * RING_LEDS)), 0,  StripLights_RED );
			StripLights_Pixel((greenPosition + (rings * RING_LEDS)), 0,  StripLights_GREEN );
			StripLights_Pixel((bluePosition  + (rings * RING_LEDS)), 0,  StripLights_BLUE );

			StripLights_Pixel(COUNT_LEN - (redPosition + (rings * RING_LEDS)), 1, StripLights_RED);
			StripLights_Pixel(COUNT_LEN-(greenPosition + (rings * RING_LEDS)), 1,  StripLights_GREEN );
			StripLights_Pixel(COUNT_LEN-(bluePosition  + (rings * RING_LEDS)), 1,  StripLights_BLUE );

			
			

		}
		// Trigger to update all the LEDs at once
		RingOnOff(mode);
		 StripLights_Trigger(1);
		
		// Calculate new LED positions around the ring
		if(left_right)
		{
			redPosition   = (redPosition + 1)   % RING_LEDS;
			greenPosition = (greenPosition + 1) % RING_LEDS;
			bluePosition  = (bluePosition + 1)  % RING_LEDS;
		}
		else
		{
			if(redPosition)   
				redPosition   = (redPosition - 1)   % RING_LEDS;
			else
				redPosition   = RING_LEDS-1;
				
			if(greenPosition) 
				greenPosition = (greenPosition - 1) % RING_LEDS;
			else
				greenPosition = RING_LEDS -1;
				
			if(bluePosition)    
				bluePosition  = (bluePosition - 1)  % RING_LEDS;
			else
				bluePosition = RING_LEDS -1;
		}
		
		// Loop delay in mSeconds
			CyDelay(delay);
			
		// If SW1 pressed, leave loop
		if(SW1_Read() == 0) break;
		if(TimeOut()) break;
		
	}	
	// Wait for SW1 to be released then delay a bit for debouncing
	while(SW1_Read() == 0);
	CyDelay(50);
}

/************************************************
 *                    Rainbow()
 *
 *  Use the colorwheel section of the color lookup
 *  table to create a rotating rainbow.
 *
 * Params:  
 *   delay: Time in msec between loop iterations
 *   mode:  Flash mode.
 ************************************************/
void Rainbow(uint32 delay, uint32 mode)
{
	uint32 startColor = 0;  // Index color in colorwheel
	uint32 ledPosition = 0; // LED position when setting color         
	uint32 color = 0;       // Temp color to set LED
	uint32 rings = 0;
	
	for(;;)
	{   
		// Wait for last update to complete
		while( StripLights_Ready() == 0);  
		
		// Set new start color
		color = startColor;    
		
		for(rings = 0; rings <= RINGS; rings++ )
		{
			// Loop through all LEDs giving each one a different color from the color wheel
			for(ledPosition = 0; ledPosition <= RING_LEDS; ledPosition++)
			{
				StripLights_Pixel((ledPosition + (rings * RING_LEDS)), 0, StripLights_getColor( color ));
				StripLights_Pixel(COUNT_LEN-(ledPosition + (rings * RING_LEDS)), 1, StripLights_getColor( color ));
				color++;
				if(color >= StripLights_COLOR_WHEEL_SIZE) color = 0;
			}
		}
		
		// Update the LEDs all at once
		RingOnOff(mode);
		StripLights_Trigger(1); 
		
		// Each time through the main loop start with different color
		// Wrap around at end of color wheel
		startColor++;  
		if(startColor >= StripLights_COLOR_WHEEL_SIZE) startColor = 0;
	
		CyDelay(delay);   // Loop delay

		if(SW1_Read() == 0) break;  // If SW1 pressed leave
		if(TimeOut()) break;
	}	
	// Wait for SW1 to be released then delay a bit for debouncing
	while(SW1_Read() == 0);         
	CyDelay(50);
}

/************************************************
 *                    RingRainbow()
 *
 *  Use the colorwheel section of the color lookup
 *  table to create a rainbow across the rings.
 *  Each ring will have a different color, but the
 *  entire ring will be the same color.
 *
 * Params:  
 *   delay: Time in msec between loop iterations
 *   mode:  Flash mode.
 ************************************************/
void RingRainbow(uint32 delay, uint32 mode)
{
	uint32 startColor = 0;  // Index color in colorwheel        
	uint32 color = 0;       // Temp color to set LED
	uint32 ring = 0;
	uint32 xpos1, xpos2;
	
	for(;;)
	{   
		// Wait for last update to complete
		while( StripLights_Ready() == 0);  
		
		color = startColor;
		for(ring = 0; ring <= RINGS; ring++ )
		{
			xpos1 = ring * RING_LEDS;
			xpos2 = xpos1 + RING_LEDS - 1;

			StripLights_DrawLine(xpos1, 0, xpos2, 0, StripLights_CLUT[color]);
			StripLights_DrawLine(COUNT_LEN-xpos1, 1,COUNT_LEN- xpos2, 1, StripLights_CLUT[color]);
			
			color++;
			 if(color >= StripLights_COLOR_WHEEL_SIZE) color = 0;
			
		}
		
		startColor++;
		if(startColor >= StripLights_COLOR_WHEEL_SIZE) startColor = 0;
		
		RingOnOff(mode);
		StripLights_Trigger(1); 
			
		CyDelay(delay);  // Loop delay
		
		if(SW1_Read() == 0) break;  // If SW1 pressed leave
		if(TimeOut()) break;
	}	
	// Wait for SW1 to be released then delay a bit for debouncing
	while(SW1_Read() == 0);         
	CyDelay(50);
}

/************************************************
 *                    OneColor()
 *
 *  Use the colorwheel section of the color lookup
 *  table to write all LEDs the same color.
 *
 * Params:  
 *   delay: Time in msec between loop iterations
 *   mode:  Flash mode.
 ************************************************/
void OneColor(uint32 delay, uint32 mode)
{
	uint32 color;  // Index color in colorwheel
	uint32 nextColor = 0;
	uint32 pct = 0;
	uint32 toColor, fromColor, newColor;
	uint32 ledPosition = 0; // LED position when setting color  
	uint32 rings = 0;
	
	color = rand() % StripLights_COLOR_WHEEL_SIZE;
	
	for(;;)
	{   
		nextColor = color + 1;
		if(nextColor >= StripLights_COLOR_WHEEL_SIZE) nextColor = 0;
		
		fromColor = StripLights_CLUT[color];
		toColor   = StripLights_CLUT[nextColor];  

		for(pct = 0; pct <= 100; pct += 10)
		{
			newColor  = (((pct * (toColor & 0x00FF0000)) + ((100-pct) * (fromColor & 0x00FF0000)))/100) & 0x00FF0000;
			newColor |= (((pct * (toColor & 0x0000FF00)) + ((100-pct) * (fromColor & 0x0000FF00)))/100) & 0x0000FF00;
			newColor |= (((pct * (toColor & 0x000000FF)) + ((100-pct) * (fromColor & 0x000000FF)))/100) & 0x000000FF;	
			
			// Wait for last update to complete
			 while( StripLights_Ready() == 0); 
			
			// Loop through all LEDs giving each one a different color from the color wheel
			for(ledPosition = 0; ledPosition <= RING_LEDS; ledPosition++)
			{
				for(rings = 0; rings <= RINGS; rings++ )
				{
					StripLights_Pixel((ledPosition + (rings * RING_LEDS)), 0, newColor);
					StripLights_Pixel((ledPosition + (rings * RING_LEDS)), 1, newColor);
				}
			}
		
			// Update the LEDs all at once
			RingOnOff(mode);
			StripLights_Trigger(1); 
			
			if(SW1_Read() == 0) break;  // If SW1 pressed leave
			CyDelay(delay);   // Loop delay
		} 
		// Each time through the main loop start with different color
		// Wrap around at end of color wheel
		color++;  
		if(color >= StripLights_COLOR_WHEEL_SIZE) color = 0;

		if(SW1_Read() == 0) break;  // If SW1 pressed leave
		if(TimeOut()) break;
	}	
	// Wait for SW1 to be released then delay a bit for debouncing
	while(SW1_Read() == 0);         
	CyDelay(50);
}

/************************************************
 *                    Sparkle()
 *
 *  Use the colorwheel to randomly paint rings a
 *  different color.
 *
 * Params:  
 *   delay: Time in msec between loop iterations
 *   mode:  Flash mode.
 ************************************************/
void Sparkle(uint32 delay, uint32 mode)
{
	uint32 color;  // Index color in colorwheel
	uint32 ring;
	uint32 xpos1, xpos2;
	
	
	for(;;)
	{ 
		color = rand() % StripLights_COLOR_WHEEL_SIZE;
		ring = rand() % RINGS;
		
		xpos1 = ring * RING_LEDS;
		xpos2 = xpos1 + RING_LEDS - 1;
		
		if(mode == SPARKLE_ONE)
		   StripLights_MemClear(StripLights_BLACK);
		
		StripLights_DrawLine(xpos1, 0, xpos2, 0, StripLights_CLUT[color]);
		StripLights_DrawLine(xpos1, 0, xpos2, 1, StripLights_CLUT[color]);
		
		StripLights_Trigger(1); 
			
		CyDelay(delay);   // Loop delay

		if(SW1_Read() == 0) break;  // If SW1 pressed leave
		if(TimeOut()) break;
	}	
	// Wait for SW1 to be released then delay a bit for debouncing
	while(SW1_Read() == 0);         
	CyDelay(50);
}

/************************************************
 *                  RingOnOff()
 *
 *  A simple function to turn some of the rings
 *  off to simulate a random flash.  This works with
 *  up to 32 rings on a string.
 *
 * Params:  
 *   mode:  ONOFF_RANDOM Randomly turn off rings
 *          ONOFF_CHASE  Create a 2 on, 1 off chase pattern
 *          ONOFF_SEQ    Display only one ring at a time.
 *          ONOFF_ALLON  Leave all rings on.
 ************************************************/
void RingOnOff(uint32 mode)
{
	static uint32 seqPos = 0;
	static uint32 count = 0;
	uint32 ledMask;
	uint32 ring, loop;

	switch(mode)  // Determine the flash methos
	{
		case ONOFF_RANDOM:
		ledMask = (uint32)rand();
		break;
		
		case ONOFF_CHASE:
		ledMask = chaseArray[count % 3];
		count++;
		break;
		
		case ONOFF_SEQ:
		ledMask = 0x00000001 << seqPos;
		seqPos++;
		if(seqPos >= RINGS) seqPos = 0;
		break;
		
		case ONOFF_ALLON:
		default:
		ledMask = 0xFFFFFFFF;
		break;
	}
	
	for(ring = 0; ring <= RINGS; ring++)
	{
		if((ledMask & (0x00000001 << ring)) == 0)
		{
			// Leep througha and turn off the LEDs.
			for(loop = 0; loop < RING_LEDS; loop++)
			{
				StripLights_Pixel((loop + (ring * RING_LEDS)), 0, StripLights_BLACK );
				StripLights_Pixel(COUNT_LEN - (loop + (ring * RING_LEDS)), 1, StripLights_BLACK);
			}	
		}
	}
}

// various simple effects  (not used, sending PC does work) could be offline mode




// *******************************************************************************************
// *******************************************************************************************
// *******************************************************************************************
// *******************************************************************************************
// *******************************************************************************************




// nowe efekty wzięte z xmaz
void ColorFader( int count , uint32 color) 
{
	while(count--){
		FadeToColor( 0,StripLights_COLUMNS, color, 50,1 );
	}
}

void Tween1( void )
{
	hsv_color tween;
	static led_color src;
	static hsv_color result ;
	led_color tmp_color;
	
	src.c.r = rand()%255;
	src.c.g = rand()%255;
	src.c.b = rand()%255;

	tmp_color.rgb = StripLights_getColor(rand() % StripLights_COLOR_WHEEL_SIZE);

	tween = rgb_to_hsv(tmp_color);
	
	result.hsv = TweenerHSV(
		0,
		StripLights_COLUMNS,
		result.hsv,
		tween.hsv,
		10
		,1);
	
	// Tweener( 100,src.rgb );

	src.c.r += 5-(rand()%10);
	src.c.g += 5-(rand()%10);
	src.c.b += 5-(rand()%10);

	result.hsv = TweenerHSV(
		StripLights_COLUMNS,
		StripLights_COLUMNS,
		result.hsv,
		tween.hsv,
		10
		,-1
	);
		
}

void CandyCane ( uint16_t count , uint32 c1, uint32 c2 )
{
	int i,x;
	uint8_t flip =0;
	
	// Candy cane
	
	// loop effect for this many times
	for( i=0; i < count ; i++ )
	{   
		
		// all strip, for every other led
		for(x = StripLights_MIN_X; x <= StripLights_MAX_X; x+=2)
		{
			// if flipped. draw c1,c2 otherwise c2,c1
			if( flip ) {
				StripLights_Pixel(x, 0, c1);
				StripLights_Pixel(x+1, 0, c2);
			} else {
				StripLights_Pixel(x+1, 0, c1);
				StripLights_Pixel(x, 0, c2);
			}
		}

		// toggle flip
		flip = 1 - flip;
		
		// wait and trigger
		while( StripLights_Ready() == 0);
		StripLights_Trigger(1);
		
		// delay between transitions 
		CyDelay( 100 );        
		
		if(SW1_Read() == 0) break;  // If SW1 pressed leave
		//if(TimeOut()) break;
	}   
	ticTocCounter  = 0;
}

/*
 * FadeLED - Tween one LED to a specified colour
 *
 */
void FadeLED( uint16 i, uint32_t target, int percentage)
{
		led_color trgb, target_tmp;

		target_tmp.rgb = target;

		trgb.rgb = StripLights_GetPixel(i,0);
		
		trgb.rgb = TweenC1toC2( trgb, target_tmp, percentage ) ;

		StripLights_Pixel( i, 0, trgb.rgb );
}
		
void CandyCaneSmooth ( uint16_t count , led_color c1, led_color c2 )
{
	int i,x,percentage;
	uint8_t flip =0;
	uint32 t1,t2;
	
	// Candy cane
/*	if (0 ) {
		char buffer[256];
		sprintf(buffer,"c1 = %02x %02x %02x\n",c1.c.r,c1.c.g,c1.c.b);
		UART_UartPutString( buffer );
		sprintf(buffer,"c2 = %02x %02x %02x\n",c2.c.r,c2.c.g,c2.c.b);
		UART_UartPutString( buffer );
	}
*/	
			
	// loop effect for this many times
	for( i=0; i < count ; i++ )
	{   
		
		for( percentage = 0 ; percentage <= 100 ; percentage+=5 ) { 
			
			//  calculate target colours
			t1 = TweenC1toC2( c1, c2, percentage ) ;
			t2 = TweenC1toC2( c2, c1, percentage ) ;
						
			// all strip, for every other led
			for(x = StripLights_MIN_X; x <= StripLights_MAX_X; x+=2)
			{
				// if flipped. draw c1,c2 otherwise c2,c1
				if( flip ) {
					StripLights_Pixel(x, 0, t1);
					StripLights_Pixel(x+1, 0, t2);
				} else {
					StripLights_Pixel(x, 0, t2);
					StripLights_Pixel(x+1, 0, t1);
				}
			}

			// toggle flip
			flip = 1 - flip;
			
			// wait and trigger
			while( StripLights_Ready() == 0);
			StripLights_Trigger(1);
			
			
			// delay between transitions 
			CyDelay( 120 );        
			
			if(SW1_Read() == 0) break;  // If SW1 pressed leave
			//if(TimeOut()) break;
		}
	} 
	ticTocCounter  = 0;
}
void SingleLEDPingPong( uint16_t count , uint8 fade_amount, uint32 color) 
{
	int i,x;
	
	for( i=0; i < count ; i++ )
	{   
		for(x = StripLights_MIN_X; x <= StripLights_MAX_X; x++)
		{
			if(fade_amount ) {
				// Fade  strip	
				FadeStrip( StripLights_MIN_X, StripLights_MAX_X , fade_amount );
			} else { 
				StripLights_MemClear(0);
			}
		
			
			StripLights_Pixel(x, 0, color);
			
			while( StripLights_Ready() == 0);
			StripLights_Trigger(1);
			CyDelay( 5 );        
			 if(SW1_Read() == 0) break;  // If SW1 pressed leave
			 //if(TimeOut()) break;
		}
		
		for(x = StripLights_MIN_X; x <= StripLights_MAX_X; x++)
		{
			if(fade_amount ) {
				// Fade  strip	
				FadeStrip( StripLights_MIN_X, StripLights_MAX_X , fade_amount );
			} else { 
				StripLights_MemClear(0);
			}
			
			StripLights_Pixel(StripLights_MAX_X-x, 0, color);
			
			while( StripLights_Ready() == 0);
			StripLights_Trigger(1);
			
			CyDelay( 5 );        
			
			if(SW1_Read() == 0) break;  // If SW1 pressed leave
		   // if(TimeOut()) break;
		}
	}
	
	ticTocCounter  = 0;
}

// snake tail chaser
void Snake( uint16_t count )
{
	int i,x;        
	uint32 startColor;
	 
	count = count ;
	
	startColor = StripLights_RED;

	for(x = StripLights_MIN_X+1; x <= StripLights_MAX_X; x++)
	{            
		
		if( x & 6)
			for(i = StripLights_MIN_X; i <= StripLights_MAX_X; i++)
			{            
				uint32_t colour = StripLights_GetPixel(i, 0);
				StripLights_Pixel(i, 0, colour/2);
			}        
		
		StripLights_Pixel(x, 0, startColor);
		
		while( StripLights_Ready() == 0);
		StripLights_Trigger(1);
		CyDelay( 15 );        

		if( x % 10 == 5 ) startColor+=0x010101;
		
		if(SW1_Read() == 0) break;  // If SW1 pressed leave
		//if(TimeOut()) break;
	}
	ticTocCounter  = 0;
}

void Twinkle( uint16_t count ) 
{
		int i,x;
		led_color col;
		uint32 startColor;
		startColor = StripLights_WHITE;
			
		
		for(x = 0; x <= count; x++)
		{            

			col.c.r = rand();
			col.c.g = rand();
			col.c.b = rand();

			startColor = col.rgb;
					   
			StripLights_Pixel(rand()%StripLights_MAX_X, 0, startColor);
			
			for(i = StripLights_MIN_X; i <= StripLights_MAX_X; i++)
			{            
				col.rgb = StripLights_GetPixel(i, 0);
				
				if ( col.c.r > 0 ) col.c.r -= col.c.r/2; 
				if ( col.c.g > 0 ) col.c.g -= col.c.g/2; 
				if ( col.c.b > 0 ) col.c.b -= col.c.b/2; 
				
				StripLights_Pixel(i, 0, col.rgb );
			}        
	  
			while( StripLights_Ready() == 0);
			StripLights_Trigger(1);
			CyDelay( 15 );        
			
			if(SW1_Read() == 0) break;  // If SW1 pressed leave
			//if(TimeOut()) break;
		}
		
		ticTocCounter  = 0;
}

void FadeStrip(uint16 start, uint16 length, int percentage)
{
	led_color trgb,black_color;
	int i;

	black_color.rgb = StripLights_BLACK;

	for(i = start; i <= start+length; i++) {
	
		// get pixel
		trgb.rgb = StripLights_GetPixel(i,0);
		
		trgb.rgb = TweenC1toC2( trgb,black_color, percentage ) ;

		StripLights_Pixel( i, 0, trgb.rgb );
	}
}

//inline uint8 calculate_sparkle( register uint8 i );
//shortcut for generate a sparkle
extern inline uint8 calculate_sparkle( register uint8 i ) 
{
	register uint8 rnd;

	// every so often return very dark, to make it sparkle
	if ( rand() % 20 == 10 ) {
		return 3;
	}
	
	// pick value from hole range
	rnd = ( rand() % 255 );

	// scale down by level
	rnd /= ( i + 1 ) ;
	
	// scale down again if near end
	if ( i > 4 ) rnd /= 2;
	if ( i > 6 ) rnd /= 2;
	
	return rnd;
}

void Icicle (uint8 redraw, uint8 length, int fade_amount )           
 {
	int x,j,i;
	led_color temp;

	// for entire length of strip, plus engough to move it off the display)
	for(x = StripLights_MIN_X; x <= StripLights_MAX_X + ( length * 2 ); x++)
	{
		
		if(fade_amount ) {
			// Fade  strip	
			FadeStrip( StripLights_MIN_X, StripLights_MAX_X , fade_amount );
		} else { 
			StripLights_MemClear(0);
		}
		
		
		// draw in same place 8 times
		for ( j = 0 ; j < redraw ;j++ ){
			
			// length of icicle
			for(i=0; i < length; i++)
			{
				// caculate a randow twink based on current position in length
				temp.c.r =
				temp.c.g =
				temp.c.b = calculate_sparkle(i);

				// draw a pixel at x+i
				StripLights_Pixel(x+i, 0, temp.rgb );
				
				CyDelay( 1 );  
			}    
				
			// strip ready?
			while( StripLights_Ready() == 0);
			
			//push current data to led strip
			StripLights_Trigger(1);
			CyDelay( 3 );        
		}
		
		if(SW1_Read() == 0) break;  // If SW1 pressed leave
		//if(TimeOut()) break;
	}
	
	ticTocCounter  = 0;
}



void Sparkler ( uint16 runtime, int fade_amount , int num_sparkles ,char white ) 
{
	int x,j;
	led_color temp;

	// length of time to run
	for(x = 0; x <= runtime ; x++)
	{
		if(fade_amount ) {
			// Fade  strip	
			FadeStrip( StripLights_MIN_X, StripLights_MAX_X , fade_amount );
		} else { 
			StripLights_MemClear(0);
		}
		 
		
		// draw in same place 8 times
		for ( j = 0 ; j < num_sparkles ;j++ ){
						
			temp.c.r = calculate_sparkle( j );
			
			if (white ) { 
				temp.c.g = temp.c.b = temp.c.r;
			} else {
				temp.c.g = calculate_sparkle( j );
				temp.c.b = calculate_sparkle( j );
			}
				
			// draw a pixel 
			StripLights_Pixel(rand()%StripLights_MAX_X, 0, temp.rgb );
		}
		
		// strip ready?
		while( StripLights_Ready() == 0);
		
		//push current data to led strip
		StripLights_Trigger(1);
		CyDelay( 3 );     
	}
	
	if( fade_amount ) {
		// fade at end
		for(x = 0; x <= 200 ; x++)
		{
			// Fade  strip	
			FadeStrip( StripLights_MIN_X, StripLights_MAX_X , fade_amount );

			// strip ready?
			while( StripLights_Ready() == 0);

			//push current data to led strip
			StripLights_Trigger(1);
			CyDelay( 3 );     
		}
	}		
	ticTocCounter  = 0;
}

void ColorWheel( uint16_t count )
{
	static int i =0xAAA ,x;
	static uint32 color;
	static uint32 startColor ;   

	
	if (i >= count ) {
		i = 0;
	}
	
	if ( i == 0xaaa ) {
		i = 0;
		color = 0;
		startColor = 0;
	}
	
	for( ; i < count ; i++ )
	{   
		color = startColor;
		for(x = StripLights_MIN_X; x <= StripLights_MAX_X; x++)
		{
			StripLights_Pixel(x, 0, StripLights_getColor( color ));

			 color++;
			
			if(color >= StripLights_COLOR_WHEEL_SIZE) color = 0;
		}
		
		startColor++;
	
		if(startColor >= StripLights_COLOR_WHEEL_SIZE) startColor = 0;
	
	   while( StripLights_Ready() == 0);
	
		StripLights_Trigger(1);
	
	   CyDelay( 50 );

		if(SW1_Read() == 0) break;  // If SW1 pressed leave
		//if(TimeOut()) break;
	}
	ticTocCounter  = 0;
}

void Tweener( uint16_t count, uint32 from) 
{
	int i,newx;
	led_color src, target;

	src.rgb  = from;

	newx = 0;
	
	for( i = 0 ; i < count; i ++ )
	{
	   //StripLights_MemClear( 0 );
			
		target.c.r += 5-(rand()%10);
		target.c.g += 5-(rand()%10);
		target.c.b += 5-(rand()%10);

		newx = TweenC1toC2Range(StripLights_COLUMNS/2,newx,src.rgb,target.rgb);
		newx = TweenC1toC2Range(StripLights_COLUMNS/2,newx,target.rgb,src.rgb);
		
	   while( StripLights_Ready() == 0); 
	   StripLights_Trigger(1); CyDelay( 50 );
	
	}
	
	ticTocCounter  = 0;
	
}


void FadeToColor( uint16_t startx, uint16_t count, uint32 target, uint32 delay, int direction)
{
	int j,i;
	int offset,oldoffset;

	led_color frgb,trgb;
	hsv_color  src,target_hsv,result;

	frgb.rgb = target;

	src = rgb_to_hsv( frgb ) ;

	offset = startx;

	for ( j = 0 ; j < 100 ; j ++ ) {
		
		oldoffset = offset;		
		
		for( i = 0 ; i < count; i ++ ){
			
			// get colour of current LED at offset
			trgb.rgb = StripLights_GetPixel(offset,0);

			// convert current led color to hsv
			target_hsv = rgb_to_hsv( trgb );
			
			result = target_hsv;
			// tween, what we want to  what it is at percentage i
			result.h.h = TweenU8toU8( target_hsv.h.h, src.h.h, j );
			
			// update pixel
			StripLights_PixelHSV(offset, 0, result );
			
			// handle travel direction of pixel
			offset += direction;
			if (offset < (int)StripLights_MIN_X ) offset = startx ;
			if (offset > startx+count ) offset = StripLights_MIN_X ;
			
		}
		
		// check bootloader mode
		if(SW1_Read() == 0) break;  // If SW1 pressed leave
		//if(TimeOut()) break;   
		
		// if wants a delay, update led strip and delay
		if(delay) {
			while( StripLights_Ready() == 0);

			StripLights_Trigger(1);
			CyDelay( delay );
		}
			
		offset = oldoffset;		

	}	
}

uint32 TweenerHSV( uint16_t startx, uint16_t count, uint32 from,uint32 to,uint32 delay,int direction) 
{
	int i;
	int offset;
	led_color frgb,trgb;
	hsv_color  src, target,result;

	trgb.rgb = to;
	frgb.rgb = from;
	
	src = rgb_to_hsv( frgb ) ;
	target = rgb_to_hsv( trgb );

	result = src;
	
	offset = startx;
	
	for( i = 1 ; i < count; i ++ )
	{
		result.h.h = TweenU8toU8(src.h.h, target.h.h, i);
		
		StripLights_PixelHSV(offset, 0, result );
		
		offset += direction;
		
		if (offset < (int)StripLights_MIN_X ) offset = startx ;
		if (offset > startx+count ) offset = StripLights_MIN_X ;
		
		if(SW1_Read() == 0) break;  // If SW1 pressed leave
		//if(TimeOut()) break;    
		
		if(delay) {
			while( StripLights_Ready() == 0);

			StripLights_Trigger(1);
			CyDelay( delay );
		}
		
	}
			
   while( StripLights_Ready() == 0);

	StripLights_Trigger(1);

   CyDelay( 5 );


	return result.hsv;
}

uint32 TweenerALLHSV( uint16_t count, uint32 from,uint32 to,uint32 delay) 
{
	int i;
	
	led_color frgb,trgb;
	hsv_color  src, target,result;

	trgb.rgb = to;
	frgb.rgb = from;
	
	src = rgb_to_hsv( frgb ) ;
	target = rgb_to_hsv( trgb );

	result = src;
	
	for( i = 1 ; i < count; i ++ )
	{
		result.h.h = TweenU8toU8(src.h.h, target.h.h, i);
		
		StripLights_DisplayClearHSV( result );
		
		if(delay){
			CyDelay( delay );
		}
		
		if(SW1_Read() == 0) break;  // If SW1 pressed leave
		//if(TimeOut()) break;   
	}
	return result.hsv;
}

// quick helper function for testing hsv/rgb.
void StripLights_PixelHSV(int32 x,int32 y,hsv_color hsv )
{
	led_color rgb;
	
	rgb = hsv_to_rgb( hsv ) ;
	
	StripLights_Pixel( x,y,rgb.rgb);
}
// quick helper function for testing hsv/rgb.
void StripLights_DisplayClearHSV(hsv_color hsv )
{
	led_color rgb;
	
	rgb = hsv_to_rgb( hsv ) ;
	
	StripLights_DisplayClear( rgb.rgb);
}





/* [] END OF FILE */
