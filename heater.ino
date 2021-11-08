#define start_sw A1
#define lid_sw A2
#define red_led 13
#define green_led 12
#define relay 10
uint8_t temp = 15;

enum state {not_ready, heating, stopped, end_heating};
enum state heater = not_ready;

uint8_t start_sw_status = 0;
uint8_t red_light_on = 1;
unsigned long blink_timer = 0;
unsigned long timer = 0;
unsigned long start_unpress_timer = 0;

void blink_red(){
  if (millis() - blink_timer >= 500){
    blink_timer = millis();
    red_light_on ^= 1;
  }
  digitalWrite(red_led, red_light_on);
}

void check_start_sw(){
  unsigned long pressTimer = 0;
  if ((digitalRead(start_sw) == LOW) && (start_sw_status == 0)){
    start_sw_status = 1;
    pressTimer = millis();
  }
  if ((digitalRead(start_sw) == HIGH) && (start_sw_status == 1) && (millis() - pressTimer > 200))
    start_sw_status = 0;
  if (start_sw_status == 2 && (millis() - start_unpress_timer > 300))
    start_sw_status = 0;
}

void setup() {
  pinMode(start_sw, INPUT);
  pinMode(lid_sw, INPUT);
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(red_led, 1);
  digitalWrite(green_led, 1);
  digitalWrite(relay, 1);
  Serial.begin(9600);
  Serial.println("Water heater:");
}

void loop() {
  switch(heater){
    case not_ready:
      if (millis() - timer >= 2000 && temp > 10){
        timer = millis();
        temp--;
        Serial.println(temp);
      } //scade pana la "temperatura camerei"
      if (start_sw_status == 1 && digitalRead(lid_sw) == HIGH){
        heater = heating;
        start_sw_status = 2;
        start_unpress_timer = millis();
      } // incepe incalzirea / trece in "heating"
      digitalWrite(relay, 1);
      digitalWrite(green_led, 1);
      blink_red();
    break;
    case heating:
      if (millis() - timer >= 500){
        timer = millis();
        temp++;
        Serial.println(temp);
      } // creste temperatura
      if (temp >= 95)
        heater = end_heating;
        // opreste incalzirea / trece in "end_heating"
      if (start_sw_status == 1 || digitalRead(lid_sw) == LOW){
        heater = stopped;
        start_sw_status = 2;
        start_unpress_timer = millis();
      } // opreste incalzirea temporar / trece in "stopped"
      digitalWrite(relay, 0);
      digitalWrite(green_led, 1);
      digitalWrite(red_led, 0);
    break;
    case stopped:
      if (start_sw_status == 1 && digitalRead(lid_sw) == HIGH){
        heater = heating;
        start_sw_status = 2;
        start_unpress_timer = millis();
      } // incepe incalzirea / trece in "heating"
      digitalWrite(relay, 1);
      digitalWrite(green_led, 1);
      blink_red();
    break;
    case end_heating:
      if (millis() - timer >= 1000){
        timer = millis();
        temp--;
        Serial.println(temp);
        if (temp < 35)
          heater = not_ready;
      } // scade temperatura pana la 35, apoi trece in "not ready"
      digitalWrite(relay, 1);
      digitalWrite(green_led, 0);
      digitalWrite(red_led, 1);
    break;
  }
  check_start_sw();
}
