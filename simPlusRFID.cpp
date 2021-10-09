#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;


#include <SoftwareSerial.h>
SoftwareSerial GSMport(7, 8); // RX, TX
int char_;
//int SensorPin = 10;
int SensorState;
int SensorLastState = HIGH;

void setup() {
  delay(3000); //дадим время на инициализацию GSM модулю
//  pinMode(SensorPin, INPUT);
//  digitalWrite(SensorPin, HIGH);  //вкл. подтягивающий резистор 20ом
  Serial.begin(9600);  //скорость порта
  Serial.println("GPRS test");
  GSMport.begin(9600);
  gprs_init();

  //rfid
  SPI.begin();     // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522




  gprs_send(String(SensorState));//connect device
}

void loop() {

   if (rfid.PICC_IsNewCardPresent()){
    readRFID();
  }
  delay(100);

}
///===============================readRFID=====================================///
void readRFID(){
 
  rfid.PICC_ReadCardSerial();

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
//  printDec(rfid.uid.uidByte, rfid.uid.size); //вывод карты

  ////////////////////                                           вывод в переменную id card
    String userid;
    for (byte i = 0; i < rfid.uid.size; i++){
      userid += String(rfid.uid.uidByte[i], HEX);
    }
  ////////////////////вызываем ф-цию оледа
  gprs_send(String(userid));//try use

  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}
///===============================readRFID=====================================///
void gprs_init() {  //Процедура начальной инициализации GSM модуля
  int d = 500;
  int ATsCount = 7;
  String ATs[] = {  //массив АТ команд
    "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"",  //Установка настроек подключения
    "AT+SAPBR=3,1,\"APN\",\"internet\"",
    "AT+SAPBR=3,1,\"USER\",\"\"",
    "AT+SAPBR=3,1,\"PWD\",\"\"",
    "AT+SAPBR=1,1",  //Устанавливаем GPRS соединение
    "AT+HTTPINIT",  //Инициализация http сервиса
    "AT+HTTPPARA=\"CID\",1"  //Установка CID параметра для http сессии
  };
  int ATsDelays[] = {6, 1, 1, 1, 3, 3, 1}; //массив задержек
  Serial.println("GPRG init start");
  for (int i = 0; i < ATsCount; i++) {
    Serial.println(ATs[i]);  //посылаем в монитор порта
    GSMport.println(ATs[i]);  //посылаем в GSM модуль
    delay(d * ATsDelays[i]);
    Serial.println(ReadGSM());  //показываем ответ от GSM модуля
    delay(d);
  }
  Serial.println("GPRG init complete");
}

void gprs_send(String data) {  //Процедура отправки данных на сервер
  //отправка данных на сайт
  int d = 400;
  Serial.println("Send start");
  Serial.println("setup url");
  GSMport.println("AT+HTTPPARA=\"URL\",\"http://37.53.69.88:8001/news/?sim="+ data + "\"");
  delay(d * 2);
  Serial.println(ReadGSM());
  delay(d);
  Serial.println("GET url");
  GSMport.println("AT+HTTPACTION=0");
  delay(d * 2);
  Serial.println(ReadGSM());
  delay(d);
  Serial.println("Send done");
  //AT+HTTPREAD
  delay(d);
  GSMport.println("AT+HTTPREAD");
//TYPE_NAME(ReadGSM())
  String str = ReadGSM();
  Serial.println(ReadGSM());
  if (str.indexOf("true") >=0){
     Serial.println("fins true");
}
  
}

String ReadGSM() {  //функция чтения данных от GSM модуля
  int c;
  String v;
  while (GSMport.available()) {  //сохраняем входную строку в переменную v
    c = GSMport.read();
    v += char(c);
    if(v==8){
      Serial.println("there");
      }
    delay(10);
  }
  //
  
  return v;
}
