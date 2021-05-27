/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#include "HX711.h"
#include "Ubidots.h"

const char* UBIDOTS_TOKEN = "BBFF-QzB2aXJOMuWwTfOqWI2a9jPA6k7iGU";  // Put here your Ubidots TOKEN
const char* WIFI_SSID = "BTHub6-G8H8";      // Put here your Wi-Fi SSID
const char* WIFI_PASS = "qrHGcqU7M46b";      // Put here your Wi-Fi password
Ubidots ubidots(UBIDOTS_TOKEN, UBI_UDP); // Uncomment to use UDP


HX711 scale;

#define calibration_factor -24.42 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define keg_weight 5000 //This value is obtained using the SparkFun_HX711_Calibration sketch


#define LOADCELL_DOUT_PIN 2
#define LOADCELL_SCK_PIN  4

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

int beerLeft = 0;       
int previousBeerLeft = 0; 

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);
  ubidots.wifiConnect("BTHub6-G8H8", "qrHGcqU7M46b");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  Serial.println(scale.get_units());
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("initialising..");
  display.display(); 
}

void loop() {
beerLeft = scale.get_units()-keg_weight;
    Serial.println("beerLeft: ");
    Serial.print(beerLeft/1000);
    Serial.print("L");
if((previousBeerLeft > beerLeft) && ((previousBeerLeft - beerLeft)>50)){
     ubidots.add("BeerLeft", ((scale.get_units()-keg_weight)/1000));
       bool bufferSent = false;
  bufferSent = ubidots.send(); 
  if (bufferSent) {
    // Do something if values were sent properly
    Serial.println("Ubidots updated");
  }
}else {
      Serial.println("No change");
}

  delay(5000);
  display.clearDisplay();
    display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("Beer left: ");
  display.setTextSize(3);
  display.setCursor(0,25);
  display.print(((scale.get_units()-keg_weight)/1000), 2);
  display.print("L");
  display.display(); 
               previousBeerLeft = beerLeft;

}
