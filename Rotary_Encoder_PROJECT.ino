#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//DEFINISCO ROTARY ENCODER
#define PINCLK 16
#define PINDT 17
#define PINSW 18

//DEFINISCO SERVOMOTORE
Servo myServo;

//DEFINISCO DISPLAY LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);



// Conserva stato prima posizione encoder
int firstClk;
int firstDt;

//Inizializzo variabile contatore
int count = 0;

//Inizializzo variabile posizione puntatore
int posit = 0;

//Inizializzo variabile posizione servo
int servoPosit = 0;

//Inizializzo variabile durata della pressione del bottone
unsigned long bttPressed = 0;

//PIN APERTURA
int openCode[] = {1, 2, 3, 4};

//PIN USER
int userCode[] = {0, 0, 0, 0};


//FUNZIONE CONTATORE
void rotationCounterValue() {
  // Leggi valori attuali dell'encoder
  int currClk = digitalRead(PINCLK);
  int currDt = digitalRead(PINDT);

  // Se la lettura corrente è diversa da quella prima
  // currClick == 1 per evitare il conteggio doppio
  if (currClk != firstClk && currClk == 1) {
    if (currDt == firstDt && currDt != currClk) {
      count --;
    } else {
      count ++;
    }
  }

  //Aggiorna valori
  firstClk = currClk;
  firstDt = currDt;

  noOverflow();
}

//FUNZIONE range numeri 0-9
void noOverflow() {
  if (count > 9) {
    count = 0;
  } else if (count < 0) {
    count = 9;
  }
}

//FUNZIONE PER SETTARE IN QUALI POSIZIONI SI PUò SPOSTARE IL PUNTATORE
void setPosition() {
  if (posit == 0) {
    lcd.setCursor(7, 0);
  } else if (posit == 1) {
    lcd.setCursor(9, 0);
  } else if (posit == 2) {
    lcd.setCursor(11, 0);
  } else if (posit == 3) {
    lcd.setCursor(13, 0);
  }
}

//FUNZIONE PER CAMBIARE LA POSIZIONE DEL PUNTATORE
void changePosition() {
  if (!digitalRead(PINSW)) {
    userCode[posit] = count;
    count = 0;
    posit++;
    if (posit > 3) {
      lcd.clear();
      comparePin();
      posit = 0;
    }
  }
  setPosition();
}

//FUNZIONE PER APRIRE LA PORTA

void comparePin() {
  for (int i = 0; i <= 3; i++) {
    if (userCode[i] != openCode[i] ) {
      lcd.setCursor(0, 1);
      lcd.print("THE PIN IS WRONG");
      delay(3000);
      lcd.clear();
      return;
    }
  }
  lcd.setCursor(0, 1);
  lcd.print("OPENED");
  myServo.write(servoPosit = 90);
  delay(3000);
  lcd.clear();
}

//FUNZIONE PER CHIUDERE LA PORTA
void closingDoor() {
  if (!digitalRead(PINSW)) {
    bttPressed = millis();
    while (digitalRead(PINSW) == LOW) {
      if (millis() - bttPressed >= 2000) {
        lcd.clear();
        myServo.write(servoPosit = 0);
        lcd.setCursor(0, 1);
        lcd.print("CLOSED");
        delay(3000);
        lcd.clear();
        posit = 0;
      }
    }
  }
}




void setup() {
  pinMode(PINCLK, INPUT);
  pinMode(PINDT, INPUT);
  pinMode(PINSW, INPUT_PULLUP);

  myServo.attach(19);

  //inizializzo LCD
  lcd.init();
  lcd.backlight();

  // Esegui prima lettura dei valori ENCODER
  firstClk = digitalRead(PINCLK);
  firstDt = digitalRead(PINDT);

}

void loop() {
  //LAYOUT DISPLAY LCD
  lcd.setCursor(2, 0);
  lcd.print("PIN");
  lcd.setCursor(8, 0);
  lcd.print("-");
  lcd.setCursor(10, 0);
  lcd.print("-");
  lcd.setCursor(12, 0);
  lcd.print("-");

  rotationCounterValue();
  changePosition();
  lcd.print(count);

  closingDoor();


}
