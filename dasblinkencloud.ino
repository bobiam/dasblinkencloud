//DasBlinkenCloud is a cloud-based LED lifeform built by Bob Eells in 2016.
//This code comes from a number of places and stands on the shoulders of giants.
//Many thanks to Mark Kriegsman, Andrew Tuline, Daniel Wilson and the many others who've shared their blinky light code on the web.
//I've attempted to preserve comments giving attribution on patterns that aren't mine below.
//The structure of this code borrows heavily from the FastLED DemoReel pattern.

#include "FastLED.h"
#include "SoftwareSerial.h"

SoftwareSerial BTSerial(2, 3); // RX | TX

FASTLED_USING_NAMESPACE

#if FASTLED_VERSION < 3001000
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    6
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS    33

//fps is the maximum possible frames per second assuming the pattern contains no delays at all between frames.
//which is often (but not always) the case.
byte fps = 120; //frames per second

CRGB leds[NUM_LEDS];
byte brightness = 255;

void setup() {
  delay(3000); // 3 second delay for recovery

  BTSerial.println("DasBlinkenCloud is Online and ready to do your bidding!");  
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(brightness);

  BTSerial.begin(57600);  
  BTSerial.setTimeout(10);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { lightning, rainbow_cylon, lightning_rainbow, bw_rand, red_rand, green_rand, blue_rand, flashlight, rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, fire_rand, water_rand, earth_rand, air_rand  };
int num_patterns = 18;

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/fps); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  //EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically

  if(BTSerial.available())
  {
    //get the command off the Bluetooth buffer
    byte c = BTSerial.read();
    BTSerial.print(" I heard  ");
    BTSerial.println(c);
    
    //l = last pattern
    if(c == 108)
    {
      lastPattern();           
      BTSerial.print("Moving back to pattern index ");
      BTSerial.println(gCurrentPatternNumber);
    }    
    
    //n = next pattern
    if(c == 110)
    {
      nextPattern();
      BTSerial.print("Advancing to pattern index ");
      BTSerial.println(gCurrentPatternNumber);
    }    
    
    //p should always be followed by an integer, run a specific pattern
    if(c == 112)
    {
      //specific pattern.  Read an integer off of the buffer
      gCurrentPatternNumber = BTSerial.parseInt();
      //protect it
      if(gCurrentPatternNumber < 0 || gCurrentPatternNumber > num_patterns - 1)
      {
        BTSerial.println("Problem with pattern requested, defaulting to first pattern");
        gCurrentPatternNumber = 0;        
      }
      BTSerial.print("Running pattern");
      BTSerial.println(gCurrentPatternNumber);
    }
    
    //b is brightness, and should have an integer brightness.
    if(c == 98){
      //setting brightness, Read an integer off of the buffer
      brightness = BTSerial.parseInt();
      BTSerial.print("Setting brightness to: ");
      BTSerial.println(brightness);
      FastLED.setBrightness(brightness);
    }

    //s is speed, and should have an integer that represents the new framerate
        //p should always be followed by an integer, run a specific pattern
    if(c == 115)
    {
      //new framerate
      fps = BTSerial.parseInt();
      BTSerial.print("Updated framerate to: ");
      BTSerial.println(fps);
    }
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void lastPattern()
{
  if(gCurrentPatternNumber > 0)
  {
    gCurrentPatternNumber = gCurrentPatternNumber - 1;
  }else{
    gCurrentPatternNumber = num_patterns - 1;
  }
}

void green_rand()
{
  CRGB colors[24];
  for(int i=0;i<23;i++)
  {
    colors[i].setRGB(0,i*8,0); 
  }
  palette_rand(colors, 24, 200);
}

void red_rand()
{
  CRGB colors[24];
  for(int i=0;i<23;i++)
  {
    colors[i].setRGB(i*8,0,0); 
  }
  palette_rand(colors, 24, 200);
}

void blue_rand()
{
  CRGB colors[24];
  for(int i=0;i<23;i++)
  {
    colors[i].setRGB(0,0,i*8); 
  }
  palette_rand(colors, 24, 200);
}

void flashlight100(){
  brightness = 255;
  FastLED.setBrightness(255);
  flashlight();
}

void flashlight75(){
  brightness = 190;
  FastLED.setBrightness(190);
  flashlight();
}

void flashlight50(){
  brightness = 128;
  FastLED.setBrightness(128);
  flashlight();
}

void flashlight25(){
  brightness = 64;
  FastLED.setBrightness(64);
  flashlight();
}

void flashlight10(){
  brightness = 25;
  FastLED.setBrightness(25);
  flashlight();
}

void flashlight()
{
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::White;
    // Show the leds
    FastLED.show(); 
  }
}

void off()
{
  BTSerial.println("Turning Off");
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
    // Show the leds
    FastLED.show(); 
  }
}

void reset_brightness(){
  brightness = 255;
  FastLED.setBrightness(255);
}

void fire_rand()
{
  CRGB colors[] = {CRGB::Tomato, CRGB::Red, CRGB::OrangeRed, CRGB::Brown, CRGB::FireBrick, CRGB::Maroon};
  palette_rand(colors, 6, 200);
}

void water_rand()
{
  CRGB colors[] = {CRGB::Blue, CRGB::DarkBlue, CRGB::DarkTurquoise, CRGB::Aqua, CRGB::SeaGreen, CRGB::Aquamarine};
  palette_rand(colors, 6, 200);
}

void earth_rand()
{
  CRGB colors[] = {CRGB::Chartreuse, CRGB::Green, CRGB::DarkOliveGreen, CRGB::DarkGreen, CRGB::LimeGreen, CRGB::ForestGreen};
  palette_rand(colors, 6, 200);
}

void air_rand()
{
  CRGB colors[] = {CRGB::Azure, CRGB::Cyan, CRGB::DeepSkyBlue, CRGB::LightSkyBlue, CRGB::DodgerBlue, CRGB::Turquoise};
  palette_rand(colors, 6, 200);
}

void bw_rand()
{
  CRGB colors[] = {CRGB::Black, CRGB::White};
  palette_rand(colors, 2, 200);
}

void palette_rand(CRGB colors[], int colorCount,int wait)
{
    leds[random(0,NUM_LEDS)] = colors[random(0,colorCount)];
    FastLED.show();
    fadeall();
    FastLED.delay(wait); 
}


void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

void rainbow_cylon() { 
  static uint8_t hue = 0;
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show(); 
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(10);
  }

  // Now go in the other direction.  
  for(int i = (NUM_LEDS)-1; i >= 0; i--) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(10);
  }
}

//  Lightnings is a program that lets you make an LED strip look like a 1D cloud of lightning
//  Original by: Daniel Wilson, 2014
//  Modified by: Andrew Tuline 2015
//  Modified slightly by: Bob Eells 2016 - mostly for lightning_rainbow below

uint8_t frequency = 80;                                       // controls the interval between strikes
uint8_t flashes = 8;                                          //the upper limit of flashes per strike
unsigned int dimmer = 1;

uint8_t ledstart;                                             // Starting location of a flash
uint8_t ledlen;                                               // Length of a flash

void lightning(){
  all(CRGB::Black);
  ledstart = random8(NUM_LEDS);           // Determine starting location of flash
  ledlen = random8(NUM_LEDS-ledstart);    // Determine length of flash (not to go beyond NUM_LEDS-1)
  for (int flashCounter = 0; flashCounter < random8(3,flashes); flashCounter++) {
    if(flashCounter == 0) dimmer = 5;     // the brightness of the leader is scaled down by a factor of 5
    else dimmer = random8(1,3);           // return strokes are brighter than the leader
    fill_solid(leds+ledstart,ledlen,CHSV(255, 0, 255/dimmer));
    FastLED.show();                       // Show a section of LED's
    delay(random8(4,10));                 // each flash only lasts 4-10 milliseconds
    fill_solid(leds+ledstart,ledlen,CHSV(255,0,0));   // Clear the section of LED's
    FastLED.show();     
    if (flashCounter == 0) delay (150);   // longer delay until next flash after the leader
    delay(50+random8(100));               // shorter delay between strokes  
  } // for()
  delay(random8(frequency)*100);          // delay between strikes
}

void lightning_rainbow(){
  all(CRGB::Black);
  ledstart = random8(NUM_LEDS);           // Determine starting location of flash
  ledlen = random8(NUM_LEDS-ledstart);    // Determine length of flash (not to go beyond NUM_LEDS-1)
  for (int flashCounter = 0; flashCounter < random8(3,flashes); flashCounter++) {
    if(flashCounter == 0) dimmer = 5;     // the brightness of the leader is scaled down by a factor of 5
    else dimmer = random8(1,3);           // return strokes are brighter than the leader
    fill_solid(leds+ledstart,ledlen,CHSV(random(255), 255, 255/dimmer));
    FastLED.show();                       // Show a section of LED's
    delay(random8(3,16));                 // duration of min/max delays in ms
    fill_solid(leds+ledstart,ledlen,CHSV(random(255),255,0));   // Clear the section of LED's
    FastLED.show();     
    if (flashCounter == 0) delay (150);   // longer delay until next flash after the leader
    delay(50+random8(100));               // shorter delay between strokes  
  } // for()
  delay(random8(frequency)*100);          // delay between strikes
}



void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}


//all
//helper fucntion, sets entire ring to provided color in one frame.
void all(CRGB all_c){
  for(int i=0;i<NUM_LEDS;i++)
  {
    leds[i] = all_c;
  }
}

