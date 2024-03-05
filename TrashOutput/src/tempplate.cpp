// #include <WiFi.h>
// #include <HTTPClient.h>

// #include <SPI.h>
// #include <Wire.h>

// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>

// #include <esp_now.h>
// #include <string.h>

// using namespace std;

// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64

// unsigned long trackTime = 0;
// unsigned long timeInterval = 1500;

// #define OLED_RESET -1
// Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// uint8_t broadcastAddress[] = {0xA4, 0xCF, 0x12, 0x8F, 0xD1, 0x30};

// // Must match the receiver structure
// typedef struct struct_message
// {
//   int trashPercent;
//   char message[32];
// } struct_message;

// struct_message incomingReadings;

// esp_now_peer_info_t peerInfo;

// void setupConnection();

// void outputBinStatus();

// int binData = 0;

// void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);

// void setup()
// {
//   Serial.begin(9600);
//   setupConnection();

//   if (!OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C))
//   {
//     Serial.println("SSD1306 allocation failed");
//   }
//   else
//   {
//     Serial.println("ArdinoAll OLED Start Work !!!");
//   }

//   if (esp_now_init() != ESP_OK)
//   {
//     Serial.println("Error initializing ESP-NOW");
//   }
//   else
//   {
//     Serial.println("ESP-NOW Init Success");
//   }

//   memcpy(peerInfo.peer_addr, broadcastAddress, 6);
//   peerInfo.channel = 0;
//   peerInfo.encrypt = false;

//   if (esp_now_add_peer(&peerInfo) != ESP_OK)
//   {
//     Serial.println("Failed to add peer");
//     return;
//   }

//   esp_now_register_recv_cb(OnDataRecv);
// }

// void loop()
// {
//   // Send an HTTP POST request every 5 seconds
//   if ((millis() - trackTime) > timeInterval)
//   {
//     outputBinStatus();
//     trackTime = millis();
//   }
// }

// void outputBinStatus()
// {
//   String payload = String(to_string(binData).c_str());

//   OLED.clearDisplay();             // ลบภาพในหน้าจอทั้งหมด
//   OLED.setTextColor(WHITE, BLACK); // กำหนดข้อความสีขาว ฉากหลังสีดำ
//   OLED.setCursor(0, 0);            // กำหนดตำแหน่ง x,y ที่จะแสดงผล
//   OLED.setTextSize(2);             // กำหนดขนาดตัวอักษร
//   OLED.println("Bin status");
//   OLED.setTextSize(2); // กำหนดขนาดตัวอักษร
//   OLED.setCursor(50, 25);
//   String str = payload + "%";
//   OLED.println(str);

//   OLED.setTextSize(1);
//   OLED.setTextColor(BLACK, WHITE); // กลับสีข้อความกับพื้นหลัง

//   int percent = payload.toInt();
//   int battery = map(percent, 0, 100, 0, 5);

//   for (int i = 0; i <= battery; i++)
//   {
//     OLED.drawRect(0 + (i * 25), 50, 20, 10, WHITE);
//   }

//   OLED.display(); // สั่งให้จอแสดงผล
// }

// void setupConnection()
// {
//   WiFi.mode(WIFI_MODE_STA);

//   // 3C:61:05:03:9B:14
//   Serial.println(WiFi.macAddress());

//   Serial.println("\nConnecting");
// }

// void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
// {
//   memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
//   Serial.print("Message received: ");
//   Serial.println(incomingReadings.message);
//   Serial.print("TrashPercent: ");
//   Serial.println(incomingReadings.trashPercent);
//   binData = incomingReadings.trashPercent;
// }