
#define NUM_LEDS 120          
#define DI_PIN 10            
#define OFF_TIME 10          
#define CURRENT_LIMIT 2000   
#define START_FLASHES 0      

#define AUTO_BRIGHT 0        
#define MAX_BRIGHT 255       
#define MIN_BRIGHT 50      
#define BRIGHT_CONSTANT 500  
#define COEF 0.9             

int new_bright, new_bright_f;
unsigned long bright_timer, off_timer;

#define serialRate 115200  
uint8_t prefix[] = {'A', 'd', 'a'}, hi, lo, chk, i;  
#include <FastLED.h>
CRGB leds[NUM_LEDS];  
boolean led_state = true;  

void setup()
{
  FastLED.addLeds<WS2812, DI_PIN, GRB>(leds, NUM_LEDS);  
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);

  
  if (START_FLASHES) {
    LEDS.showColor(CRGB(255, 0, 0));
    delay(500);
    LEDS.showColor(CRGB(0, 255, 0));
    delay(500);
    LEDS.showColor(CRGB(0, 0, 255));
    delay(500);
    LEDS.showColor(CRGB(0, 0, 0));
  }

  Serial.begin(serialRate);
  Serial.print("Ada\n");   
}

void check_connection() {
  if (led_state) {
    if (millis() - off_timer > (OFF_TIME * 1000)) {
      led_state = false;
      FastLED.clear();
      FastLED.show();
    }
  }
}

void loop() {
  if (AUTO_BRIGHT) {                        
    if (millis() - bright_timer > 100) {    
      bright_timer = millis();              
      new_bright = map(analogRead(6), 0, BRIGHT_CONSTANT, MIN_BRIGHT, MAX_BRIGHT);  
      new_bright = constrain(new_bright, MIN_BRIGHT, MAX_BRIGHT);
      new_bright_f = new_bright_f * COEF + new_bright * (1 - COEF);
      LEDS.setBrightness(new_bright_f);     
    }
  }
  if (!led_state) led_state = true;
  off_timer = millis();  

  for (i = 0; i < sizeof prefix; ++i) {
waitLoop: while (!Serial.available()) check_connection();;
    if (prefix[i] == Serial.read()) continue;
    i = 0;
    goto waitLoop;
  }

  while (!Serial.available()) check_connection();;
  hi = Serial.read();
  while (!Serial.available()) check_connection();;
  lo = Serial.read();
  while (!Serial.available()) check_connection();;
  chk = Serial.read();
  if (chk != (hi ^ lo ^ 0x55))
  {
    i = 0;
    goto waitLoop;
  }

  memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    byte r, g, b;
   
    while (!Serial.available()) check_connection();
    r = Serial.read();
    while (!Serial.available()) check_connection();
    g = Serial.read();
    while (!Serial.available()) check_connection();
    b = Serial.read();
    leds[i].r = r;
    leds[i].g = g;
    leds[i].b = b;
  }
  FastLED.show();  
}
