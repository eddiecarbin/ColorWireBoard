
#include <Arduino.h>
#include "../lib/SoundPlayer/SoundPlayer.h"
#include "../lib/FSM/FSM.h"
#include "../lib/PotMonitor/PotMonitor.h"
#include "NeonSectionController.h"
#include "FastLED.h" // FastLED library. Preferably the latest copy of FastLED 2.1.

#include <FastLED.h> // for short list

// Fixed definitions cannot change on the fly.
#define COLOR_ORDER GRB // It's GRB for WS2812 and BGR for APA102.
#define DATA_PIN 5
#define NUM_LEDS 16
#define FRAME_PER_SECOND 60

#define GO_TO_GAME_LOOP 1
#define GO_TO_START_GAME 2

#define MIN_SIGNAL 50
#define COMPLETION_TIMEOUT 15 * 1000

#define SOUND_ON 1
#define SOUND_ERROR 2

uint8_t game_brightness = 200;
uint8_t complete_brightness = 255; // Overall brightness definition. It can be changed on the fly.

//struct CRGB leds[NUM_LEDS]; // Initialize our LED array.
CRGB leds[NUM_LEDS];

CRGB wireColor0 = CRGB::Red;
CRGB wireColor1 = CRGB::Green;
CRGB wireColor2 = CRGB::Yellow;
CRGB wireColor3 = CRGB::Blue;

State StateDoNothing(NULL, NULL, NULL);
Fsm fsm(&StateDoNothing);

float a0, a1, a2, a3, b0, b1, b2, b3;

bool combinationHasBeenCompleted = false;

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

ToggleButton wireButton0(a0, false, 50, false);
ToggleButton wireButton1(a1, false, 50, false);
ToggleButton wireButton2(a2, false, 50, false);
ToggleButton wireButton3(a3, false, 50, false);

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

  b0 = analogRead(A9);
  b0 = analogRead(A9);
  b1 = analogRead(A8);
  b2 = analogRead(A7);
  b3 = analogRead(A6);

  if (inRange(b0, value, 20))
  {
    return wireColor0;
  }
  if (inRange(b1, value, 20))
  {
    return wireColor1;
  }
  if (inRange(b2, value, 20))
  {
    return wireColor2;
  }
  if (inRange(b3, value, 20))
  {
    return wireColor3;
  }
  return CRGB::Black;
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

  // if ( wireButton0.changed()){
  // Serial.println("Wire button 0 changed");
  // a0 = analogRead(A0);
  // }
  a0 = analogRead(A0);
  a1 = analogRead(A1);
  a2 = analogRead(A2);
  a3 = analogRead(A3);
  int correctCount = 0;

  if (wireButton0.wasPressed() || wireButton0.wasReleased())
  {
    a0 = analogRead(A0);
  }
  if (a0 > MIN_SIGNAL)
  {
    if (answerArray[0].color == findSignalColor(a0))
    {
      if (neonController0.setState(WireState::ON))
      {
        playSound(SOUND_ON);
      }
      correctCount += 1;
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
  if (a1 > MIN_SIGNAL)
  {
    if (answerArray[1].color == findSignalColor(a1))
    {
      if (neonController1.setState(WireState::ON))
      {
        playSound(SOUND_ON);
      }
      correctCount += 1;
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
  if (a2 > MIN_SIGNAL)
  {
    if (answerArray[2].color == findSignalColor(a2))
    {
      if (neonController2.setState(WireState::ON))
      {
        playSound(SOUND_ON);
      }
      correctCount += 1;
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

  if (a3 > MIN_SIGNAL)
  {
    if (answerArray[3].color == findSignalColor(a3))
    {

      if (neonController3.setState(WireState::ON))
      {
        playSound(SOUND_ON);
      }
      correctCount += 1;
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

  restartMap = false;

  if (!combinationHasBeenCompleted && correctCount >= 4)
  {
    // play sound & start timer!
    combinationHasBeenCompleted = true;
    FastLED.setBrightness(complete_brightness);
    timer = millis();
  }

  // check to reset game board
  if (combinationHasBeenCompleted && (millis() - timer) > COMPLETION_TIMEOUT)
  {
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

  neonController0.setState(WireState::EFFECT);
  neonController2.setState(WireState::ON);

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
