/*
 * arduino-mini-pro-temp-hum-sensor.ino - Firmware for Arduino Mini Pro (3.3V 8MHz) based temperature and humidity sensor Node with nRF24L01+ module
 *
 * Copyright 2015 Tomas Hozza <thozza@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Tomas Hozza <thozza@gmail.com>
 *
 * MySensors library - http://www.mysensors.org/
 * DHT library - https://github.com/markruys/arduino-DHT
 * OneWire library - http://playground.arduino.cc/Learning/OneWire
 * Library for Dallas temperature ICs - https://github.com/milesburton/Arduino-Temperature-Control-Library
 *
 * Arduino Pro Mini - http://www.arduino.cc/en/Main/ArduinoBoardProMini 
 * nRF24L01+ spec - https://www.sparkfun.com/datasheets/Wireless/Nordic/nRF24L01P_Product_Specification_1_0.pdf
 * DHT22 spec - https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf
 * DS18B20 spec - http://datasheets.maximintegrated.com/en/ds/DS18B20.pdf
 */

#include <MyTransportNRF24.h>
#include <MyHwATMega328.h>
#include <MyMessage.h>
#include <MySensor.h>
#include <SPI.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DEBUG 1
#define SLEEP_TIME 5000
#define NODE_ID 1

#define SENSOR_DHT22_HUM 0
#define SENSOR_DHT22_TEMP 1
#define SENSOR_DS18B20_TEMP 2

#define SKETCH_NAME "arduino-mini-pro-temp-hum-sensor"
#define SKETCH_VERSION "0.1"

/***********************************/
/********* PIN DEFINITIONS *********/
/***********************************/
#define RX_LED_pin 7
#define TX_LED_pin 6
#define ERR_LED_pin 5

#define RF24_CE_pin 9
#define RF24_CS_pin 10

#define DHT22_pin 3
#define ONE_WIRE_BUS_pin 8

/*****************************/
/********* FUNCTIONS *********/
/*****************************/
float readDS18B20Temp();
float readDHT22Hum();
float readDHT22Temp();

uint16_t readVcc();
uint8_t getVccLevel();

/************************************/
/********* GLOBAL VARIABLES *********/
/************************************/
MyHwATMega328 hw;
MyTransportNRF24 transport(RF24_CE_pin, RF24_CS_pin);

#ifdef WITH_LEDS_BLINKING
MySensor node(transport, hw, RX_LED_pin, TX_LED_pin, ERR_LED_pin);
#else
MySensor node(transport, hw);
#endif

MyMessage msgDHT22Hum(SENSOR_DHT22_HUM, S_HUM);
MyMessage msgDHT22Temp(SENSOR_DHT22_TEMP, S_TEMP);
MyMessage msgDS18B20Temp(SENSOR_DS18B20_TEMP, S_TEMP);

DHT dht22;
OneWire oneWire(ONE_WIRE_BUS_pin);
DallasTemperature ds18b20_sensor(&oneWire);

/**********************************/
/********* IMPLEMENTATION *********/
/**********************************/
void setup() {
  node.begin(NULL /* inc msg callback */, NODE_ID);
  dht22.setup(DHT22_pin);
  ds18b20_sensor.begin();

  node.sendSketchInfo(SKETCH_NAME, SKETCH_VERSION);

  node.present(SENSOR_DHT22_HUM, S_HUM);
  node.present(SENSOR_DHT22_TEMP, S_TEMP);
  node.present(SENSOR_DS18B20_TEMP, S_TEMP);
}

void loop() {
//  node.sendBatteryLevel(getVccLevel());
  
  node.send(msgDHT22Hum.set(readDHT22Hum(), 2));
  node.send(msgDHT22Temp.set(readDHT22Temp(), 2));
  node.send(msgDS18B20Temp.set(readDS18B20Temp(), 2));

  node.sleep(SLEEP_TIME);
}

/**
 * Read the humidity from DHT22
 */
float readDHT22Hum() {
  float hum = dht22.getHumidity();

  #if DEBUG
  Serial.print(F("Read Hum from DHT22 = "));
  Serial.println(hum);
  #endif

  return hum;
}

/**
 * Read the temperature from DHT22
 */
float readDHT22Temp() {
  float temp = dht22.getTemperature();

  #if DEBUG
  Serial.print(F("Read Temp from DHT22 = "));
  Serial.println(temp);
  #endif

  return temp;
}

/*
 * Read temperature from the only one DS18B20 sensor
 */
float readDS18B20Temp() {
  float temp;

  ds18b20_sensor.requestTemperatures();
  temp = ds18b20_sensor.getTempCByIndex(0);

  #if DEBUG
  if (temp == DEVICE_DISCONNECTED_C) {
    Serial.println(F("No DS18B20 connected!"));
  }
  else {
    Serial.print(F("Read Temp from DS18B20 = "));
    Serial.println(temp);
  }
  #endif

  return temp;
}
