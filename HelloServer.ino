//#include <Task.h>
#include "FS.h"
#include <NeoPixelBrightnessBus.h>
#include <NeoPixelAnimator.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266HTTPUpdateServer.h>
#include <NeoPixelPainter.h>
//#include <Scheduler.h>
#include <NeoPixelBus.h>
#include <NeoPixelBusHelper.h>
#include "effects.h"
#include "StripLights.h"



#include <TimeLib.h>
#include <NtpClientLib.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Ticker.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include "FSWebServerLib.h"
#include <Hash.h>
#include "Ticker.h"
#include <ESP8266HTTPClient.h>

//#include <PubSubClient.h>
//#include <aREST.h>



#define NUMBEROFPIXELS 300//Number of LEDs on the strip
#define PIXELPIN 2 //Pin where WS281X pixels are connected
#define PIN 2
const int duration = 2000; //number of loops to run each animation for

extern int32 ticTocCounter;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMBEROFPIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);


// tutaj ustawione dwie taśmy, na port szeregowy (wejście użyte jako wyjście) i pin GPIO2 
// tylko te dwa bo tam jest obsługa dma
NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(NUMBEROFPIXELS);
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip2(NUMBEROFPIXELS);



NeoPixelPainterCanvas pixelcanvas = NeoPixelPainterCanvas(&strip); //create canvas, linked to the neopixels (must be created before the brush)
NeoPixelPainterBrush pixelbrush = NeoPixelPainterBrush(&pixelcanvas); //crete brush, linked to the canvas to paint to


NeoPixelPainterCanvas pixelcanvas2 = NeoPixelPainterCanvas(&strip2); //create canvas, linked to the neopixels (must be created before the brush)
NeoPixelPainterBrush pixelbrush2 = NeoPixelPainterBrush(&pixelcanvas2); //crete brush, linked to the canvas to paint to


AsyncFSWebServer server(80);
// zdalna aktulalizacja jest robiona przez server
//ESP8266HTTPUpdateServer httpUpdater;

// obsługa aRest
//WiFiClient espClient;
//PubSubClient client(espClient);

// Create aREST instance
//aREST rest = aREST(client);

// Unique ID to identify the device for cloud.arest.io
//char* device_id = "2pbV6q";

// Functions
//void callback(char* topic, byte* payload, unsigned int length);



const char* ssid = "wlan-g";
const char* password = "szafa1000";
const char* host = "ledstrip";

/////////////////////////////////////////////////////////////////////////////////////////////////
//  POKAZY RÓŻNE
/////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_MODE 75


void rainbow(uint8_t wait);
bool perform(uint8_t wait);
bool Rainbow_Paint();
void SPARKLER(unsigned long par);
void TWINKLY_STARS();
void CHASER();
void HUE_FADER();
void SPEEDTRAILS();
void BOUNCY_BALLS();
void TWO_BRUSH_COLORIMAX();


// koniec pokazow
///////////////////////////////////////////////////////////////////////////////////////////////////


void pokaz ();

#define IDLE 0
#define PRESENTATION 1

// mode- co ma zaraz zrobic, wybierany przez program przy sterowaniu automatycznym lub ręcznie przez stronę www,
// state - aktualny stan

// możliwe stany mode i state


uint8_t mode,state, zmiana_mode;
unsigned long mode_par1, mode_par2, mode_par3 ;
String tmp_string;
Ticker ticker;

//const int led = 13;

float t = 10.3;
float h = 23.6;
float p = 56.9;

led_color rand_color1, rand_color2;

// funkcja wywolywana w kazdym wolnym czasie
// ogolnie w loop oraz wewnatrz czekania w czasie pokazu
void LoopFunction();



////////////////////////////////////////////////////////////////////////////////
//  przystosowywanie CYPRESSA do ARDUINO
/*


void   StripLights_Start(void);
void   StripLights_Stop(void);
void   StripLights_WriteColor(uint32 color);
void   StripLights_DisplayClear(uint32 color);
void   StripLights_MemClear(uint32 color);
void   StripLights_Trigger(uint32 rst);
uint32 StripLights_Ready(void);



void   StripLights_DrawRect(int32 x0, int32 y0, int32 x1, int32 y1, int32 fill, uint32 color);
void   StripLights_DrawLine(int32 x0, int32 y0, int32 x1, int32 y1, uint32 color);
void   StripLights_DrawCircle(int32 x0, int32 y0, int32 radius, uint32 color);
void   StripLights_Pixel(int32 x, int32 y, uint32 color);
uint32 StripLights_GetPixel(int32 x, int32 y);
uint32 StripLights_ColorInc(uint32 incValue);
void   StripLights_Dim(uint32 dimLevel);
*/



void   StripLights_Pixel(int32 x, int32 y, uint32 color)
{
	if (!y)
		strip.SetPixelColor(x,RgbColor(color));
	else
		strip2.SetPixelColor(x, RgbColor(color));


}


void   StripLights_Trigger(uint32 rst)
{
	
	strip.Show();
	strip2.Show();
	
}

bool SW1_Read(void)
{
	return 1;
}

bool timeout_global = false;;

void CyDelay(uint32_t delay)
{

	timeout_global |= smart_delay(delay);
		 

	
}


bool TimeOut(void)
{

	return timeout_global;

}

uint32 StripLights_Ready(void)
{
	return true;
}


void   StripLights_MemClear(uint32 color)
{
	
	RgbColor tmp_color = RgbColor(color);

	strip.ClearTo(tmp_color);
	strip2.ClearTo(tmp_color);

}


void   StripLights_DisplayClear(uint32 color)
{

	

	StripLights_MemClear(color);
	



}



uint32 StripLights_GetPixel(int32 x, int32 y)
{

	RgbColor wynik;

	if (!y)
	{
		wynik = strip.GetPixelColor(x);
	}
	else
	{
		wynik =  strip2.GetPixelColor(x);
	}

	return  RgbColor::ColorUint(wynik.R, wynik.G, wynik.B);

}


/*Function Name : StripLights_DrawLine
********************************************************************************
*
* Summary :
*Draw a line on the display.
*
* Parameters :
*x0, y0 : The beginning endpoint
*  x1, y1 : The end endpoint.
*  color : Color of the line.
*  działa dla dwóch pasków
* Return :
*None
*
******************************************************************************* */

void StripLights_DrawLine(int32 x0, int32 y0, int32 x1, int32 y1, uint32 color)
{
	int32 dy = y1 - y0; /* Difference between y0 and y1 */
	int32 dx = x1 - x0; /* Difference between x0 and x1 */
	int32 stepx, stepy;

	if (dy < 0)
	{
		dy = -dy;
		stepy = -1;
	}
	else
	{
		stepy = 1;
	}

	if (dx < 0)
	{
		dx = -dx;
		stepx = -1;
	}
	else
	{
		stepx = 1;
	}

	dy <<= 1; /* dy is now 2*dy  */
	dx <<= 1; /* dx is now 2*dx  */
	StripLights_Pixel(x0, y0, color);

	if (dx > dy)
	{
		int fraction = dy - (dx >> 1);
		while (x0 != x1)
		{
			if (fraction >= 0)
			{
				y0 += stepy;
				fraction -= dx;
			}
			x0 += stepx;
			fraction += dy;
			StripLights_Pixel(x0, y0, color);
		}
	}
	else
	{
		int fraction = dx - (dy >> 1);
		while (y0 != y1)
		{
			if (fraction >= 0)
			{
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;
			StripLights_Pixel(x0, y0, color);
		}
	}
}



// koniec przystosowywania 
//////////////////////////////////////////////////////////////////////////////////////////////

/*
void handleRoot() {
 
	//server.send(200, "text/html", getPage());// "uzywaj:\n/rainbow");
  
}
*/

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
RgbColor Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
	  return RgbColor(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
	WheelPos -= 85;
	return RgbColor(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return RgbColor(WheelPos * 3, 255 - WheelPos * 3, 0);
}




//void handleNotFound(){
  //digitalWrite(led, 1);
	/*
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
	message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);

  */

  //digitalWrite(led, 0);
//}


// funkcja odmierza czas i sprawdza sieć co z niej przyszło 
// jeżeli last_time_mils 0 oznacza pierwsze wywołanie i wtedy ignorujemy tą wartość
// i tylko odmierzamy czas
bool smart_delay(unsigned long delay_mils) //, unsigned long last_time_mils)
{
	bool przekrecenie;
	bool status;
	
	// określa czy zmienił się i mode a to ma powodować zakończenie bierzącj funkcji
	status = false;

	

	// wewnątrz funkcji operuje na mikrosekundach
	unsigned long old_micros = micros();
	unsigned long last_time_mils = millis();
	
	// sprawdzamy czy nie zawinię się cas mils czyli 50 dni pracy systemu
	//if (last_time_mils)
	/*if (last_time_mils + delay_mils < millis())
		przekrecenie = true;
	else
		przekrecenie = false;
		*/
	do
	{
		LoopFunction();

		if (zmiana_mode)
		{
			status = true;
			zmiana_mode = false;
			Serial.println("Nastapila zmian");
		}

		yield();

	}   while (!status &&
			// normalny warunek czasowy
			millis()<last_time_mils+delay_mils);
	


	return status;


}




void rainbow(uint8_t wait) 
{
	static uint16_t i, j, p;
	static bool initialize;
	static unsigned long time_perf;

	if (!initialize)
	{
		i = 0;
		p = 0;
		j = 0;
		initialize = true;
		time_perf = 0;

	}



//	 for(p=0; p<256; p++) 
//		  for(j=0; j<256; j++) {
//			  for (i = 0; i<strip.PixelCount(); i++) {
			  //strip.SetPixelColor(i, Wheel((i+j) & 255));
//			}
			//strip.Show();
			//delay(wait);
	  //}



	if (time_perf + wait < millis())
	{

		for (i = 0; i < strip.PixelCount(); i++) {
			strip.SetPixelColor(i, Wheel(i + j));
			strip2.SetPixelColor(i, Wheel(i + j));
		}





		strip.Show();
		strip2.Show();

		


		time_perf = millis();
		j++;
		if (j == 256)
		{
			j = 0;
			p++;
			if (p == 256)
			{
				initialize = false;
			}
			
		}
	}



}

bool perform(uint8_t wait)
{

	 static uint16_t j, p;
	 static uint8_t initialize = false;
	 static unsigned long time_perf;
	 uint8_t zero = 0;


	 if (!initialize)
	 {
		 p = 0;
		 j = 0;
		 initialize = true;
		 time_perf = 0;

	 }
 
	
	if (time_perf + wait <= millis())
	{
		strip.ClearTo(RgbColor(zero));
		strip2.ClearTo(RgbColor(zero));


		strip.SetPixelColor(j, RgbColor(100, 100, 100));
		strip2.SetPixelColor(strip.PixelCount() - j, RgbColor(100, 100, 100));


		strip.Show();
		strip2.Show();



		time_perf = millis();
		j++;
		if (j == strip.PixelCount())
		{
			j = 0;
			initialize = false;
		}
	}
	  
	return initialize;
}




unsigned long loopcounter; //count the loops, switch to next animation after a while
bool initialized = false; //initialize the canvas & brushes in each loop when zero


bool Rainbow_Paint()
{
	uint8_t zero = 0;
	//initialized = false; //reset the variable before moving to the next loop
	//---------------------
	//RAINBOW PAINT (aka nyan cat)
	//---------------------
	//Serial.println("Pierwszy");
	//the brush moves along the strip, leaving a colorful rainbow trail
	//for (loopcounter = 0; loopcounter<duration; loopcounter++)
	{
		static unsigned int hue = 0; //color hue to set to brush
		HSV brushcolor; //HSV color definition

		//Serial.print(loopcounter);
		//Serial.print(" ");
			
		if (initialized == false) //initialize the brushes
		{
			hue = 0;
			initialized = true;
			pixelbrush.setSpeed(2500); //brush moving speed 
			pixelbrush.setFadeSpeed(250);
			pixelbrush.setFadein(false); //brightness will fade-in if set to true
			pixelbrush.setFadeout(true);
			pixelbrush.setBounce(false);
			loopcounter = 0;

			pixelbrush2.setSpeed(-3095); //brush moving speed 
			pixelbrush2.setFadeSpeed(250);
			pixelbrush2.setFadein(false); //brightness will fade-in if set to true
			pixelbrush2.setFadeout(true);
			pixelbrush2.setBounce(false);


		}

		hue++;
		brushcolor.h = hue / 6; //divide by 3 to slow down color fading
		brushcolor.s = 255; //full saturation
		brushcolor.v = 255; //full brightness

		pixelbrush.setColor(brushcolor); //set new color to the bursh
		pixelbrush2.setColor(brushcolor);

		strip2.ClearTo(RgbColor(zero));
		strip.ClearTo(RgbColor(zero));

		pixelbrush.paint(); //paint the brush to the canvas (and update the brush, i.e. move it a little)
		pixelcanvas.transfer(); //transfer (add) the canvas to the neopixels

		pixelbrush2.paint(); //paint the brush to the canvas (and update the brush, i.e. move it a little)
		pixelcanvas2.transfer();

		strip.Show();
		strip2.Show();
		//delay(1);
		//yield();

		loopcounter++;

		if (loopcounter == 5000)
			initialized = false;

		return initialized;

	}


}

void SPARKLER(unsigned long par)
{


	initialized = false; //reset the variable before moving to the next loop
	Serial.println("Drugi");
	uint8_t zero = 0;
	

	//SPARKLER: a brush seeding sparkles
	while (1)
	{

		HSV brushcolor;
		//Serial.print(loopcounter);
		// Serial.print(" ");

		if (initialized == false)
		{
			initialized = true;
			pixelbrush.setSpeed(par);
			pixelbrush.setFadeout(true); //sparkles fade in
			pixelbrush.setFadein(true);  //and fade out immediately after reaching the set brightness
			
			pixelbrush2.setSpeed(-par);
			pixelbrush2.setFadeout(true); //sparkles fade in
			pixelbrush2.setFadein(true);  //and fade out immediately after reaching the set brightness
		}

		//set a new brush color in each loop
		brushcolor.h = random(255); //random color
		brushcolor.s = random(130); //random but low saturation, giving white-ish sparkles
		brushcolor.v = random(200); //random (peak) brighness

		pixelbrush.setColor(brushcolor);
		pixelbrush.setFadeSpeed(random(100) + 150); //set a new fadespeed with some randomness

		pixelbrush2.setColor(brushcolor);
		pixelbrush2.setFadeSpeed(random(100) + 150);

		strip.ClearTo(RgbColor(zero));
		strip2.ClearTo(RgbColor(zero));


		pixelbrush.paint(); //paint the brush to the canvas (and update the brush, i.e. move it a little)
		pixelcanvas.transfer(); //transfer (add) the canvas to the neopixels

		pixelbrush2.paint(); //paint the brush to the canvas (and update the brush, i.e. move it a little)
		pixelcanvas2.transfer(); //transfer (add) the canvas to the neopixels


		if(smart_delay(2))
			return;
		strip.Show();
		strip2.Show();

		
	}



}


void TWINKLY_STARS()
{

	initialized = false; //reset the variable before moving to the next loop
	//---------------------
	//TWINKLY STARS
	//---------------------
	//brush set to random positions and painting a fading star

	Serial.println("Trzeci");
	while (1)
	{

		HSV brushcolor;

		if (initialized == false)
		{
			initialized = true;
			pixelbrush.setSpeed(0); //do not move automatically
			pixelbrush.setFadein(true); //fade in 
			pixelbrush.setFadeout(true); //and fade out
		}


		if (rand() % 100 == 0) //at a random interval, move the brush to paint a new pixel (brush only paints a new pixel once)
		{
			brushcolor.h = rand();
			brushcolor.s = random(40); //set low saturation, almost white
			brushcolor.v = random(200) + 20; //set random brightness
			pixelbrush.setColor(brushcolor);
			pixelbrush.moveTo(random(NUMBEROFPIXELS)); //move the brush to a new, random pixel
			pixelbrush.setFadeSpeed(random(10) + 5); //set random fade speed, minimum of 5
		}

		//add a background color by setting all pixels to a color (instead of clearing all pixels):
		int i;
		for (i = 0; i < NUMBEROFPIXELS; i++)
		{
			strip.SetPixelColor(i, RgbColor(1, 0, 6)); //color in RGB: dark blue
		}


		pixelbrush.paint(); //paint the brush to the canvas 
		pixelcanvas.transfer(); //transfer (add) the canvas to the neopixels
		
		
		if (smart_delay(3))
			return;
		strip.Show();
		
	}


}


void CHASER()
{

	initialized = false; //reset the variable before moving to the next loop
	uint8_t zero = 0;


	//-------------
	//CHASER
	//-------------

	// two brushes chasing each other, one painting the pixel in a color, the other one painting 'black' (acting on the same canvas)

	while (true) //create a loop with an additional brush (is deleted automatically once the loop finishes)
	{

		//create an additional brush, painting on the same canvas as the globally defined brush
		NeoPixelPainterBrush pixelbrush2 = NeoPixelPainterBrush(&pixelcanvas);

//		if (pixelbrush2.isvalid() == false) Serial.println(F("brush2 allocation problem"));
//		else  Serial.println(F("brush2 allocation ok"));


		for (loopcounter = 0; loopcounter<duration; loopcounter++)
		{


			if (initialized == false)
			{
				HSV brushcolor;

				initialized = true;

				brushcolor.h = random(255); //choose random color once
				brushcolor.s = 255; //full staturation
				brushcolor.v = 150;

				//initialize the first brush to move and paint a color, no fading 
				pixelbrush.setSpeed(900); //moving speed
				pixelbrush.setColor(brushcolor);
				pixelbrush.setFadeout(false); //deactivate fade-out (was activated in last animation)
				pixelbrush.setFadein(false); //deactivate fade-in 
				pixelbrush2.moveTo(0); //move the brush to pixel 0
				//initialize the second brush to move at the same speed but starting at a different position (default position is 0)
				brushcolor.v = 0; //zero intensity = black
				pixelbrush2.setSpeed(900);
				pixelbrush2.setColor(brushcolor);
				pixelbrush2.moveTo(2 * NUMBEROFPIXELS / 3); //move the brush 

			}

			strip.ClearTo(RgbColor(zero));

			pixelbrush.paint(); //apply the paint of the first brush to the canvas (and update the brush)
			pixelbrush2.paint(); //apply the paint of the second brush to the canvas (and update the brush)
			pixelcanvas.transfer(); //transfer the canvas to the neopixels

			if (smart_delay(3))
				return;
			strip.Show();
			
		}
		break; //quit the while loop immediately (and delete the created brush)
	}


}

void HUE_FADER()
{


	initialized = false;
	uint8_t zero = 0;




	//------------------------------
	//HUE FADER: demo of hue fading
	//------------------------------

	//hue fading can be done in two ways: change the color moving the shortest distance around the colorwheel (setFadeHueNear)
	//or intentionally moving around the colorwheel choosing the long way (setFadeHueFar)
	//two brushes move along the strip in different speeds, each painting a different color that the canvas will then fade to
	//a new color is set when the first brush passes pixel 0
	//both brushes act on the same canvas

	while (true) //create a loop with an additional brush (is deleted automatically once the loop finishes)
	{

		//create an additional brush, painting on the same canvas as the globally defined brush
		NeoPixelPainterBrush pixelbrush2 = NeoPixelPainterBrush(&pixelcanvas);

//		if (pixelbrush2.isvalid() == false) Serial.println(F("brush2 allocation problem"));
//		else  Serial.println(F("brush2 allocation ok"));

		pixelcanvas.clear(); //clear the canvas

		for (loopcounter = 0; loopcounter<duration; loopcounter++)
		{

			static unsigned int lastposition = 0; //to detect zero crossing only once (brush may stay at pixel zero for some time since it uses sub-pixel resolution movement)

			if (pixelbrush.getPosition() == 0 && lastposition > 0) initialized = false; //choose new color & speed if brush reaches pixel 0

			lastposition = pixelbrush.getPosition(); //save current position for next position check

			if (initialized == false)
			{
				initialized = true;

				HSV brushcolor;


				brushcolor.h = random(255); //random color
				brushcolor.s = 255; //full saturation
				brushcolor.v = 130; //medium brightness

				pixelbrush.setSpeed(random(150) + 150); //random movement speed
				pixelbrush.setFadeSpeed(random(10) + 20); //set random fading speed
				pixelbrush.setColor(brushcolor); //update the color of the brush
				pixelbrush.setFadeHueNear(true); //fade using the near path on the colorcircle

				//second brush paints on the same canvas
				brushcolor.h = random(255);
				pixelbrush2.setSpeed(random(150) + 150);
				pixelbrush2.setFadeSpeed(random(10) + 20);
				pixelbrush2.setColor(brushcolor);
				pixelbrush2.setFadeHueNear(true); //fade using the near path on the colorcircle
				//pixelbrush.setFadeHueFar(true); //fade using the far path on the colorcircle (if both are set, this path is chosen)
				pixelbrush2.setBounce(true); //bounce this brush at the end of the strip
			}

			strip.ClearTo(RgbColor(zero));

			pixelbrush.paint(); //apply the paint of the first brush to the canvas (and update the brush)
			pixelbrush2.paint(); //apply the paint of the second brush to the canvas (and update the brush)
			pixelcanvas.transfer(); //transfer the canvas to the neopixels

			if (smart_delay(3))
				return;
			strip.Show();
			
		}
		break; //quit the while loop immediately (and delete the created brush)
	}

}

void SPEEDTRAILS()
{
	uint8_t zero = 0;

	initialized = false;

	//------------------------------
	//SPEEDTRAILS
	//------------------------------
	//three brushes painting on one canvas, all following each other at the same speed, painting fading pixels

	while (true) //create a loop with two additional brushes (are deleted automatically once the loop finishes)
	{
		int brushspeed = 900;

		//create additional brushes, painting on the same canvas as the globally defined brush
		NeoPixelPainterBrush pixelbrush2 = NeoPixelPainterBrush(&pixelcanvas);
		NeoPixelPainterBrush pixelbrush3 = NeoPixelPainterBrush(&pixelcanvas);

//		if (pixelbrush2.isvalid() == false) Serial.println(F("brush2 allocation problem"));
//		else  Serial.println(F("brush2 allocation ok"));

//		if (pixelbrush3.isvalid() == false) Serial.println(F("brush3 allocation problem"));
//		else  Serial.println(F("brush3 allocation ok"));

		for (loopcounter = 0; loopcounter<duration; loopcounter++)
		{

			HSV brushcolor;


			if (initialized == false) //initialize the brushes
			{
				initialized = true;
				brushcolor.h = 0;
				brushcolor.s = 0; //make it white
				brushcolor.v = 150; //medium brightness
				pixelbrush.setColor(brushcolor);
				pixelbrush.setSpeed(brushspeed);
				pixelbrush.setFadeSpeed(250); //fast fading (255 is max.)
				pixelbrush.setFadeHueNear(false); //deactivate hue fading, was activated in last animation
				pixelbrush.setFadeout(true);
				pixelbrush.moveTo(0); //move brush to zero

				//second brush
				brushcolor.h = 0; //red
				brushcolor.s = 250;
				brushcolor.v = 150;
				pixelbrush2.setSpeed(brushspeed);
				pixelbrush2.setFadeSpeed(220);
				pixelbrush2.setFadeout(true);
				pixelbrush2.setColor(brushcolor);
				pixelbrush2.moveTo(NUMBEROFPIXELS / 3); //move it up one third of the strip

				//third brush
				brushcolor.h = 28; //yellow
				brushcolor.s = 255;
				brushcolor.v = 100;
				pixelbrush3.setSpeed(brushspeed);
				pixelbrush3.setFadeSpeed(190);
				pixelbrush3.setFadeout(true);
				pixelbrush3.setColor(brushcolor);
				pixelbrush3.moveTo(2 * NUMBEROFPIXELS / 3); //move it up two thirds of the strip
			}

			strip.ClearTo(RgbColor(zero));

			pixelbrush.paint(); //apply the paint of the first brush to the canvas (and update the brush)
			pixelbrush2.paint(); //apply the paint of the second brush to the canvas (and update the brush)
			pixelbrush3.paint(); //apply the paint of the third brush to the canvas (and update the brush)
			pixelcanvas.transfer(); //transfer the canvas to the neopixels (and update i.e. fade pixels)

			if (smart_delay(3))
				return;
			strip.Show();
			
		}
		break; //quit the while loop immediately (and delete the created brush)
	}

}



void BOUNCY_BALLS()
{

	initialized = false;
	uint8_t zero = 0;




	//-------------
	//BOUNCY BALLS
	//-------------
	//three brushes painting on one canvas, attracted to the zero pixel as if by gravity

	while (true) //create a loop with two additional brushes (are deleted automatically once the loop finishes)
	{

		//create additional brushes, painting on the same canvas as the globally defined brush
		NeoPixelPainterBrush pixelbrush2 = NeoPixelPainterBrush(&pixelcanvas);
		NeoPixelPainterBrush pixelbrush3 = NeoPixelPainterBrush(&pixelcanvas);

		NeoPixelPainterBrush pixelbrush4 = NeoPixelPainterBrush(&pixelcanvas2);
		NeoPixelPainterBrush pixelbrush5 = NeoPixelPainterBrush(&pixelcanvas2);
		NeoPixelPainterBrush pixelbrush6 = NeoPixelPainterBrush(&pixelcanvas2);

/*		if (pixelbrush2.isvalid() == false) Serial.println(F("brush2 allocation problem"));
		else  Serial.println(F("brush2 allocation ok"));

		if (pixelbrush3.isvalid() == false) Serial.println(F("brush3 allocation problem"));
		else  Serial.println(F("brush3 allocation ok"));

		// drugi pasek

		if (pixelbrush4.isvalid() == false) Serial.println(F("brush4 allocation problem"));
		else  Serial.println(F("brush4 allocation ok"));

		if (pixelbrush5.isvalid() == false) Serial.println(F("brush5 allocation problem"));
		else  Serial.println(F("brush5 allocation ok"));

		if (pixelbrush6.isvalid() == false) Serial.println(F("brush6 allocation problem"));
		else  Serial.println(F("brush6 allocation ok"));
*/



		byte skipper = 0;

		while (1)
		{


			if (initialized == false) //initialize the brushes
			{
				initialized = true;

				HSV brushcolor;

				brushcolor.h = 20; //orange
				brushcolor.s = 240; //almost full saturation
				brushcolor.v = 150; //medium brightness

				//first brush
				pixelbrush.setSpeed(0); //zero initial speed
				pixelbrush.setFadeSpeed(150);
				pixelbrush.setFadeout(true);
				pixelbrush.setColor(brushcolor);
				pixelbrush.moveTo(NUMBEROFPIXELS - 1); //move to end of the strip
				pixelbrush.setBounce(true); //bounce if either end of the strip is reached

				//second brush
				brushcolor.h = 220; //pink
				pixelbrush2.setSpeed(0); //zero initial speed
				pixelbrush2.setFadeSpeed(190);
				pixelbrush2.setFadeout(true);
				pixelbrush2.setColor(brushcolor);
				pixelbrush2.moveTo(NUMBEROFPIXELS / 3); //move to one third of the strip
				pixelbrush2.setBounce(true);

				brushcolor.h = 70; //green-ish (pure green is 85 or 255/3)
				pixelbrush3.setSpeed(0);
				pixelbrush3.setFadeSpeed(220);
				pixelbrush3.setFadeout(true);
				pixelbrush3.setColor(brushcolor);
				pixelbrush3.moveTo(2 * NUMBEROFPIXELS / 3);
				pixelbrush3.setBounce(true);

				/////////////////////////////////////////////////////////////////////////////////////////////////////
				// drugi pendzel

				//first brush
				brushcolor.h = 30;
				pixelbrush4.setSpeed(0); //zero initial speed
				pixelbrush4.setFadeSpeed(140);
				pixelbrush4.setFadeout(true);
				pixelbrush4.setColor(brushcolor);
				pixelbrush4.moveTo(NUMBEROFPIXELS - 1); //move to end of the strip
				pixelbrush4.setBounce(true); //bounce if either end of the strip is reached

				//second brush
				brushcolor.h = 180; //pink
				pixelbrush5.setSpeed(0); //zero initial speed
				pixelbrush5.setFadeSpeed(200);
				pixelbrush5.setFadeout(true);
				pixelbrush5.setColor(brushcolor);
				pixelbrush5.moveTo(NUMBEROFPIXELS / 3); //move to one third of the strip
				pixelbrush5.setBounce(true);

				brushcolor.h = 90; //green-ish (pure green is 85 or 255/3)
				pixelbrush6.setSpeed(0);
				pixelbrush6.setFadeSpeed(240);
				pixelbrush6.setFadeout(true);
				pixelbrush6.setColor(brushcolor);
				pixelbrush6.moveTo(2 * NUMBEROFPIXELS / 3);
				pixelbrush6.setBounce(true);
			}

			//apply some gravity force that accelerates the painters (i.e. add speed in negative direction = towards zero pixel)
			//  if (skipper % 5 == 0) //only apply gravity at some interval to make it slower on fast processors
			//  {
			//read current speed of each brush and speed it up in negative direction (towards pixel zero)
			pixelbrush.setSpeed(pixelbrush.getSpeed() - 10);
			pixelbrush2.setSpeed(pixelbrush2.getSpeed() - 10);
			pixelbrush3.setSpeed(pixelbrush3.getSpeed() - 10);


			pixelbrush4.setSpeed(pixelbrush4.getSpeed() - 12);
			pixelbrush5.setSpeed(pixelbrush5.getSpeed() - 9);
			pixelbrush6.setSpeed(pixelbrush6.getSpeed() - 11);

			//  }
			//  skipper++;

			strip2.ClearTo(RgbColor(zero));
			strip.ClearTo(RgbColor(zero));

			pixelbrush.paint(); //apply the paint of the first brush to the canvas (and update the brush)
			pixelbrush2.paint(); //apply the paint of the second brush to the canvas (and update the brush)
			pixelbrush3.paint(); //apply the paint of the third brush to the canvas (and update the brush)
			pixelcanvas.transfer(); //transfer the canvas to the neopixels (and update i.e. fade pixels)

			pixelbrush4.paint(); //apply the paint of the first brush to the canvas (and update the brush)
			pixelbrush5.paint(); //apply the paint of the second brush to the canvas (and update the brush)
			pixelbrush6.paint(); //apply the paint of the third brush to the canvas (and update the brush)
			pixelcanvas2.transfer(); //transfer the canvas to the neopixels (and update i.e. fade pixels)



			if (smart_delay(2))
				return;
			strip.Show();
			strip2.Show();

			
		}
		break; //quit the while loop immediately (and delete the created brush)
	}

}

void TWO_BRUSH_COLORIMAX()
{
	uint8_t zero = 0;

	initialized = false;

	//---------------------
	//TWO-BRUSH-COLORMIXING
	//---------------------

	//two brushes moving around randomly paint on their individual canvas, resulting in colors being mixed 
	while (true) //create a loop with two additional brushes (are deleted automatically once the loop finishes)
	{

		//create an additional canvas for the second brush (needs to be created before the brush)
		NeoPixelPainterCanvas pixelcanvas2 = NeoPixelPainterCanvas(&strip);

		//create additional brush, painting on the second canvas 
		NeoPixelPainterBrush pixelbrush2 = NeoPixelPainterBrush(&pixelcanvas2);

//		if (pixelcanvas2.isvalid() == false) Serial.println("canvas2 allocation problem");
//		else  Serial.println("canvas2 allocation ok");

//		if (pixelbrush2.isvalid() == false) Serial.println(F("brush2 allocation problem"));
//		else  Serial.println(F("brush2 allocation ok"));


		for (loopcounter = 0; loopcounter<duration; loopcounter++)
		{


			HSV brushcolor;
			static bool firstrun = true;

			brushcolor.s = 255; //full color saturation
			brushcolor.v = 100; //medium-low brightness

			if (initialized == false) //initialize the brushes
			{
				initialized = true;

				brushcolor.h = 8;

				//setup the first brush
				pixelbrush.setSpeed(-750);
				pixelbrush.setSpeedlimit(1000);
				pixelbrush.setFadeSpeed(random(80) + 50);
				pixelbrush.setFadeout(true);
				pixelbrush.setFadein(true);
				pixelbrush.setColor(brushcolor);
				pixelbrush.moveTo(random(NUMBEROFPIXELS));
				pixelbrush.setBounce(true);

				//setup the second brush
				brushcolor.h = 160;
				pixelbrush2.setSpeed(600);
				pixelbrush2.setSpeedlimit(1000);
				pixelbrush2.setFadeSpeed(random(80) + 50);
				pixelbrush2.setFadeout(true);
				pixelbrush2.setFadein(true);
				pixelbrush2.setColor(brushcolor);
				pixelbrush2.moveTo(random(NUMBEROFPIXELS));
				pixelbrush2.setBounce(true);
			}


			if (rand() % 10) //slowly (and randomly) change hue of brushes
			{
				brushcolor = pixelbrush.getColor();
				brushcolor.h += random(3) - 1; //randomly change hue a little ( ± random(1))
				pixelbrush.setColor(brushcolor);

				brushcolor = pixelbrush2.getColor();
				brushcolor.h += random(3) - 1; //randomly change hue a little ( ± random(1))
				pixelbrush2.setColor(brushcolor);

			}

			//slowly change speed of both brushes
			pixelbrush.setSpeed(pixelbrush.getSpeed() + random(6) - 3); //means speed = currentspeed ± random(3)
			pixelbrush2.setSpeed(pixelbrush2.getSpeed() + random(6) - 3); //means speed = currentspeed ± random(3)


			strip.ClearTo(RgbColor(zero)); //remove any previously applied paint

			pixelbrush.paint(); //apply the paint of the first brush to its assigned canvas (and update the brush)
			pixelbrush2.paint(); //apply the paint of the second brush to  its assigned canvas (and update the brush)

			pixelcanvas.transfer(); //transfer the first canvas to the strip
			pixelcanvas2.transfer(); //transfer the sedonc canvas to the neopixels (adding colors, rather than overwriting colors)

			if (smart_delay(3))
				return;
			strip.Show();
			
		}
		break; //quit the while loop immediately (and delete the created brush)
	}


}


void CalyKolor(uint8_t r, uint8_t g, uint8_t b)
{
	int par;

	for (;;)
	{


		for (par = 0; par < NUMBEROFPIXELS; par++)
		{
			strip.SetPixelColor(par, RgbColor(r, g, b));
			strip2.SetPixelColor(par, RgbColor(r, g, b));
		}



		strip.Show();
		strip2.Show();


		if (smart_delay(3))
			return;
		


	}


}

// przykład wcześniejszy użycia pokazu
void pokaz ()
{

 //main program loops through different animations, allocating addidional canvas and brushes if needed
	
  
 //Serial.println(F("Start perform"));
 mode = PRESENTATION;
  
}


void setup(void){


  randomSeed(analogRead(0)); //new random seed 

  //pinMode(led, OUTPUT);
  //digitalWrite(led, 0);
  //Serial.begin(115200);
  Serial.begin(921600);
  
//  WiFi.begin(ssid, password);
  Serial.println("");

  //pinMode(0, OUTPUT);

  // Wait for connection
 /* while (WiFi.status() != WL_CONNECTED) {
	delay(500);
	Serial.print("..");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("kompilacja 5");

  if (MDNS.begin(host)) {
	Serial.println("MDNS responder started");
  }
  */
  
  //server.on("/", handleRoot);
  
  server.on("/pokaz", [](AsyncWebServerRequest *request) {
	  if (!server.checkAuth(request))
		  return request->requestAuthentication();
		
	if (request->args())

	if (request->argName(0) == "mode")
	{
		//tylko jak piewszy parametr mode
		mode = request->arg("mode").toInt();
		if (mode<0 && mode>=MAX_MODE)
			mode = IDLE;
		mode_par1 = request->arg("arg1").toInt();
		mode_par2 = request->arg("arg2").toInt();
		mode_par3 = request->arg("arg3").toInt();
	
		Serial.println("");
		Serial.print("Parametry wartość mode - ");
		Serial.print(request->argName(0));
		Serial.print(", wartosc paremetru mode - ");
		Serial.print(request->arg("arg1"));
		Serial.print(" ");


	}

	Serial.print(mode);
	Serial.println(" ");
	zmiana_mode = true;

	tmp_string = "Bedzie pokaz mode=";
	tmp_string += mode;
	tmp_string += ", par1=";
	tmp_string += mode_par1;
	tmp_string += ", par2=";
	tmp_string += mode_par2;
	tmp_string += ", par3=";
	tmp_string += mode_par3;


	String values = "";

	values += "mode|" + (String)mode + "|input\n";
	values += "arg1|" + (String)mode_par1 + "|input\n";
	values += "arg2|" + (String)mode_par2 + "|input\n";
	values += "arg3|" + (String)mode_par3 + "|input\n";
	values += "version|" + (String)__DATE__+"_"+__TIME__+ "|input\n";

	request->send(200, "text/plain", values);
	values = "";

	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");


  });
  
  

  server.on("/ledstrip.html", [](AsyncWebServerRequest *request) {
	  if (!server.checkAuth(request))
		  return request->requestAuthentication();

	  if (request->args())

	  {

		  if (request->argName(0) == "mode")
		  {
			  //tylko jak piewszy parametr mode
			  mode = request->arg("mode").toInt();
			  if (mode<0 && mode >= MAX_MODE)
				  mode = IDLE;
			  mode_par1 = request->arg("arg1").toInt();
			  mode_par2 = request->arg("arg2").toInt();
			  mode_par3 = request->arg("arg3").toInt();

			  Serial.println("");
			  Serial.print("Parametry wartość mode - ");
			  Serial.print(request->argName(0));
			  Serial.print(", wartosc paremetru mode - ");
			  Serial.print(request->arg("arg1"));
			  Serial.print(" ");


		  }

	  Serial.print(mode);
	  Serial.println(" ");
	  zmiana_mode = true;

	  tmp_string = "Bedzie pokaz mode=";
	  tmp_string += mode;
	  tmp_string += ", par1=";
	  tmp_string += mode_par1;
	  tmp_string += ", par2=";
	  tmp_string += mode_par2;
	  tmp_string += ", par3=";
	  tmp_string += mode_par3;


	  //String values = "";

	  //values += "mode|" + (String)mode + "|input\n";
	  //values += "arg1|" + (String)mode_par1 + "|input\n";
	  //values += "arg2|" + (String)mode_par2 + "|input\n";
	  //values += "arg3|" + (String)mode_par3 + "|input\n";

	  //request->send(200, "text/plain", values);
	  //values = "";

	  }

	  server.handleFileRead(request->url(), request);

	  DEBUGLOG(__PRETTY_FUNCTION__);
	  DEBUGLOG("\r\n");


  });
  

  /*
  server.on("/rainbow", [](){
	server.send(200, "text/plain", "rainbow");
	rainbow(1);
  });

   server.on("/perf", [](){
	server.send(200, "text/plain", "perf");
	perform(1);
  });

  */
   //server.on("/reset", [](){
//	   server.send(200, "text/plain", "reset");
//	   ESP.restart();
//   });

   
//  server.onNotFound(handleNotFound);
 
  //httpUpdater.setup(&server);

 

  // Set callback
  //client.setCallback(callback);

  // Give name and ID to device
  //rest.set_id(device_id);
 // rest.set_name("relay_anywhere");
 
 
  SPIFFS.begin(); // Not really needed, checked inside library and started if needed
  server.begin(&SPIFFS);

  mode = IDLE;
  state = IDLE;

  strip.Begin();
  strip.Show(); // Initialize all pixels to 'off'

  strip2.Begin();
  strip2.Show();

  Serial.println("HTTP server started");

 Serial.println(" begin i show");
  //strip.SetBrightness(50);
  //strip2.SetBrightness(50);
 Serial.println("brignest");
  //check if ram allocation of brushes and canvases was successful (painting will not work if unsuccessful, program should still run though)
  //this check is optional but helps to check if something does not work, especially on low ram chips like the Arduino Uno
  if (pixelcanvas.isvalid() == false) Serial.println("canvas allocation problem");
  else  Serial.println("canvas allocation ok");


  if (pixelbrush.isvalid() == false) Serial.println("brush allocation problem");
  else  Serial.println("brush allocation ok");

  // rozpoczęcie wątków
  delay(1000);
  /*
  Scheduler.start(&timer_service);
  Scheduler.start(&net_service);
  Scheduler.start(&strip_service);

  Scheduler.begin();
  */

 

  // Set output topic
 // char* out_topic = rest.get_topic();

  ticker.attach(1, TickCallback);
  ticker.attach(18000, UpdateDynuDns); //co 3 godziny aktulizacja dyn dns
  
}

#define TimePresentationS	10	

void TickCallback()
{
	static int time_tick = 0;

	if (time_tick > TimePresentationS)
	{
		time_tick = 0;
		//zmiana_mode = true;
		//mode = rand() % MAX_MODE;

	}

	time_tick++;

	Serial.print("Tic Tac - ");
	Serial.print(time_tick);
	Serial.print("\n");
	
}


void UpdateDynuDns()
{


	HTTPClient http;

	Serial.print("[HTTP] begin...\n");
	// configure traged server and url
	http.begin("http://api.dynu.com/nic/update?hostname=mirkas.dynu.com&username=barbi111&password=a4b4d4dfdf614dbbdf3ac904b06872ea"); //HTTP

	Serial.print("[HTTP] GET...\n");
	// start connection and send HTTP header
	int httpCode = http.GET();

	// httpCode will be negative on error
	if (httpCode > 0) {
		// HTTP header has been send and Server response header has been handled
		Serial.printf("[HTTP] GET... code: %d\n", httpCode);

		// file found at server
		if (httpCode == HTTP_CODE_OK) {
			String payload = http.getString();
			Serial.println(payload);
		}
	}
	else {
		Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
	}

	http.end();

}

void callback(char* topic, byte* payload, unsigned int length) {

//	rest.handle_callback(client, topic, payload, length);

}


void loop(void){
  
 
	LoopFunction();
 
	//Serial.print(".");
  //Serial.println("Strip service ");

	  timeout_global = false;

	  switch (mode)
	  {
	  case IDLE:
		  // nic nie robimy
		  //czyścimy wyświetlacz
		  StripLights_MemClear(0);
		  strip.Show();
		  strip2.Show();
		  break;


	  case 1:
		  perform(mode_par1);
		  break;



	  case 2:

		  CalyKolor(mode_par1, mode_par2, mode_par3);

		  break;

	  case 3:
		  UpdateDynuDns();
		  //perform(0);

		  break;

	  case 4:

		  rainbow(5);
		  break;

	  case 5:

		  rainbow(40);
		  break;


	  case 6:

		  Rainbow_Paint();
		  break;

	  case 7:

		  SPARKLER(mode_par1);
		  break;

	  case 8:

		  TWINKLY_STARS();
		  break;

	  case 9:

		  CHASER();
		  break;

	  case 10:

		  HUE_FADER();
		  break;

	  case 11:

		  SPEEDTRAILS();
		  break;
	  case 12:

		  BOUNCY_BALLS();
		  break;

	  case 13:

		  TWO_BRUSH_COLORIMAX();
		  break;

	  case 14:

		  SPARKLER(mode_par1);
		  break;

	  case 15:

		  SPARKLER(mode_par1);
		  break;


		  // tutaj cypress 

	  case 45:
		  Icicle(6, 9, 6);
		  break;
	  case 46:
		  Sparkler(100, 1, 8, 0);
		  break;

	  case 47:
		  Sparkler(100, 1, 8, 1);
		  break;

	  case 48:
		  StripLights_DisplayClear(StripLights_LTGREEN);
		  break;
	  case 49:
		  StripLights_DisplayClear(StripLights_WHITE);
		  break;
	  case 50:
		  StripLights_DisplayClear(StripLights_BLUE);
		  break;
	  case 51:
		  SingleLEDPingPong(1, 6, StripLights_getColor(rand() % StripLights_COLOR_WHEEL_SIZE));
		  break;
	  case 52:
		  Tween1();
		  break;
	  case 53:
		  ColorFader(1, StripLights_getColor(rand() % StripLights_COLOR_WHEEL_SIZE));
		  break;
	  case 54:
		  CandyCane(5, StripLights_RED, StripLights_WHITE);
		  break;
	  case 55:
		  Snake(5);
		  break;
	  case 56:
		  ColorWheel(10);
		  break;
	  case 57:
		  CandyCane(5, StripLights_getColor(rand() % StripLights_COLOR_WHEEL_SIZE), StripLights_WHITE);
		  break;

	  case 58:

		  rand_color1.rgb = StripLights_getColor(rand() % StripLights_COLOR_WHEEL_SIZE);
		  rand_color2.rgb = StripLights_getColor(rand() % StripLights_COLOR_WHEEL_SIZE);

		  CandyCaneSmooth(5, rand_color1, rand_color2);
		  break;

	  case 59:

		  rand_color1.rgb = StripLights_getColor(rand() % StripLights_COLOR_WHEEL_SIZE);
		  rand_color2.rgb = StripLights_getColor(rand() % StripLights_COLOR_WHEEL_SIZE);

		  CandyCaneSmooth(10, rand_color1, rand_color2);
		  break;

	  case 60:
		  OppositeRings(50, ONOFF_ALLON, StripLights_BLUE, StripLights_RED);
		  break;

	  case 61:
		  OppositeRings(50, ONOFF_ALLON, StripLights_GREEN, StripLights_RED);
		  break;

	  case 62:
		  RgbChase(20, ONOFF_ALLON, RIGHT_ROTATION);
		  break;
	  case 63:
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
		  Gwiazdki(25, 5, 0, StripLights_WHITE);
		  break;

	  case 35:
		  Gwiazdki(25, 5, 0, StripLights_LTRED);
		  break;

	  case 36:
		  Gwiazdki(5, 5, 1, StripLights_CYAN);
		  break;

	  case 37:
		  //długie zanikanie szybko
		  Gwiazdki(5, 5, 1, 0);
		  break;

	  case 38:
		  Gwiazdki(25, 5, 0, 0);
		  break;
	  case 39:
		  Gwiazdki(25, 5, 0, StripLights_BLUE);
		  break;
	  case 40:
		  Gwiazdki(25, 5, 0, StripLights_GREEN);
		  break;
	  case 41:
		  Gwiazdki(25, 5, 0, StripLights_RED);
		  break;

		  //losowy jeden kolor mało
	  case 42:
		  Gwiazdki(25, 1, 0, rand() % StripLights_COLOR_WHEEL_SIZE);
		  break;
		  //losowy jeden kolor mało
	  case 43:
		  Gwiazdki(mode_par1, mode_par2, mode_par3, rand() % StripLights_COLOR_WHEEL_SIZE);
		  break;

	  case 44:
		  CaleLatanie(mode_par1, mode_par2, mode_par3);
		  break;

		  // konie gwiazdek

	  case 64:
		  SingleLED(mode_par1, ONOFF_ALLON, LEFT_ROTATION, StripLights_BLUE, StripLights_BLACK);
		  break;

	  case 65:
		  SingleLED(20, ONOFF_ALLON, RIGHT_ROTATION, StripLights_GREEN, StripLights_BLACK);
		  break;

	  case 66:
		  SingleLED(30, ONOFF_ALLON, LEFT_ROTATION, StripLights_CYAN, StripLights_BLACK);
		  break;

	  case 67:
		  SingleLED(20, ONOFF_ALLON, RIGHT_ROTATION, StripLights_VIOLET, StripLights_BLACK);
		  break;

	  case 25:
		  SingleLED(20, ONOFF_ALLON, RIGHT_ROTATION, StripLights_RED, StripLights_BLACK);
		  break;

	  case 21:
		  SingleLED(20, ONOFF_ALLON, RIGHT_ROTATION, StripLights_WHITE, StripLights_BLACK);
		  break;

	  case 32:
		  SingleLED(20, ONOFF_ALLON, RIGHT_ROTATION, StripLights_WHITE, StripLights_BLACK);
		  break;

	  case 33:
		  SingleLED(20, ONOFF_ALLON, LEFT_ROTATION, StripLights_WHITE, StripLights_BLACK);
		  break;


	  case 68:
		  RgbChase(mode_par1, mode_par2, mode_par3);
		  break;

	  case 69:
		  RgbChase(100, ONOFF_ALLON, LEFT_ROTATION);
		  break;

	  case 70:
		  OneColor(10, ONOFF_CHASE);
		  break;

	  case 71:
		  OneColor(80, ONOFF_CHASE);
		  break;

	  case 72:
		  RingRainbow(100, ONOFF_ALLON);
		  break;

	  case 73:
		  RingRainbow(100, ONOFF_ALLON);
		  break;

	  case 74:
		  RingRainbow(100, ONOFF_ALLON);
		  break;

	  case 75:
		  RingRainbow(100, ONOFF_ALLON);
		  break;

	  case 16:
		  Sparkle(100, SPARKLE_MULTI);
		  break;

	  case 17:
		  Sparkle(150, SPARKLE_ONE);
		  break;

	  case 18:
		  OppositeRings(120, ONOFF_ALLON, StripLights_WHITE, StripLights_BLUE);
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
		  OppositeRings(50, ONOFF_ALLON, StripLights_WHITE, StripLights_RED);
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
		  OppositeRings(20, ONOFF_ALLON, StripLights_WHITE, StripLights_BLUE);
		  break;



	  default:

		  break;
	  }


 
  
}


void LoopFunction()
{


	server.handle();
//	rest.loop(client);
	
}