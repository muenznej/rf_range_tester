#include <Wire.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define RECV_LED 5
#include <RH_ASK.h>
#define RX_PIN 11
#define TX_PIN 12

const uint16_t RH_SPEED[] = {600, 1200, 2400};
const uint8_t IDX_RH_SPEED = 0;

struct {
  unsigned long counter = 1;
  unsigned short trans_time = 0;
} tp;

byte avg_cnt = 0;

byte zize = sizeof(tp);
byte buf[sizeof(tp)] = {0};

#define RH_BUF_LEN sizeof(tp)
#define RH_ASK_MAX_MESSAGE_LEN RH_BUF_LEN
RH_ASK driver(RH_SPEED[IDX_RH_SPEED], RX_PIN, TX_PIN);

void setup()
{
  pinMode(OLED_RESET, OUTPUT);
  digitalWrite(OLED_RESET, HIGH);
  pinMode(RECV_LED, OUTPUT);
  Serial.begin(9600);   // Debugging only
  if (!driver.init())
    Serial.println("Reciever Init failed");
  else
    Serial.println("Reciever Init successful");

  delay(50);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}
boolean isFirst = true;
unsigned short cur_cnt = 0;
unsigned short prev_cnt = 0;
unsigned short first_id = 0;
unsigned short cnt_packages = 0;
unsigned short cnt_fails = 0;
unsigned short cnt_successes = 0;
float err_rate = 0.0;

unsigned long start_time;
unsigned long stop_time;

void loop()
{
  digitalWrite(RECV_LED, LOW);
  if (driver.recv(buf, &zize)) {
    digitalWrite(RECV_LED, HIGH);
    //driver.printBuffer("Received:", buf, zize); // raw data
    memcpy(&tp, buf, zize );
    cur_cnt = tp.counter;
    if (isFirst) {
      first_id  = cur_cnt;
      prev_cnt = cur_cnt - 1;
      isFirst = false;
    }
    cnt_packages = cur_cnt - first_id + 1;
    cnt_fails += abs(cur_cnt - prev_cnt) - 1;
    cnt_successes = cnt_packages - cnt_fails;

    Serial.print("Fails: ");
    Serial.print(cnt_fails);
    Serial.println("");
    Serial.print("Current ErrorRate: ");
    if (!isFirst) {
      err_rate = 100 * (float)cnt_fails / (float)cnt_packages;
      Serial.print(err_rate);
    }
    else {
      Serial.print("N/A");
    }
    Serial.println("");

    prev_cnt = cur_cnt;
    displaydata();
  }
}

void displaydata() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1.5);

  display.setCursor(0, 0);
  display.print("Error Rate:");
  display.print(err_rate);
  display.println("%");

  display.setCursor(0, 10);
  display.print("Fails: ");
  display.print(cnt_fails);
  display.print(" / ");
  display.print(cnt_packages);
  display.display();
}
