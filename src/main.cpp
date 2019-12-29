#include <Arduino.h>
int ledPin = 9;    // LED connected to digital pin 9
int analogPin = 3; // potentiometer connected to analog pin 3
int val = 0;       // variable to store the read value

void setup()
{
  // pinMode(ledPin, OUTPUT); // sets the pin as output
  // pinMode(analogPin, INPUT);

  pinMode(ledPin, INPUT_PULLUP); 
  analogWrite(ledPin, 100);
}

void loop()
{
  val = analogRead(ledPin);  // read the input pin
  
  Serial.println(val);

  //analogWrite(ledPin, val / 4); // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
}