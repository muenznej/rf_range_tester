#include <Wire.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define RECV_LED 5

#include <RH_ASK.h>
#define RX_PIN 11
#define TX_PIN 12

struct {
  unsigned short counter = 1;
} test_package;

byte zize = sizeof(test_package);
byte buf[sizeof(test_package)] = {0};

#define RH_BUF_LEN sizeof(test_package)
#define RH_ASK_MAX_MESSAGE_LEN RH_BUF_LEN

RH_ASK driver(600, RX_PIN, TX_PIN);

unsigned short prev_cnt = 1;
unsigned long cnt_successes = 0;
unsigned long cnt_fails = 0;
unsigned short avg_cnt = 0;

byte avgstack = 0;

float err_rate = 0;
void setup()
{
  //  pinMode(OLED_RESET,OUTPUT);
  //  digitalWrite(OLED_RESET,HIGH);
  pinMode(RECV_LED, OUTPUT);
  Serial.begin(9600);   // Debugging only
  if (!driver.init())
    Serial.println("Reciever Init failed");
  else
    Serial.println("Reciever Init successful");
  delay(50);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void loop()
{
  avg_cnt++;
  digitalWrite(RECV_LED, LOW);
  if (driver.recv(buf, &zize)) {
    digitalWrite(RECV_LED, HIGH);
    //driver.printBuffer("Received:", buf, zize); // raw data
    memcpy(&test_package, buf, zize );

    if ((test_package.counter - prev_cnt) == 1) // correct package
      cnt_successes++;
    else
      cnt_fails++;

    
    prev_cnt = test_package.counter;
  }
  delay(1000);
  displaydata();
}
void displaydata() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 5);
  //    display.print("ERR:");
  //    display.print(err_rate);
  //    display.println("%");
  byte x = avgstack;
  x |= 1UL << 0;
  x = x << 1;

  display.println( x );
  Serial.println( x, BIN);
  display.display();
}
