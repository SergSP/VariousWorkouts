//Прошивка для компактного переносного дисплея (питание от батарейки) для отображения температуры и влажности с удаленного датчика
//Применяется шифрование передаваемых по радио данных
#include <stdio.h>
#include <SPI.h>
#include "TM1637.h"
#include "RF24.h"
#include "printf.h"

#define CLK 3//Pins for TM1637       
#define DIO 4
#define ARNAME "ArDisply"

TM1637 tm1637(CLK, DIO);



RF24 radio(9, 10);

const uint8_t pipes[][6] = {"1ArDt", "2ArDt"}; //адрес канала, для передачи радио сигнала. Read,Write : //Read,Write : Read=1ArDt
const char keyhash[33] = "GDpkyz7OjbTKunz8rJXraEEhosm0yOg6";
char charT2[5];
char charT[5];
char charH[5];
char datag[33];
char datas[9];
float temp;
void setup() {

  Serial.begin(115200);
  Serial.println("Load...");
  printf_begin();

  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);

  delay(2000);

  radio.begin();                          // Включение модуля;

  radio.setChannel(106);                    // Установка канала вещания;
  radio.setRetries(15, 15);               // Установка интервала и количества попыток "дозвона" до приемника;
  radio.setDataRate(RF24_1MBPS);        // Установка минимальной скорости;
  radio.setPALevel(RF24_PA_MAX);          // Установка максимальной мощности;
  radio.openWritingPipe(pipes[1]); // Открываем радио канал передачи
  radio.openReadingPipe(1, pipes[0]);
  radio.printDetails();
  radio.startListening();
  printf("Wait...\n");
  strcpy(datas, "get_outt");
  encode_m(datas);

}


void loop()
{
  
  radio.stopListening();
  Serial.println(datas);
  radio.flush_tx ();
  radio.write( &datas, 8 );

  radio.startListening();

  unsigned long started_waiting_at = millis();
  bool timeout = false;
  while ( ! radio.available() && ! timeout ) {
    if (millis() - started_waiting_at > 2000 )
      timeout = true;
  }


  if ( timeout )
  {
    printf("Timed out.\n");
  }
  else
  {
    radio.read( &datag, 32 );

    decode_m(datag);
    temp=atof(datag);
    int t1=temp/10;
    int t2=temp;
    t2=t2 % 10;
    int t3=(temp*100);
    t3=t3 % 100;

    tm1637.clearDisplay();
    tm1637.point(POINT_ON);
    tm1637.display(0,t1);
    tm1637.display(1,t2); 

    tm1637.display(2,t3/10);
    tm1637.display(3,12);
    delay(15000);
  }

  
}

void encode_m(char str[]) {
  for (int i = 0; i < 8; i++)
  {
    str[i] = ((str[i] + 12) ^ keyhash[i]) - 2;
  }
}
void decode_m(char str[]) {
  for (int i = 0; i < 32; i++)
  {
    str[i] = ((str[i] + 94) ^ keyhash[i]) - 7;
  }
}
