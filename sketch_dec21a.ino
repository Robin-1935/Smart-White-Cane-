#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>
#include <NewPing.h>

#define MAX_TOP 200
#define MAX_BOT 200
#define vibrator 7
#define button  A1
#define trigTop  9
#define echoTop  8
#define trigBot 10
#define echoBot 11
#define fire    12
#define buzzer   6

const char *number1 = "01690192590";
const char *number2 = "01812734340";
const char *message = "I am in problem!";

NewPing sonar[2] = {
  NewPing(trigTop, echoTop, MAX_TOP),
  NewPing(trigBot, echoBot, MAX_BOT)
};
SoftwareSerial gsm(2, 3);
SoftwareSerial gpsSerial(5, 4);
TinyGPSPlus gps;

bool emgButt, isFire;
byte distTop, distBot;
int water;
double lati, longi;
long prevMs;

void setup() {
  Serial.begin(9600);
  gsm.begin(9600);
  gpsSerial.begin(9600);

  pinMode(button, INPUT);
  pinMode(vibrator, OUTPUT);
  pinMode(fire, INPUT);
  pinMode(buzzer, OUTPUT);

  GSMinit();
}

void loop() {
  distTop = sonar[0].ping_cm();
  distBot = sonar[1].ping_cm();
  water = analogRead(A0);
  emgButt = digitalRead(button);
  isFire = !digitalRead(fire);
  checkGPS();

  if (distTop > 0 && distTop <= 20) {
    beepBlink(3, 50, 50);
  }
  else if (distBot > 0 && distBot <= 20) {
    beepBlink(3, 100, 50);
  }
  else if (water > 500) {
    beepBlink(5, 50, 100);
  }
  else if (isFire == 1) {
    beepBlink(5, 100, 100);
  }
  else {
    digitalWrite(buzzer, 0);
  }

  if (emgButt == 1) {
    analogWrite(buzzer, 150);
    //sendSMS(number1);
    sendSMS(number2);
    digitalWrite(buzzer, 0);
  }

  if (millis() - prevMs >= 1000) {
    serialPrint();
    prevMs = millis();
  }
}

void serialPrint() {
  Serial.print((String) "TD:" + distTop + " | ");
  Serial.print((String) "BD:" + distBot + " | ");
  Serial.print((String) "LA:" + lati + " | ");
  Serial.print((String) "LO:" + longi + " | ");
  Serial.print((String) "WA:" + water + " | ");
  Serial.print((String) "FI:" + isFire + " | ");
  Serial.print((String) "BT:" + emgButt + " | ");
  Serial.println();
}

void checkGPS() {
  gpsSerial.listen();
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        lati = gps.location.lat();
        longi = gps.location.lng();
      }
    }
  }
  yield();
}

void beep(int on, int off) {
  digitalWrite(buzzer, 1);
  digitalWrite(vibrator, 1);
  delay(on);
  digitalWrite(buzzer, 0);
  digitalWrite(vibrator, 0);
  delay(off);
}

void beepBlink(byte times, int on, int off) {
  for (byte i = 0; i < times; i++) {
    beep(on, off);
  }
}

void GSMinit() {
  beepBlink(60, 100, 150);
  gsm.println("AT");
  delay(1000);
  gsm.println("ATE0");
  delay(1000);
  gsm.println("AT+CMGF=1");
  delay(1000);
  gsm.println("AT+CNMI=1,2,0,0,0");
  delay(1000);
}

void sendSMS(const char number[]) {
  gsm.print("AT+CMGF=1\r\n");
  delay(500);
  gsm.print("AT+CMGS=\"");
  delay(500);
  gsm.print(number);
  gsm.print("\"\r\n");
  delay(500);
  gsm.print(message);
  gsm.print("\nLink: https://www.google.com/maps/place/");
  gsm.print(lati, 6);
  gsm.print(",");
  gsm.print(longi, 6);
  gsm.write(0x1A);
  gsm.print("\r\n");
  delay(3500);
}
