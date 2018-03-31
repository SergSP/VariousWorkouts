//Прошивка для удаленного датчика температуры (2 канала) и влажности
//Применяется шифрование передаваемых по радио данных
#include <stdio.h>
#include "string.h"
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "RF24.h"
#include "printf.h"
#include "DHT.h"




#define DHTPIN 2
#define DHTTYPE DHT22
#define ARNAME "ArRoom"


RF24 radio(9, 10);
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(5);
DallasTemperature sensors(&oneWire);

const uint8_t pipes[][6] = {"1ArDt", "2ArDt"}; //адрес канала, для передачи радио сигнала. Write,Read : Write=1ArDt, Read=*ArDt
const char keyhash[33] = "GDpkyz7OjbTKunz8rJXraEEhosm0yOg6 ";

void setup() {

  Serial.begin(115200);
  Serial.println("Ok");
  printf_begin();
  delay(2000);

  dht.begin();
  sensors.begin();
  radio.begin();                          // Включение модуля;

  radio.setChannel(106);                    // Установка канала вещания;
  radio.setRetries(15, 15);               // Установка интервала и количества попыток "дозвона" до приемника;
  radio.setDataRate(RF24_1MBPS);        // Установка минимальной скорости;
  radio.setPALevel(RF24_PA_MAX);          // Установка максимальной мощности;
  radio.openWritingPipe(pipes[0]); // Открываем радио канал передачи
  radio.openReadingPipe(1, pipes[1]);
  radio.printDetails();
  radio.startListening();
  printf("Wait...\n");


}
char charT2[5];
char charT[5];
char charH[5];
char datag[10];
char datas[33];
int x;
void loop()
{
  if (radio.available()) {
    //strcpy(datag, "");
    //strcpy(datas, "");
    radio.read(&datag, 9);
    decode_m(datag);

    radio.stopListening();
    if (strcmp(datag, "get_alld") == 0) {

      sensors.requestTemperatures();
      dtostrf(dht.readTemperature(), 2, 1, charT);
      dtostrf(dht.readHumidity(), 2, 1, charH);
      dtostrf(sensors.getTempCByIndex(0), 2, 1, charT2);
      
      sprintf(datas, "%sI%sO%sH%s", ARNAME, charT, charT2, charH);
      Serial.println(datas);
      encode_m(datas);

      Serial.println(datas);
      radio.write(&datas, 32);

    }else if (strcmp(datag, "get_outt") == 0) {

      sensors.requestTemperatures();

      dtostrf(sensors.getTempCByIndex(0), 2, 1, charT2);
      
      sprintf(datas, "%s", charT2);
      Serial.println(datas);
      encode_m(datas);

      Serial.println(datas);
      radio.write(&datas, 32);

    }else if (strcmp(datag, "get_inht") == 0) {

      dtostrf(dht.readTemperature(), 2, 1, charT);
      
      sprintf(datas, "%s", charT);
      Serial.println(datas);
      encode_m(datas);

      Serial.println(datas);
      radio.write(&datas, 32);

    }
    radio.startListening();
  }
}

void encode_m(char str[]) {
  for (int i = 0; i < 32; i++)
  {
    str[i] = ((str[i] + 7) ^ keyhash[i]) - 94;
  }
}
void decode_m(char str[]) {
  for (int i = 0; i < 8; i++)
  {
    str[i] = ((str[i] + 2) ^ keyhash[i]) - 12;
  }
}
