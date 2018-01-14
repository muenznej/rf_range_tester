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
} test_package;

byte zize = sizeof(test_package);
byte buf[sizeof(test_package)] = {0};

#define RH_BUF_LEN sizeof(test_package)
#define RH_ASK_MAX_MESSAGE_LEN RH_BUF_LEN
RH_ASK driver(RH_SPEED[IDX_RH_SPEED], RX_PIN, TX_PIN);

unsigned short prev_ID = 0;
boolean doStart = false;
unsigned long cnt_success = 0;
unsigned long cnt_successes_total = 0;
unsigned long cnt_total = 0;
float total_err_rate = 0;
float err_rate = 0;
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

void loop()
{

  digitalWrite(RECV_LED, LOW);
  if (driver.recv(buf, &zize)) {
    digitalWrite(RECV_LED, HIGH);
    //driver.printBuffer("Received:", buf, zize); // raw data
    memcpy(&test_package, buf, zize );
    if (test_package.counter == 1)  {
      doStart = true;
    }
    if (doStart) {
      cnt_total++;
      if (prev_ID != test_package.ID) { // new package
        Serial.print("Successes: ");
        Serial.print(cnt_success);
        Serial.println("");

        Serial.print("Failures: ");
        Serial.print(MAX_ITER[test_package.IDX_MAX_ITER] - cnt_success, DEC);
        Serial.println("");

        Serial.print("Error Rate: ");
        err_rate = 100 * (1.0 - cnt_success / (float)MAX_ITER[test_package.IDX_MAX_ITER]);
        Serial.print(err_rate, DEC);
        Serial.println("");

        cnt_successes_total += cnt_success;
        Serial.print("Error Rate: ");
        total_err_rate = 100 * (1.0 - cnt_successes_total / (float)cnt_total);
        Serial.print(total_err_rate, DEC);
        Serial.println("");

        cnt_success = 0;
      } else {
        cnt_success++;
      }
      Serial.print("Package: ");
      Serial.print(test_package.ID, DEC);
      Serial.print(" @");
      Serial.print(RH_SPEED[test_package.IDX_RH_SPEED]);
      Serial.print(" ");
      Serial.print(test_package.counter, DEC);
      Serial.print("/");
      Serial.print(MAX_ITER[test_package.IDX_MAX_ITER]);
      Serial.println("");
      displaydata();
    }
    prev_ID = test_package.ID;
  }
}

void displaydata() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1.5);

  display.setCursor(0, 5);
  display.print("Cur ErrR:");
  display.print(err_rate);
  display.println("%");

  display.setCursor(0, 20);
  display.print("All ErrR:");
  display.print(total_err_rate);
  display.println("%");
  display.display();
}
