#include <ESP32Servo.h>
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define LED2 2
#define DIGITAL_PIN 36

Servo servo1;
Servo servo2;
const int servo1Pin = 23;
const int servo2Pin = 22;
const int freq = 1000;
const int openTime = 5000; // 5 seconds
int servoPos = 0;

const int trigPin = 19;
const int echoPin = 18;

// define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;

uint8_t receiver_mac[] = {0x3C, 0x61, 0x05, 0x03, 0x9B, 0x14};

typedef struct struct_message { // สร้างตัวแปรแพ็จเกจแบบ struct
  int trashPercent;
  char message[32];
} struct_message;

struct_message incomingReading;
int trash_percent = 0;
void onDataReceived(const uint8_t *senderMac, const uint8_t *incomingData, int len)
{
  memcpy(&incomingReading, incomingData, sizeof(incomingReading));
  Serial.print("Received : ");
  Serial.println(incomingReading.trashPercent);
  Serial.println(incomingReading.message);

  trash_percent = incomingReading.trashPercent;
}

void setupWifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  else {
    Serial.println("ESP-NOW Initialized");
  }
  esp_now_register_recv_cb(onDataReceived);
}

void setupServo()
{
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  servo1.setPeriodHertz(50);
  servo1.attach(servo1Pin, 1000, 2000);

  servo2.setPeriodHertz(50);
  servo2.attach(servo2Pin, 1000, 2000);
}

void setupUltraSonic()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void servoOn()
{
  for (servoPos = 0; servoPos <= 180; servoPos += 1)
  {
    servo1.write(servoPos);
    servo2.write(servoPos);
    delay(1);
  }
}

void servoOff()
{
  for (servoPos = 180; servoPos >= 0; servoPos -= 1)
  {
    servo1.write(servoPos);
    servo2.write(servoPos);
    delay(1);
  }
}

bool checkOpener()
{
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED / 2;

  Serial.printf("Distance: %.2f\n", distanceCm);

  if (distanceCm < 20)
  {
    return true;
  }
  return false;
}

void initUltraSonic()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
}

unsigned long time_to_close = 5;
unsigned long current_time;
bool isServoOn = false;


void setup()
{
  Serial.begin(9600);
  setupWifi();
  pinMode(LED2, OUTPUT);
  pinMode(DIGITAL_PIN, INPUT);

  setupUltraSonic();
  setupServo();
}

void loop()
{
  initUltraSonic();

  if (checkOpener() && trash_percent <= 70)
  {
    current_time = millis() + time_to_close * 1000;
    if (!isServoOn)
    {
      servoOn();
      isServoOn = true;
    }
  }

  if (millis() > current_time && isServoOn)
  {
    Serial.printf("%ul, %ul\n", current_time, millis());
    servoOff();
    isServoOn = false;
  }

}