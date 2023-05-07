//        RemoteXY include library          //
// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__ESP8266_HARDSERIAL_POINT

#include <RemoteXY.h>
#include <LiquidCrystal.h>

// RemoteXY connection settings 
#define REMOTEXY_SERIAL Serial
#define REMOTEXY_SERIAL_SPEED 115200
#define REMOTEXY_WIFI_SSID "RemoteXY"
#define REMOTEXY_WIFI_PASSWORD "123456789"
#define REMOTEXY_SERVER_PORT 6377

// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =   // 315 bytes
  { 255,10,0,10,0,52,1,16,31,2,130,1,1,1,45,61,1,1,20,20,
  30,2,1,14,12,18,9,53,24,22,11,36,26,31,31,79,78,0,79,70,
  70,0,1,0,3,12,9,9,9,23,12,12,120,31,0,4,0,36,22,6,
  38,46,23,7,18,190,26,10,48,35,12,9,9,49,5,15,15,79,26,31,
  79,78,0,31,79,70,70,0,130,1,54,1,45,61,1,1,45,61,30,129,
  0,9,3,27,7,4,3,18,6,26,76,105,103,104,116,105,110,103,0,129,
  0,57,3,37,7,37,11,27,7,26,84,104,101,114,109,111,115,116,97,116,
  0,67,5,63,11,28,18,53,17,20,5,2,26,4,6,0,6,24,23,23,
  53,35,20,20,2,26,66,0,57,36,8,24,53,38,7,16,2,26,129,0,
  70,31,15,3,53,3,37,7,26,70,97,110,32,83,112,101,101,100,0,10,
  53,20,48,11,11,25,33,15,15,120,26,31,79,78,0,31,79,70,70,0,
  65,241,4,48,11,11,53,36,9,9,4,0,89,34,7,25,53,36,7,18,
  2,26,129,0,73,37,7,2,53,32,15,3,26,32,77,97,110,117,97,108,
  0,129,0,73,39,8,2,53,39,8,2,26,79,118,101,114,114,105,100,101,
  0,2,0,71,43,12,5,53,35,22,11,2,26,31,31,79,78,0,79,70,
  70,0,70,16,73,50,8,8,53,51,9,9,26,37,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  uint8_t switch_1; // =1 if switch ON and =0 if OFF 
  uint8_t button_1; // =1 if button pressed, else =0 
  int8_t slider_1; // =0..100 slider position 
  uint8_t pushSwitch_1; // =1 if state is ON, else =0 
  uint8_t rgb_r; // =0..255 Red color value 
  uint8_t rgb_g; // =0..255 Green color value 
  uint8_t rgb_b; // =0..255 Blue color value 
  uint8_t pushSwitch_2; // =1 if state is ON, else =0 
  int8_t slider_2; // =0..100 slider position 
  uint8_t switch_2; // =1 if switch ON and =0 if OFF 

    // output variables
  char text_1[4];  // string UTF8 end zero 
  int8_t level_1; // =0..100 level position
  uint8_t rgb_indicator_a; // =0..255 LED alpha channel 
  uint8_t rgb_indicator_r; // =0..255 LED Red brightness 
  uint8_t rgb_indicator_g; // =0..255 LED Green brightness 
  uint8_t rgb_indicator_b; // =0..255 LED Blue brightness 
  uint8_t led_1; // led state 0 .. 1 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)
//           END RemoteXY include          //

// Define Pins:
#define PIN_BUTTON_1 2
#define PIN_SLIDER_1 3
#define PIN_SWITCH_1 4
#define PIN_DCMOTOR_EN 5
#define PIN_DCMOTOR_1 A4
#define PIN_DCMOTOR_2 A5
#define PIN_PUSHSWITCH_1 7
#define PIN_RGB_R A3
#define PIN_RGB_G A2
#define PIN_RGB_B A1
#define PIN_TEMPSENSOR_1 A0

LiquidCrystal lcd(13,12,8,9,10,11);

// Global Variables:
int temperature;
int timer = millis();
int RGBArray[3] = {0,0,0};
int slider1Check = 0;
int oldFanSpeed = 0;
int fanSpeed = 0;

// Get Temperature from thermistor pin:
float getTemp()
{
  int sensorInput = analogRead(PIN_TEMPSENSOR_1);
  double thermoresist = 100000.0 * ((1024.0/sensorInput -1));
  double logofthermoresist = log(thermoresist);
  double tempK = 1/(0.001129148 + (0.000234125 + (0.0000000876741 * logofthermoresist * logofthermoresist)) * logofthermoresist);
  float tempC = tempK - 273.15;
  float tempF = (tempC * 9.0)/5.0 + 32.0;

  return tempF;
}

// Set the RGB LED Color:
void setRGB(unsigned int red, unsigned int green, unsigned int blue)
{
  // Set Color Pins:
  analogWrite(PIN_RGB_R, red);
  analogWrite(PIN_RGB_G, green);
  analogWrite(PIN_RGB_B, blue);
  // Set Indicator Color:
  RemoteXY.rgb_indicator_a = (int)map((red+blue+green),1,(255*3),1,255);
  RemoteXY.rgb_indicator_r = red;
  RemoteXY.rgb_indicator_g = green;
  RemoteXY.rgb_indicator_b = blue;
}

void setup() 
{
  // Required:
  RemoteXY_Init (); 
  
  // Set Pin Modes:
  pinMode (PIN_BUTTON_1, OUTPUT);
  pinMode (PIN_SLIDER_1, OUTPUT);
  pinMode (PIN_SWITCH_1, OUTPUT);
  pinMode (PIN_DCMOTOR_EN, OUTPUT);
  pinMode (PIN_DCMOTOR_1, OUTPUT);
  pinMode (PIN_DCMOTOR_2, OUTPUT);
  pinMode (PIN_PUSHSWITCH_1, OUTPUT);
  pinMode (PIN_RGB_R, OUTPUT);
  pinMode (PIN_RGB_G, OUTPUT);
  pinMode (PIN_RGB_B, OUTPUT);

  // LCD Initiate:
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Hello!");

  RemoteXY.level_1 = 0;

}

void loop() 
{ 
  // Required:
  RemoteXY_Handler ();

  // Endable Motor:
  digitalWrite(PIN_DCMOTOR_EN, HIGH);

  // RGB LED:
  if (RemoteXY.pushSwitch_2 == 0){setRGB(0,0,0);}
  if ((RemoteXY.rgb_r != RGBArray[0] || RemoteXY.rgb_g != RGBArray[1] || RemoteXY.rgb_b != RGBArray[2]) && RemoteXY.pushSwitch_2 == 1)
  {
    setRGB(RemoteXY.rgb_r, RemoteXY.rgb_g, RemoteXY.rgb_b);
    RGBArray[0] = RemoteXY.rgb_r;
    RGBArray[1] = RemoteXY.rgb_g;
    RGBArray[2] = RemoteXY.rgb_b;
  }

  // Lighting Slider:
  if (RemoteXY.slider_1 != slider1Check)
  {
    analogWrite(PIN_SLIDER_1, RemoteXY.slider_1 * 2.55);
    slider1Check = RemoteXY.slider_1;
  }

  // Temperature:
  // Get Current Temp:
  float tempMeasure=getTemp();
  int temp = (int)tempMeasure;
  
  // Every 5 Seconds Update The Temperature Reading and Print:
  int getTime = millis();
  if (getTime > (timer + 5000))
  {
    itoa (temp, RemoteXY.text_1, 10);
    lcd.clear();
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print((char)223);
    lcd.print("F");
    lcd.setCursor(0,1);
    lcd.print("Fan Speed: ");
    lcd.print(fanSpeed);
    lcd.print("%");
    timer=getTime;
  }

  // Check and Modify Values of Buttons and Sliders:
  digitalWrite(PIN_SWITCH_1, (RemoteXY.switch_1==0)?LOW:HIGH);
  digitalWrite(PIN_BUTTON_1, (RemoteXY.button_1==0)?LOW:HIGH);
  digitalWrite(PIN_PUSHSWITCH_1, (RemoteXY.pushSwitch_1==0)?LOW:HIGH);

  // Fan Controls:
  if (RemoteXY.switch_2==1)
  {
    RemoteXY.led_1 = 1;
    fanSpeed = RemoteXY.slider_2;
    analogWrite(PIN_DCMOTOR_1, fanSpeed*2.55);
    analogWrite(PIN_DCMOTOR_2, 0);
  } else if ((temp > 75) && (RemoteXY.switch_2==0)) {
    RemoteXY.led_1 = 0;
    fanSpeed = 75;
    analogWrite(PIN_DCMOTOR_1, fanSpeed*2.55);
    analogWrite(PIN_DCMOTOR_2, 0);    
  } else {
    RemoteXY.led_1 = 0;
    fanSpeed = 0;
    analogWrite(PIN_DCMOTOR_1, 0);
    analogWrite(PIN_DCMOTOR_2, 0);
  }
  RemoteXY.level_1 = fanSpeed;
}