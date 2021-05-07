#include <Adafruit_NeoPixel.h>
#include <Servo.h> 

#define PIN 2	 // pin Neopixel is attached to
#define SENSOR A0		//input pin for Potentiometer
#define NUMPIXELS 1 // number of neopixels in strip
#define MOTORPIN 3
#define GASPIN A1
#define PIEZOPIN 4
#define FLEXPIN A2
#define SERVOPIN 5
#define MAINSERVO 7
//SHU
#define temperature A3
#define Photo_Pin A4
//SHU
/*THese values adjust what is considered wet or dry*/
#define dryThreshold 50 //below this value, begin alerting dry, turn red;
#define wetThreshold 200 //above this value, begin alerting wet,turn blue;
#define thresholdCenter (dryThreshold + wetThreshold)/2 //Brightest Green point
#define crossFade 20 //how much blue and red should fade in to green
#define gasThreshold 150

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Servo Servo1;
int delayval = 100; // timing delay in milliseconds

int redColor = 0;
int greenColor = 0;
int blueColor = 0;

//Sensor reading and the value converted from 1024 to 255 for output
int sensorValue  = 0;
int transitionValue = 0;
//SHU
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
int LightPin = 6;
int readPhoto;
// the setup routine runs once when you press reset:
Servo mainServo;
int position = 0;
int previousPosition;
//SHU
void setup() {
  // Initialize the NeoPixel library.
  Serial.begin(9600);
  Servo1.attach(SERVOPIN);
  pixels.begin();
  pinMode(MOTORPIN, OUTPUT);
  pinMode(PIEZOPIN,INPUT);
   //SHU
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  pinMode(LightPin, OUTPUT);
  
  pinMode(Photo_Pin, INPUT);
  //
   pinMode(temperature, INPUT);
  
  mainServo.attach(MAINSERVO);
  
  Serial.begin(9600);
  //SHU
}

void loop() {
  	gasSensor();
  	windSensor();
  	moistureSensor();
  //SHU
  	lightSensor();
  	tempSensor();
    delay(delayval);
  //SHU
}
void tempSensor()
{
  // Temperature analysis
  int tempReading = analogRead(temperature);
  // If using 5v input
  float voltage = tempReading * 5.0; 
  
  // Divided by 1024
  voltage /= 1024.0;
  //Converting from 10mv per degree
  float tempC = (voltage - 0.5) * 100;
  
  // This maps temperature to degrees open for the flap
  int position = map(tempC, 20, 40, 0, 90);
  Serial.println(position);
  if(position>=90)
  {
	 mainServo.write(90);
     delay(1000);
  }
  else
  {
  if(previousPosition != position)
  {
     mainServo.write(position);
     delay(1000);
  }
  }
  
  previousPosition = position;
}
void lightSensor()
{
  digitalWrite(led, LOW);   // turn the LED on (HIGH is the voltage level)
  readPhoto = analogRead(Photo_Pin);
  
  if (readPhoto < 518) {
  digitalWrite(LightPin, HIGH);
  }else{
      digitalWrite(LightPin, LOW);
  }
  
  delay(1);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  //delay(1000);               // wait for a second
}
void windSensor()
{
  int flex=analogRead(FLEXPIN);
  flex=map(flex,990,1017,0,90);
  //Serial.println(flex);
  Servo1.write(90-flex);
  delay(100);
}
void gasSensor()
{
  int gas=analogRead(GASPIN);
  //Serial.println(gas);
  if(gas>gasThreshold)
  	tone(PIEZOPIN, 1000,200);
  else
    noTone(PIEZOPIN);
}
void setMotorSpeed(int value)
{
 	if(value<50)
      analogWrite(MOTORPIN,50-value);
  	else
      analogWrite(MOTORPIN,0);
}
void setColor(){
    //red value greater towards higher resistance/drier
redColor = ((transitionValue <= dryThreshold + crossFade) && (transitionValue >= 0 ))? map(transitionValue,0,dryThreshold + crossFade,255,0) : 0;
  //blue value greater towards lower resistance/wetter
blueColor = (transitionValue >= wetThreshold - crossFade && transitionValue <= 255)? map(transitionValue,wetThreshold - crossFade,255,0,255):0;   
  //green value towrds middle resistance
  if(transitionValue >= dryThreshold && transitionValue <= thresholdCenter)
  {
  greenColor = map(transitionValue,dryThreshold,thresholdCenter,0,255);
  }
  else if(transitionValue > thresholdCenter && transitionValue < wetThreshold)
  {
    greenColor = map(transitionValue,dryThreshold,thresholdCenter,255,0);
  }
  else{
    greenColor = 0;
  }
}
void moistureSensor()
{
  sensorValue = analogRead(SENSOR);
	transitionValue = map(sensorValue,0,1023,0,255);	
  	setMotorSpeed(transitionValue);
	setColor();
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(0,redColor,greenColor,blueColor);
	
    // This sends the updated pixel color to the hardware.
    pixels.show();
}

