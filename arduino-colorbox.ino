
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 4
Adafruit_NeoPixel strip = Adafruit_NeoPixel(25, PIN, NEO_RGB + NEO_KHZ800);


int encoderPinA = 2;
int encoderPinB = 3;
int encoderPinS = 7;
int encoderPos = 0;

long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 100;    // the debounce time; increase if the output flickers

boolean menuMode = true;


typedef byte Frame[5];

#include "frames.h"


void setup() {
	pinMode(encoderPinA,INPUT);
	pinMode(encoderPinB,INPUT);
	pinMode(encoderPinS,INPUT);

	attachInterrupt(digitalPinToInterrupt(encoderPinS), changeMode, RISING);
	attachInterrupt(digitalPinToInterrupt(encoderPinA), encoderStep, RISING);
	attachInterrupt(digitalPinToInterrupt(encoderPinB), encoderStep, RISING);

	strip.begin();
	strip.show(); // Initialize all pixels to 'off'

	//Initialize serial and wait for port to open:
	Serial.begin(9600);
	while (!Serial) {
		; // wait for serial port to connect. Needed for Leonardo only
	}
}


void loop() {
	uint16_t i;
	if(menuMode){
		// Serial.println("menuMode is TRUE");
		// Serial.println (encoder0Pos, DEC);
		for(i=0; i<strip.numPixels(); i++) {
			if(i == encoderPos){
				strip.setPixelColor(i, strip.Color(255, 0, 0));
				} else {
					strip.setPixelColor(i, strip.Color(0, 0, 0));
				}
			}
			strip.show();
			delay(100);


			} else {
	//Serial.println("menuMode is FALSE");
	switch (encoderPos) {
		case 0:
			colorWipe(strip.Color(255, 0, 0), 50); // Red
			colorWipe(strip.Color(0, 0, 0), 50);
		break;
		case 1:
			colorWipe(strip.Color(0, 255, 0), 50); // Green
			colorWipe(strip.Color(0, 0, 0), 50);
		break;
		case 2:
			colorWipe(strip.Color(0, 0, 255), 50); // Blue
			colorWipe(strip.Color(0, 0, 0), 50);
		break;
		case 3:
			theaterChase(strip.Color(127, 127, 127), 50); // White
		break;
		case 4:
			theaterChase(strip.Color(127, 0, 0), 50); // Red
		break;
		case 5:
			theaterChase(strip.Color(0, 0, 127), 50); // Blue
		break;
		case 6:
			rainbow(20);
		break;
		case 7:
			rainbowCycle(20);
		break;
		case 8:
			theaterChaseRainbow(50);
		break;
		case 9:
			//frames(PunkFrames, 50);
		break;
		default: 
			// if nothing else matches, do the default
			// default is optional
		break;
		}
	}
}

/*****************************************************************************
**
*****************************************************************************/
void encoderStep() {
	if ((millis() - lastDebounceTime) > debounceDelay && menuMode) {
		noInterrupts();
		if(digitalRead(encoderPinA) == HIGH){
			encoderPos = encoderPos + 1;
			if(encoderPos > 24){
				encoderPos = 0;
			}
		} else {
			encoderPos = encoderPos - 1;
			if(encoderPos < 0){
				encoderPos = 24;
			}
		}
		lastDebounceTime = millis();
		Serial.println (encoderPos, DEC);
		interrupts();
	}
}

void changeMode() {
	if ((millis() - lastDebounceTime) > debounceDelay) {
		if(menuMode){
			Serial.println("Setting menuMode FALSE");
			menuMode = false;
		} else {
			Serial.println("Setting menuMode TRUE");
			menuMode = true;
		}
		lastDebounceTime = millis();
	}
}

/*****************************************************************************
**
*****************************************************************************/


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
	for(uint16_t i=0; i<strip.numPixels(); i++) {
		strip.setPixelColor(i, c);
		strip.show();
		delay(wait);
	}
}

void rainbow(uint8_t wait) {
	uint16_t i, j;

	for(j=0; j<256; j++) {
		for(i=0; i<strip.numPixels(); i++) {
			strip.setPixelColor(i, Wheel((i+j) & 255));
		}
		strip.show();
		delay(wait);
	}
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
	uint16_t i, j;

	for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
		for(i=0; i< strip.numPixels(); i++) {
			strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
		}
		strip.show();
		delay(wait);
	}
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
	for (int j=0; j<10; j++) {  //do 10 cycles of chasing
		for (int q=0; q < 3; q++) {
			for (int i=0; i < strip.numPixels(); i=i+3) {
				strip.setPixelColor(i+q, c);    //turn every third pixel on
			}
			strip.show();

			delay(wait);

			for (int i=0; i < strip.numPixels(); i=i+3) {
				strip.setPixelColor(i+q, 0);        //turn every third pixel off
			}
		}
	}
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
	for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
		for (int q=0; q < 3; q++) {
			for (int i=0; i < strip.numPixels(); i=i+3) {
				strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
			}
			strip.show();

			delay(wait);

			for (int i=0; i < strip.numPixels(); i=i+3) {
				strip.setPixelColor(i+q, 0);        //turn every third pixel off
			}
		}
	}
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
	WheelPos = 255 - WheelPos;
	if(WheelPos < 85) {
		return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
	}
	if(WheelPos < 170) {
		WheelPos -= 85;
		return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
	WheelPos -= 170;
	return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void framesPlayer(Frame f, uint8_t wait) {
	
}

