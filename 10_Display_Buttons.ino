#include <SoftwareSerial.h>

#define hot_valve 6         // Пин управления клапаном горячей воды 24V
#define normal_valve 7      // Пин управления клапаном нормальной воды 24V
#define cold_valve 8        // Пин управления клапаном холодной воды 24V
#define sparkling_valve 16  // A2 Пин управления клапаном газированой воды 220V

byte Buffer[20];  //Создать буфер
byte Buffer_Len = 0;
bool flag = false;  // флаг чтения данных с монитора

#define DEBUG 1  // Отладка: 1 - вкл.  0 - выкл.
int couterTIMER = 0;

//SoftwareSerial mySerial(A4, A5);

void setup() {
  // put your setup code here, to run once:
  //mySerial.begin(115200);
  Serial.begin(115200);
  Serial.println("Start program");

  pinMode(hot_valve, OUTPUT);
  pinMode(normal_valve, OUTPUT);
  pinMode(cold_valve, OUTPUT);
  pinMode(sparkling_valve, OUTPUT);

  digitalWrite(sparkling_valve, LOW);
  digitalWrite(hot_valve, LOW);
  digitalWrite(normal_valve, LOW);
  digitalWrite(cold_valve, LOW);



  // инициализация Timer1
  cli();       // отключить глобальные прерывания
  TCCR1A = 0;  // установить регистры в 0
  TCCR1B = 0;

  //OCR1A = 15624; // установка регистра совпадения
  OCR1A = 1550;  // установка регистра совпадения, примерно раз в 200 ms попадаем в обработки прерывания

  TCCR1B |= (1 << WGM12);  // включение в CTC режим

  // Установка битов CS10 и CS12 на коэффициент деления 1024
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);

  TIMSK1 |= (1 << OCIE1A);  // включение прерываний по совпадению
  sei();                    // включить глобальные прерывания
}

void loop() {

  if (Serial.available()) {
    Buffer[Buffer_Len] = Serial.read();
      //if (DEBUG)
      //{
       //Serial.print(Buffer[Buffer_Len], HEX);
       //Serial.print(" ");
     // }

    Buffer_Len++;
    if (Buffer_Len == 9) {
      flag = true;
    }
  }

  else if (flag) {
    if (Buffer[0] == 0X5A) {

      if (Buffer[4] == 0X50) {

        if (Buffer[8] == 0X10) {
          digitalWrite(normal_valve, HIGH);
          
          couterTIMER +=2;
          Serial.println(1);
        } else if (Buffer[8] == 0X20) {
          digitalWrite(cold_valve, HIGH);
           Serial.println(2);
          couterTIMER +=2;
        } else if (Buffer[8] == 0X40) {
          digitalWrite(sparkling_valve, HIGH);
          //digitalWrite(hot_valve, LOW);
          couterTIMER +=2;
          Serial.println(3);
        } else if (Buffer[8] == 0X80) {
          digitalWrite(hot_valve, HIGH);
          couterTIMER +=2;
          Serial.println(4);
        }
      }
    }
    Buffer_Len = 0;  // сброс номера элемента в массиве
    flag = false;
  }
}


ISR(TIMER1_COMPA_vect) {
  if (couterTIMER && flag == false) {
    if(couterTIMER > 3)
      {
        couterTIMER -=3;
      }
      else
      {
       couterTIMER--; 
      }
    if (couterTIMER == 0) {
      //couterTIMER = 1;
      digitalWrite(sparkling_valve, LOW);
      digitalWrite(hot_valve, LOW);
      digitalWrite(normal_valve, LOW);
      digitalWrite(cold_valve, LOW);
      Serial.println(0);
    }
  }
}