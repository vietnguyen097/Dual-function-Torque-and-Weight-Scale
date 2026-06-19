#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include "HX711.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int LOADCELL_DOUT_PIN = 10;
const int LOADCELL_SCK_PIN = 11;
HX711 scale;
const byte ROWS = 4;
const byte COLS = 4;
int mode = 0;
String canhtaydon = "0";
float weight = 0;
char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 2, 3, 4, 5 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 9, 8, 7, 6 };

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

String chuoiSo = "";

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  float calibration_factor = 106;
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  scale.tare();
  Serial.println("Đã Reset cân về 0.");
  scale.set_scale(calibration_factor);
  lcd.clear();
}

void loop() {
  char customKey = customKeypad.getKey();

  if (customKey == '#') {
    scale.tare();
  }
  weight = scale.get_units(1);
  weight = round(weight / 5.0) * 5.0;
  if (weight > 1000) {
    Serial.print("Khối lượng: ");
    Serial.print(weight / 1000, 2);
    Serial.println("kg");
  }


  if (customKey) {
    if (customKey == 'A') {
      mode = 0;
      chuoiSo = "";
      lcd.clear();
    }
    if (customKey == 'B') {
      mode = 1;
      chuoiSo = "";
      lcd.clear();
    }
  }

  if (mode == 0) {
    lcd.setCursor(5, 0);
    lcd.print("WEIGHT  ");
    lcd.setCursor(5, 1);
    Serial.println(weight);
    if (weight < 1000) {
      lcd.print(weight, 1);
      lcd.print("g     ");
    } else {
      lcd.print(weight / 1000, 2);
      lcd.print("kg    ");
    }
  } else if (mode == 1) {
    if (customKey) {
      if (customKey == 'D') {
        if (chuoiSo != "") canhtaydon = chuoiSo;
        chuoiSo = "";
      } else if (customKey == 'C') {
        if (chuoiSo.length() > 0) chuoiSo.remove(chuoiSo.length() - 1);
      } else if (customKey == '*') {
        chuoiSo = "";
      } else if (customKey != 'A' && customKey != 'B' && customKey != '#') {
        chuoiSo += customKey;
      }
    }


    lcd.setCursor(0, 0);
    lcd.print("m:");
    if (weight < 1000) {
      lcd.print((int)weight);
      lcd.print("g   ");
    } else {
      lcd.print(weight / 1000, 1);
      lcd.print("kg  ");
    }

    lcd.setCursor(9, 0);
    lcd.print("D:");
    if (chuoiSo != "") {
      lcd.print(chuoiSo);
      lcd.print("_   ");
    } else {
      lcd.print(canhtaydon.toInt());
      lcd.print("cm  ");
    }
    lcd.setCursor(0, 1);
    lcd.print("M=");
    lcd.setCursor(2, 1);
    float d_met = canhtaydon.toInt() / 100.0;
    float m_kg = weight / 1000.0;
    float momen = d_met * m_kg * 9.81;
    lcd.print(momen, 3);
    lcd.print(" Nm      ");
    Serial.println(momen);
  }
}