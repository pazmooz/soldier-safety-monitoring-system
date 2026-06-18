#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_ADDR 0x3C
#define LED_PIN 13

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

String incomingData = "";

bool sosActive = false;

unsigned long lastBlink = 0;
bool ledState = false;

String latitude = "";
String longitude = "";

void setup() {

  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {

    while (1);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setTextSize(2);
  display.setCursor(0, 20);
  display.println("NANO");
  display.display();

  delay(2000);

  drawWaitingScreen();
}

void loop() {

  while (Serial.available()) {

    char c = Serial.read();

    if (c == '\n') {

      processMessage(incomingData);
      incomingData = "";

    } else {

      incomingData += c;
    }
  }

  if (sosActive) {

    if (millis() - lastBlink >= 500) {

      lastBlink = millis();

      ledState = !ledState;

      digitalWrite(LED_PIN, ledState);
    }

  } else {

    digitalWrite(LED_PIN, LOW);
  }
}

void processMessage(String msg) {

  msg.trim();

  Serial.print("RX: ");
  Serial.println(msg);

  if (msg.startsWith("SOS|")) {

    sosActive = true;

    int p1 = msg.indexOf('|');
    int p2 = msg.indexOf('|', p1 + 1);

    if (p1 > 0 && p2 > 0) {

      latitude = msg.substring(p1 + 1, p2);
      longitude = msg.substring(p2 + 1);
    }

    drawSOSScreen();
  }

  else if (msg.startsWith("SAFE|")) {

    sosActive = false;

    drawSafeScreen(msg);
  }
}

void drawWaitingScreen() {

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("RECEIVER READY");

  display.setCursor(0, 20);
  display.println("WAITING FOR DATA");

  display.display();
}

void drawSafeScreen(String msg) {

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("STATUS: SAFE");

  display.setCursor(0, 15);
  display.println("SYSTEM NORMAL");

  display.setCursor(0, 35);
  display.println("DATA RECEIVED");

  display.display();
}

void drawSOSScreen() {

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("!!! SOS ALERT !!!");

  display.setCursor(0, 15);
  display.println("EMERGENCY ACTIVE");

  display.setCursor(0, 32);
  display.print("LAT:");
  display.println(latitude);

  display.setCursor(0, 48);
  display.print("LNG:");
  display.println(longitude);

  display.display();
}
