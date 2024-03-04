#include <esp_now.h>
#include <WiFi.h>

#pragma region ESPNow
uint8_t jiratAddress[] = {0x3C, 0x61, 0x05, 0x03, 0x9B, 0x14};
uint8_t natAddress[] = {0xA4, 0xCF, 0x12, 0x8F, 0xD1, 0x30};
int trashPercent = 0;

// uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

typedef struct struct_message
{ // สร้างตัวแปรแพ็จเกจแบบ struct
  int trashPercent;
  char message[32];
} struct_message;

struct_message myData; // ตัวแปรแพ็คเกจที่ต้องการส่ง

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  // Serial.print("\r\nLast Packet Send Status:\t");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setupESPNow()
{
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {

    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // ### Jirat
  // Register peer
  esp_now_peer_info_t jiratPeerInfo = {};
  memcpy(jiratPeerInfo.peer_addr, jiratAddress, 6);
  jiratPeerInfo.channel = 0;
  jiratPeerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&jiratPeerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer jirat");
    return;
  }

  // ### Nat
  // Register peer
  esp_now_peer_info_t natPeerInfo = {};
  memcpy(natPeerInfo.peer_addr, natAddress, 6);
  natPeerInfo.channel = 0;
  natPeerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&natPeerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer nat");
    return;
  }
}

void sendData()
{
  strcpy(myData.message, "THIS IS MIN");
  myData.trashPercent = trashPercent;

  esp_err_t jirat_result = esp_now_send(jiratAddress, (uint8_t *)&myData, sizeof(myData));

  if (jirat_result == ESP_OK)
  {
    Serial.println("jirat: Sent with success");
  }
  else
  {
    Serial.println("jirat: Error sending the data");
  }

  esp_err_t nat_result = esp_now_send(natAddress, (uint8_t *)&myData, sizeof(myData));

  if (nat_result == ESP_OK)
  {
    Serial.println("nat: Sent with success");
  }
  else
  {
    Serial.println("nat: Error sending the data");
  }
}
#pragma endregion ESPNow

#pragma region UltraSonic
const int trigPin = 5;
const int echoPin = 18;

// define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
float distanceCm;

void setupUltraSonic()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loopUltraSonic()
{
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distanceCm = duration * SOUND_SPEED / 2;

  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
}

#pragma endregion UltraSonic

#pragma region Trash

int maxDistance = 60;
int minDistance = 15;

void calculationTrash()
{
  // percent
  int tmpTrashPercent = map(distanceCm, minDistance, maxDistance, 100, 0);
  // Open trash
  if (distanceCm < 15)
  {
    // trashPercent = tmpTrashPercent;
  }
  else
  {
    if (tmpTrashPercent < 0)
    {
      trashPercent = 0;
    }
    else if (tmpTrashPercent > 100)
    {
      trashPercent = 100;
    }
    else
    {
      trashPercent = tmpTrashPercent;
    }
  }
  Serial.print("Trash Percent: ");
  Serial.println(trashPercent);
}

#pragma endregion Trash

void setup()
{
  // Init Serial Monitor
  Serial.begin(9600);
  setupESPNow();
  setupUltraSonic();
}

void loop()
{
  loopUltraSonic();
  sendData();
  calculationTrash();
  delay(100);
}