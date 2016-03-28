/*
 * LED and Fan controller
 *
 * I2C write:
 *     pin # (D000000X), value (0 - 255), dim interval (0-255)[sec]
 *            |      +- pin # (0,1)
 *            +-------- dim flag (for pin 0)
 * I2C read:
 *     returns the value of pin #
 */

#include <TinyWireS.h>              // Requires fork by Rambo with onRequest support

#define LED_PIN (1)
#define FAN_PIN (4)

#define SLAVE_ADDRESS  (0x27)

uint8_t led_retval;
uint8_t fan_retval;
int16_t led_target;
uint32_t dim_interval = 1000;
unsigned long ptime = 0;

uint8_t pin;


void setup()
{
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  TinyWireS.begin(SLAVE_ADDRESS);      // Begin I2C Communication
  TinyWireS.onReceive(led_fan);
  TinyWireS.onRequest(transmit);         // When requested, call function transmit()
}


void loop()
{
  //delay(50);
  unsigned long t = millis();
  if (t - ptime > dim_interval || t < ptime) {
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
  while (y--) {
    x[i] = TinyWireS.receive(); // receive byte as a character
    i++;
  }
  pin = x[0] & 0x0f;
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
      if (i > 2) {
        dim_interval = (uint32_t)x[2] * 1000;
      }
    }
  } else if (pin == 1) {
    if (i > 1) {
      value = x[1];
      if (value > 0) {
        analogWrite(FAN_PIN, value);
        fan_retval = value;
        //digitalWrite(FAN_PIN, HIGH);
      } else {
        digitalWrite(FAN_PIN, LOW);
        fan_retval = 0;
      }
    }
  }
}

void transmit()
{
  if (pin == 0) {
    TinyWireS.send(led_retval);
  } else if (pin == 1) {
    TinyWireS.send(fan_retval);    
  }
}





