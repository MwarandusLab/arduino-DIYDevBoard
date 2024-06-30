#include <Wire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

SoftwareSerial bluetooth(8, 7); // TX, RX  Bluetooth Module
SoftwareSerial sim(6, 5); // TX, RX GSM Module

LiquidCrystal_I2C lcd(0x3F, 16, 2);

//Fan
int Relay = 9; // Fan is connected to Digital PIN 9

//LED
int WhiteLED = 13;
int YellowLED = 12;
int RedLED = 11; // PIN 11 is GPIOO of Wifi Module (Should Be connected to Ground when Programming the WIFI Module)
int GreenLED = 10; // PIN 10 is GPIO2 of the Wifi Module 

int Sms = 0;
int Call = 0;

int Data_pin = 2;
int Clock_Pin = 4;
int CLR = 3;

bool SendSms = false;

int _timeout;
String _buffer;
String number = "+254700090939"; //-> change with your number

int receivedValue;

int digits[10][8]{
  {1,1,1,1,1,1,0,0}, //digit 0
  {0,1,1,0,0,0,0,0}, //digit 1
  {1,1,0,1,1,0,1,0}, //digit 2
  {1,1,1,1,0,0,1,0}, //digit 3
  {0,1,1,0,0,1,1,0}, //digit 4
  {1,0,1,1,0,1,1,0}, //digit 5
  {1,0,1,1,1,1,1,0}, //digit 6
  {1,1,1,0,0,0,0,0}, //digit 7
  {1,1,1,1,1,1,1,0}, //digit 8
  {1,1,1,1,0,1,1,0}, //digit 9
};


void setup() {

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  _buffer.reserve(50);
  
  
  pinMode(Relay, OUTPUT);

  pinMode(WhiteLED, OUTPUT);
  pinMode(YellowLED, OUTPUT);
  pinMode(RedLED, OUTPUT);
  pinMode(GreenLED, OUTPUT);

  pinMode(Data_pin, OUTPUT);
  pinMode(Clock_Pin, OUTPUT);
  pinMode(CLR, OUTPUT);

  Serial.println("System Started...");
  delay(1000);

  digitalWrite(WhiteLED, LOW);
  digitalWrite(RedLED, LOW);
  digitalWrite(YellowLED, LOW);
  digitalWrite(GreenLED, LOW);

  digitalWrite(Relay, LOW);

  digitalWrite(Data_pin, LOW);
  digitalWrite(Clock_Pin, LOW);
  digitalWrite(CLR, LOW);

  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("WELCOME TO");
  lcd.setCursor(1,1);
  lcd.print("DIY DEV BOARD");
  delay(2000);  


}

void loop() {
  bluetooth.begin(9600);  
  if(bluetooth.available() > 0) {
    receivedValue = bluetooth.read();
    Serial.println(receivedValue);
    if (receivedValue == '1') {
      digitalWrite(Relay, HIGH); // Turn on Fan
      digitalWrite(WhiteLED, HIGH);
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("DIY DEV BOARD");
      lcd.setCursor(0,1);
      lcd.print("FAN STATUS: ON");
    }else if (receivedValue == '2') {
      digitalWrite(Relay, LOW); // Turn off Fan
      digitalWrite(WhiteLED, LOW);
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("DIY DEV BOARD");
      lcd.setCursor(0,1);
      lcd.print("FAN STATUS: OFF");
    }else if (receivedValue == '3') {
      digitalWrite(RedLED, HIGH); 
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("DIY DEV BOARD");
      lcd.setCursor(0,1);
      lcd.print("OUTGOING CALL...");
      if(Call == 0){
        sim.begin(9600);
        callNumber();
        sim.end();
        Call = 1;
      }else if(Call == 1){
        Call = 0;
      }
    }else if (receivedValue == '4') {
      SendSms = true;
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("DIY DEV BOARD");
      lcd.setCursor(0,1);
      lcd.print("SENDING SMS...");
      if(Sms == 0){
        sim.begin(9600);
        SendMessage();
        sim.end();
        Sms = 1;
      }else if(Sms == 1){
        Sms = 0;
      }
    }else if (receivedValue == '5') {
      digitalWrite(GreenLED, HIGH); // Enable Wifi Access Point
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("DIY DEV BOARD");
      lcd.setCursor(0,1);
      lcd.print("Wi-Fi: ENABLED");
    }else if (receivedValue == '6') {
      digitalWrite(GreenLED, LOW); // Disable Wifi Access Point
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("DIY DEV BOARD");
      lcd.setCursor(0,1);
      lcd.print("Wi-Fi: DISABLED");
    }else if (receivedValue == '7') {
      digitalWrite(CLR, HIGH); // Turn ON Shift Register Output
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("DIY DEV BOARD");
      lcd.setCursor(0,1);
      lcd.print("REGISTER: ON");
    }else if (receivedValue == '8') {
      digitalWrite(CLR, LOW); // Turn OFF Shift Register Output
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("DIY DEV BOARD");
      lcd.setCursor(0,1);
      lcd.print("REGISTER: OFF");
    }
  }
  if(digitalRead(CLR) == HIGH){
    digitalWrite(YellowLED, HIGH);
    for(int i = 0; i < 10; i++){
        DisplayDigit(i);
        delay(100);
    }
  }else if(digitalRead(CLR) == LOW){
    digitalWrite(YellowLED, LOW);
  }

}

void SendMessage(){
  //Serial.println ("Sending Message");
  sim.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(200);
  //Serial.println ("Set SMS Number");
  sim.println("AT+CMGS=\"" + number + "\"\r"); //Mobile phone number to send message
  delay(200);
  String SMS = "Welcome to DIY Development Board";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);// ASCII code of CTRL+Z
  delay(200);
  _buffer = _readSerial();
}
void callNumber() {
  sim.print (F("ATD"));
  sim.print (number);
  sim.print (F(";\r\n"));
  _buffer = _readSerial();
  Serial.println(_buffer);
}
String _readSerial() {
  _timeout = 0;
  while  (!sim.available() && _timeout < 12000  )
  {
    delay(13);
    _timeout++;
  }
  if (sim.available()) {
    return sim.readString();
  }
}
void DisplayDigit(int Digit){
  for(int i = 7; i >= 0; i--){
    digitalWrite(Clock_Pin, LOW);
    digitalWrite(Data_pin,!digits[Digit][i]);
    digitalWrite(Clock_Pin, HIGH);
    delay(100);
  }
}