
#include <Arduino.h>
#include "../lib/SoundPlayer/SoundPlayer.h"
#include "../lib/FSM/FSM.h"
#include "../lib/PotMonitor/PotMonitor.h"
#include "NeonSectionController.h"
#include "FastLED.h" // FastLED library. Preferably the latest copy of FastLED 2.1.
#include "AnalogButton.h"
#include <FastLED.h> // for short list

// Fixed definitions cannot change on the fly.
#define COLOR_ORDER GRB // It's GRB for WS2812 and BGR for APA102.
#define DATA_PIN 5
#define NUM_LEDS 16
#define FRAME_PER_SECOND 60

#define GO_TO_GAME_LOOP 1
#define GO_TO_START_GAME 2

#define MIN_SIGNAL 50
#define COMPLETION_TIMEOUT 8 * 1000

#define SOUND_ON 1
#define SOUND_ERROR 2

uint8_t game_brightness = 200;
uint8_t complete_brightness = 255; // Overall brightness definition. It can be changed on the fly.

//struct CRGB leds[NUM_LEDS]; // Initialize our LED array.
CRGB leds[NUM_LEDS];

CRGB wireColor0 = CRGB::Green;
CRGB wireColor1 = CRGB::Red;
CRGB wireColor2 = CRGB::Yellow;
CRGB wireColor3 = CRGB::Blue;

State StateDoNothing(NULL, NULL, NULL);
Fsm fsm(&StateDoNothing);

bool combinationHasBeenCompleted = false;
bool wireCorrect0 = false;
bool wireCorrect1 = false;
bool wireCorrect2 = false;
bool wireCorrect3 = false;

PotMonitor volumePot(A5, 12);
SoundPlayer soundPlayer(18, 15);

NeonSectionController neonController0(1, 3);
NeonSectionController neonController1(4, 3);
NeonSectionController neonController2(9, 3);
NeonSectionController neonController3(12, 3);

NeonSectionController inputColorController0(0, 1);
NeonSectionController inputColorController1(7, 1);
NeonSectionController inputColorController2(8, 1);
NeonSectionController inputColorController3(15, 1);

// ToggleButton wireButton0(A0, false, 50, true);
// ToggleButton wireButton1(A1, false, 50, true);
// ToggleButton wireButton2(A2, false, 50, true);
// ToggleButton wireButton3(A3, false, 50, true);

AnalogButton wireButton0(A0);
AnalogButton wireButton1(A1);
AnalogButton wireButton2(A2);
AnalogButton wireButton3(A3);

unsigned long timer = 0;

bool restartMap = false;

struct ColorObject
{
  CRGB color;
  // int pin;
  // int id;
};

ColorObject colorToken0 = {wireColor0};
ColorObject colorToken1 = {wireColor1};
ColorObject colorToken2 = {wireColor2};
ColorObject colorToken3 = {wireColor3};

ColorObject answerArray[4] = {colorToken0, colorToken1, colorToken2, colorToken3};

long map2(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min;
}

bool inRange(int value, int d, int range)
{
  return (((d - range) <= value) && (value <= (d + range)));
}

void ShuffleArray()
{
  const int questionCount = 4;
  for (int i = 0; i < questionCount; i++)
  {
    int n = random(0, questionCount);
    ColorObject temp = answerArray[n];
    answerArray[n] = answerArray[i];
    answerArray[i] = temp;
  }
}

CRGB findSignalColor(int value)
{

  int pinVal = analogRead(A9);
  if (inRange(pinVal, value, 20))
  {
    Serial.println("Green");
    return wireColor0;
  }

  pinVal = analogRead(A8);
  if (inRange(pinVal, value, 20))
  {
    Serial.println("Red");
    return wireColor1;
  }

  pinVal = analogRead(A7);
  if (inRange(pinVal, value, 20))
  {
    Serial.println("Yellow");
    return wireColor2;
  }

  pinVal = analogRead(A6);
  if (inRange(pinVal, value, 20))
  {
    Serial.println("Yellow");

    return wireColor3;
  }
  return CRGB::Black;
}

bool mapCompleted()
{
  return (wireCorrect0 && wireCorrect1 && wireCorrect2 && wireCorrect3);
}
void playSound(int idx)
{
  if (!restartMap)
  {
    soundPlayer.PlaySound(idx);
  }
}
//Setup game board
void OnStartGameEnter()
{
  //Scramble Array
  Serial.println("change the board");
  ShuffleArray();
  //Set lights
  inputColorController0.setColor(answerArray[0].color);
  inputColorController1.setColor(answerArray[1].color);
  inputColorController2.setColor(answerArray[2].color);
  inputColorController3.setColor(answerArray[3].color);

  neonController0.setColor(answerArray[0].color);
  neonController1.setColor(answerArray[1].color);
  neonController2.setColor(answerArray[2].color);
  neonController3.setColor(answerArray[3].color);

  combinationHasBeenCompleted = false;

  restartMap = true;
  FastLED.setBrightness(game_brightness);
}

void OnStateStartGameUpdate()
{
  fsm.trigger(GO_TO_GAME_LOOP);
}

void OnStateGameLoopEnter()
{
}

void OnStateGameLoopUpdate()
{

  wireButton0.read();
  wireButton1.read();
  wireButton2.read();
  wireButton3.read();

  int pinVal = 0;

  if (wireButton0.wasPressed() || wireButton0.wasReleased() || restartMap)
  {
    pinVal = analogRead(A0);
    Serial.println("wireButton 0");
    wireCorrect0 = false;
    if (pinVal > MIN_SIGNAL)
    {
      if (answerArray[0].color == findSignalColor(pinVal))
      {
        if (neonController0.setState(WireState::ON))
        {
          playSound(SOUND_ON);
        }
        wireCorrect0 = true;
      }
      else
      {
        if (neonController0.setState(WireState::EFFECT))
        {
          playSound(SOUND_ERROR);
        }
      }
    }
    else
    {
      combinationHasBeenCompleted = false;
      neonController0.setState(WireState::OFF);
    }
  }
  if (wireButton1.wasPressed() || wireButton1.wasReleased() || restartMap)
  {
    pinVal = analogRead(A1);
    Serial.println("wireButton 1");
    wireCorrect1 = false;

    if (pinVal > MIN_SIGNAL)
    {
      if (answerArray[1].color == findSignalColor(pinVal))
      {
        if (neonController1.setState(WireState::ON))
        {
          playSound(SOUND_ON);
        }
        wireCorrect1 = true;
      }
      else
      {
        if (neonController1.setState(WireState::EFFECT))
        {
          playSound(SOUND_ERROR);
        }
      }
    }
    else
    {
      combinationHasBeenCompleted = false;
      neonController1.setState(WireState::OFF);
    }
  }
  if (wireButton2.wasPressed() || wireButton2.wasReleased() || restartMap)
  {
    pinVal = analogRead(A2);
    Serial.println("wireButton 2");
    wireCorrect2 = false;

    if (pinVal > MIN_SIGNAL)
    {
      if (answerArray[2].color == findSignalColor(pinVal))
      {
        if (neonController2.setState(WireState::ON))
        {
          playSound(SOUND_ON);
        }
        wireCorrect2 = true;
      }
      else
      {
        if (neonController2.setState(WireState::EFFECT))
        {
          playSound(SOUND_ERROR);
        }
      }
    }
    else
    {
      combinationHasBeenCompleted = false;
      neonController2.setState(WireState::OFF);
    }
  }
  if (wireButton3.wasPressed() || wireButton3.wasReleased() || restartMap)
  {
    pinVal = analogRead(A3);
    Serial.println("wireButton 3");
    wireCorrect3 = false;

    if (pinVal > MIN_SIGNAL)
    {
      if (answerArray[3].color == findSignalColor(pinVal))
      {

        if (neonController3.setState(WireState::ON))
        {
          playSound(SOUND_ON);
        }
        wireCorrect3 = true;
      }
      else
      {

        if (neonController3.setState(WireState::EFFECT))
        {
          playSound(SOUND_ERROR);
        }
      }
    }
    else
    {
      combinationHasBeenCompleted = false;
      neonController3.setState(WireState::OFF);
    }
  }

  restartMap = false;

  //neonController0.setState(WireState::EFFECT);
  //neonController2.setState(WireState::EFFECT);


  if (!combinationHasBeenCompleted && mapCompleted())
  {
    // play sound & start timer!
    combinationHasBeenCompleted = true;
    FastLED.setBrightness(complete_brightness);
    timer = millis();
    Serial.println("completed map");
  }

  // check to reset game board
  if (combinationHasBeenCompleted && (millis() - timer) > COMPLETION_TIMEOUT)
  {
    Serial.println("go to start game");
    fsm.trigger(GO_TO_START_GAME);
  }
}

State StateStartGame(&OnStartGameEnter, &OnStateStartGameUpdate, NULL);
State StateGameLoop(&OnStateGameLoopEnter, &OnStateGameLoopUpdate, NULL);

void setup()
{
  Serial.begin(9600); // Initialize serial port for debugging.
  delay(5000);        // Soft startup to ease the flow of electrons.

  fsm.add_transition(&StateStartGame, &StateGameLoop,
                     GO_TO_GAME_LOOP, NULL);

  fsm.add_transition(&StateGameLoop, &StateStartGame,
                     GO_TO_START_GAME, NULL);

  // fsm.add_timed_transition(&StateGameLoop, &StateStartGame, 2000, NULL);

  FastLED.addLeds<UCS1903, DATA_PIN, BRG>(leds, NUM_LEDS);
  FastLED.setBrightness(game_brightness);

  neonController0.initialize(leds, wireColor0);
  neonController1.initialize(leds, wireColor1);
  neonController2.initialize(leds, wireColor2);
  neonController3.initialize(leds, wireColor3);

  inputColorController0.initialize(leds, wireColor0);
  inputColorController1.initialize(leds, wireColor1);
  inputColorController2.initialize(leds, wireColor2);
  inputColorController3.initialize(leds, wireColor3);

  inputColorController0.setState(WireState::ON);
  inputColorController1.setState(WireState::ON);
  inputColorController2.setState(WireState::ON);
  inputColorController3.setState(WireState::ON);

  Serial.println(" Start up activitiy");

  randomSeed(analogRead(A4));

  wireButton0.begin();
  wireButton1.begin();
  wireButton2.begin();
  wireButton3.begin();
  //input pins
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  //output pins
  pinMode(A9, INPUT);
  pinMode(A8, INPUT);
  pinMode(A7, INPUT);
  pinMode(A6, INPUT);

  soundPlayer.initialize();

  fsm.goToState(&StateStartGame);
}

void loop()
{

  if (volumePot.hasUpdated())
  {
    long volume = map2(volumePot.getValue(), 0, 1023, 0, 30);
    //Serial.println(volume);
    soundPlayer.volume(volume);
  }

  neonController0.update();
  neonController1.update();
  neonController2.update();
  neonController3.update();

  inputColorController0.update();
  inputColorController1.update();
  inputColorController2.update();
  inputColorController3.update();

  fsm.run_machine();

  soundPlayer.update();

  FastLED.show();
  // Serial.println("run");
  delay(1000 / FRAME_PER_SECOND);
}
