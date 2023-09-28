#define R 5
#define G 6
#define B 9
#define BTN 0
#define PWM 3
#define SLIDER A0

int16_t rgb[] = { 0, 0, 0, 0, 0 };  //data [r,g,b, statusMode, dutycycle]

uint64_t premillis;  
int sliderData;      
int oldSlider;

void setup() {
  Serial.begin(9600);

  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  pinMode(PWM, OUTPUT);
  pinMode(SLIDER, INPUT);

  attachInterrupt(BTN, sendData, FALLING);  // set interupt and sendData to node red when BTN status High to low

  analogWrite(R, ledMap(0));  //set led off
  analogWrite(G, ledMap(0));
  analogWrite(B, ledMap(0));

  oldSlider = map(analogRead(SLIDER), 0, 1023, 0, 100);
}

void serialEvent() {
  String buffer = Serial.readStringUntil('\n');
  rgb[0] = getValue(buffer, ',', 0).toInt();
  rgb[1] = getValue(buffer, ',', 1).toInt();
  rgb[2] = getValue(buffer, ',', 2).toInt();
  rgb[3] = getValue(buffer, ',', 3).toInt();
  rgb[4] = getValue(buffer, ',', 4).toInt();
}
void loop() {
  sliderData = map(analogRead(SLIDER), 0, 1023, 0, 100);
  if(abs(sliderData - oldSlider) >= 3){
    Serial.println(sliderData);
    oldSlider = sliderData;
  }



  analogWrite(PWM, ceil(rgb[4] / 100.0 * 255));   //generate pwm

  if (rgb[3] == 1) {  //mode on ,led stuck on
    analogWrite(R, ledMap(rgb[0]));
    analogWrite(G, ledMap(rgb[1]));
    analogWrite(B, ledMap(rgb[2]));
  } else if (rgb[3] == 2) {  //mode blink, fast blink , period 400 ms
    if (millis() - premillis >= 200) {
      static bool flags = false;
      analogWrite(R, flags ? ledMap(rgb[0]) : ledMap(0));
      analogWrite(G, flags ? ledMap(rgb[1]) : ledMap(0));
      analogWrite(B, flags ? ledMap(rgb[2]) : ledMap(0));
      flags = !flags;
      premillis = millis();
    }
  } else if (rgb[3] == 3) {  //mode blink, slow blink , period 1000 ms = 1s
    if (millis() - premillis >= 500) {
      static bool flags = false;
      analogWrite(R, flags ? ledMap(rgb[0]) : ledMap(0));
      analogWrite(G, flags ? ledMap(rgb[1]) : ledMap(0));
      analogWrite(B, flags ? ledMap(rgb[2]) : ledMap(0));
      flags = !flags;
      premillis = millis();
    } 
  }
  else  {  //other off led
      analogWrite(R, ledMap(0));
      analogWrite(G, ledMap(0));
      analogWrite(B, ledMap(0));
    }
}

int ledMap(int LV) {  //Set a new min max value. rgb led active low;
  return map(LV, 0, 255, 255, 0);
}

String getValue(String data, char separator, int index) {  //split data from index and separator
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void sendData() {  //send data and check button status
  String text = "RUN TIME " + String(millis() / 1000.0) + " s";
  Serial.println(text);
}
