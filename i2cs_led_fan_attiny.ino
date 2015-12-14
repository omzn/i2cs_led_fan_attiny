#include <TinyWireS.h>              // Requires fork by Rambo with onRequest support

#define LED_PIN 1
#define FAN_PIN 4

const int I2CSlaveAddress = 0x27;      // I2C Address.

byte led_retval;
int16_t led_target;
unsigned long ptime = 0;

void setup()
{
  delay(100);

  pinMode(FAN_PIN,OUTPUT);
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(FAN_PIN,LOW);
  digitalWrite(LED_PIN,LOW);

  TinyWireS.begin(I2CSlaveAddress);      // Begin I2C Communication
  TinyWireS.onReceive(led_fan);
  TinyWireS.onRequest(transmit);         // When requested, call function transmit()
}


void loop()
{
  delay(50);
  unsigned long t = millis();
  if (t - ptime > 1000) {
    if (led_target > 0) {
      if (led_retval == 255) {
        led_retval = 255;
      } else {
        led_retval++;
      }
      led_target--;
      analogWrite(LED_PIN, led_retval);
    } else if (led_target < 0) {
      if (led_retval == 0) {
        led_retval = 0;
      } else {
        led_retval--;
      }
      led_target++;
      analogWrite(LED_PIN, led_retval);
    }
    ptime = t;
  }
}  //end loop()


void led_fan(uint8_t y) {
  uint8_t x[16];
  int i = 0;
  if (y < 1) {
    return;
  }
//  while (TinyWireS.available()) {
  while (y--) {
    x[i] = TinyWireS.receive(); // receive byte as a character
    i++;
  }
  uint8_t pin = x[0] & 0x0f;
  uint8_t dim = x[0] & 0xf0;
  uint8_t value;
  if (pin == 0) {
    if (i > 1) {
      value = x[1];
      if (!dim) {
        analogWrite(LED_PIN, value);
        led_retval = value;
      } else {
        led_target = value - led_retval;
      }
    }
  } else if (pin == 1) {
    if (i > 1) {
      value = x[1];
      if (value > 0) {
        digitalWrite(FAN_PIN, HIGH);
      } else {
        digitalWrite(FAN_PIN, LOW);
      }
    }
  }
}

void transmit()
{
  TinyWireS.send(I2CSlaveAddress);                 // Send last recorded distance for current sensor
}





