#define start_sw A1
#define lid_sw A2
#define red_led 13
#define green_led 12
#define relay 10
uint8_t temp = 15;

enum state {not_ready, heating, stopped, end_heating};
enum state heater = not_ready;

uint8_t red_light_on = 1;
unsigned long blink_timer = 0;
unsigned long timer = 0;

void blink_red(){
  if (millis() - blink_timer >= 500){
    blink_timer = millis();
    red_light_on ^= 1;
  }
  digitalWrite(red_led, red_light_on);
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
      digitalWrite(relay, 0);
      if (digitalRead(start_sw) == LOW && digitalRead(lid_sw) == HIGH)
        heater = heating;
      digitalWrite(green_led, 1);
      blink_red();
    break;
    case heating:
      digitalWrite(relay, 0);
      if (millis() - timer >= 500){
        timer = millis();
        temp++;
        Serial.println(temp);
      }
      if (temp >= 95)
        heater = end_heating;
      //if (digitalRead(start_sw) == LOW || digitalRead(lid_sw) == LOW){
        //heater = stopped;
      //}
      digitalWrite(green_led, 1);
      digitalWrite(red_led, 0);
    break;
    case stopped:
      digitalWrite(relay, 0);
      digitalWrite(green_led, 1);
      blink_red();
    break;
    case end_heating:
      digitalWrite(relay, 0);
      digitalWrite(green_led, 0);
      digitalWrite(red_led, 1);
    break;
  }
}
