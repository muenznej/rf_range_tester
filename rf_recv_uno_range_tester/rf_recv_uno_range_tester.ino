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

const unsigned short MAX_ITER[] = {1, 10, 100, 1000};
const uint8_t IDX_MAX_ITER = 1;

struct {
  unsigned short ID = 1;
  byte IDX_RH_SPEED = 0;
  byte IDX_MAX_ITER = 0;
  unsigned short counter = 1;
} tp;

byte zize = sizeof(tp);
byte buf[sizeof(tp)] = {0};

#define RH_BUF_LEN sizeof(tp)
#define RH_ASK_MAX_MESSAGE_LEN RH_BUF_LEN
RH_ASK driver(RH_SPEED[IDX_RH_SPEED], RX_PIN, TX_PIN);


unsigned short cur_cnt = 0;
unsigned short prev_cnt = 0;
byte cnt_cur_successes = 0;
byte cnt_cur_fails = 0;
float cur_err_rate = 0.0;

unsigned short cnt_tot_successes = 0;
unsigned short cnt_tot_fails = 0;
float tot_err_rate = 0.0;

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
byte avg_cnt =0;
void loop()
{

  digitalWrite(RECV_LED, LOW);
  if (driver.recv(buf, &zize)) {
    avg_cnt++;
    if (avg_cnt > MAX_ITER[IDX_MAX_ITER]) {
      avg_cnt = 1;
      cnt_cur_successes = 0;
      cnt_cur_fails = 0;
    }
    digitalWrite(RECV_LED, HIGH);
    //driver.printBuffer("Received:", buf, zize); // raw data
    memcpy(&tp, buf, zize );
    cur_cnt = tp.counter;
    Serial.print("Prev: ");
    Serial.print(prev_cnt);
    Serial.print(" Current: ");
    Serial.print(cur_cnt);
    Serial.print(" Diff: ");
    Serial.print(cur_cnt - prev_cnt);
    Serial.println("");

    if (cur_cnt - prev_cnt == 1) {
      cnt_cur_successes++;
      Serial.print("Successes: ");
      Serial.print(cnt_cur_successes);
      Serial.println("");
    }
    else {
      if (prev_cnt != 0) {
        cnt_cur_fails++;
        Serial.print("Fails: ");
        Serial.print(cnt_cur_fails);
        Serial.println("");
      }
    }
    if (cnt_cur_successes != 0)
      cur_err_rate = 100 * (float)cnt_cur_fails / (float)(cnt_cur_fails + cnt_cur_successes);
    Serial.print("Current ErrorRate: ");
    Serial.print(cur_err_rate);
    Serial.println("");


    cnt_tot_successes += cnt_cur_successes;
    cnt_tot_fails += cnt_cur_fails;
    Serial.print("Total Successes: ");
    Serial.print(cnt_tot_successes);
    Serial.println("");
    Serial.print("Total Fails: ");
    Serial.print(cnt_tot_fails);
    Serial.println("");
    if (cnt_tot_successes != 0)
      tot_err_rate = 100 * (float)cnt_tot_fails / (float)(cnt_tot_fails + cnt_tot_successes);
    Serial.print("Total ErrorRate: ");
    Serial.print(tot_err_rate);
    Serial.println("");

    prev_cnt = (tp.counter % 10);
  }
  displaydata();
}


void displaydata() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1.5);

  display.setCursor(0, 5);
  display.print("Cur ErrR:");
  display.print(cur_err_rate);
  display.println("%");

  display.setCursor(0, 20);
  display.print("All ErrR:");
  display.print(tot_err_rate);
  display.println("%");
  display.display();
}
