#include <RH_ASK.h>

#define RX_PIN 11
#define TX_PIN 12

const uint16_t RH_SPEED[] = {600, 1200, 2400};
const uint8_t IDX_RH_SPEED = 0;

#define WAIT_TIME_CNT 0

struct {
  unsigned short counter = 1;
} test_package;

byte zize = sizeof(test_package);
byte buf[sizeof(test_package)] = {0};

#define RH_BUF_LEN sizeof(test_package)
#define RH_ASK_MAX_MESSAGE_LEN RH_BUF_LEN
RH_ASK driver(RH_SPEED[IDX_RH_SPEED], RX_PIN, TX_PIN);

unsigned long start_time;
unsigned long stop_time;

void setup()
{
  Serial.begin(9600);   // Debugging only
  if (!driver.init())
    Serial.println("Transmitter Init failed");
  else
    Serial.println("Transmitter Init successful");

  delay(50);
}


void loop()
{
  start_time = millis();
  test_package.counter++;
  memcpy(buf, &test_package, sizeof(test_package) );
  if (driver.send((uint8_t *)buf, zize)) {
    driver.waitPacketSent();

    stop_time = millis();
    Serial.print((stop_time - start_time) / 1000.0);
    Serial.print("sec");
    Serial.println("");
    _delay_ms(WAIT_TIME_CNT);
  }
}

