/*  Arduino LED Matrix Display - 10x MAX7219 8x8 display-modules   *
 *  Pixels 80x8 - Digital Clock - Thermometer - Ticker             * 
 *  Dev: Michalis Vasilakis - Date: 20/3/17 - Ver. 1.2             * 
 *  Time, Date, Ticker Text and brightness level can be controlled * 
 *  from bluetooth module and computer-android application         */

//Libraries
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <dht.h>
#include <SoftwareSerial.h>

//Constants
const int photoCell = A0;
dht DHT;
const int DHT_PIN = 9; // Data pin of DHT 22 (AM2302)
const int pinCS = 10; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
const int numberOfHorizontalDisplays = 10;
const int numberOfVerticalDisplays = 1;
const int wait = 30; // In milliseconds
const int spacer = 1;
const int width = 5 + spacer; // The font width is 5 pixels
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

//Variables
int chk, length, brLevel, photoCellValue, count=0;
float hum, temp;
String msg;
boolean autoBR=true;
boolean messageCompleted=false;
boolean newMessage=false;
String tickerText = "Have a nice day!!! :-)";
char incomingByte; 
String command;
unsigned long previousMillis = 0;        // will store last time LED was updated

void setup() {
  Serial.begin(9600);
  matrix.setIntensity(1); // Use a value between 0 and 15 for brightness
}

void loop() {
  initMatrix();
  unsigned long currentMillis = millis();
  //Update thermometer and photoCell variables and on starting
  if (count==0){
    chk = DHT.read11(DHT_PIN);
    //Read data and store it to variables hum and temp
    hum = DHT.humidity;
    temp= DHT.temperature;
    controlBR();
  }
  //Show content 
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    count++; //Seconds
  }
  //Then the temperature for 3sec
  else if (count>=1 && count<2){
    welcome();
  }
  //Then the humidity for 3sec
  else if (count>=2 && count<3){
    temperature();
  }
  //Finally the ticker text
  else if (count>=3 && count<4){
    humidity();   
  }
  else if (count>= 4){
     scroll(tickerText);
    count=0; //<----reset
  }
}
//Control brightness - It will run on every complete loop!
void controlBR(){
  if (autoBR){
    photoCellValue = analogRead(photoCell);
    photoCellValue = map(photoCellValue,1023,0,0,15); 
    matrix.setIntensity(photoCellValue); // Use a value between 0 and 15 for brightness
  }
  else{
    matrix.setIntensity(brLevel); // Use a value between 0 and 15 for brightness
  }
}
//Print temp and humidity
void temperature() {
    msg = "Temperature: " + String(temp) +" deg C";
    length = msg.length() * width;
    //fill with blank spaces the rest of the display area
    for (int i = 8; i>0; i--){
      msg += " ";
    }
    matrix.setCursor((numberOfHorizontalDisplays*8-length)/2,0); // Center text 
    matrix.fillScreen(LOW);
    matrix.print(msg);
    matrix.write();
    scroll(msg);
    
}

void welcome() {
    msg = "Welcome";
    length = msg.length() * width;
    //fill with blank spaces the rest of the display area
    for (int i = 8; i>0; i--){
      msg += " ";
    }
    matrix.setCursor((numberOfHorizontalDisplays*8-length)/2,0); // Center text 
    matrix.fillScreen(LOW);
    matrix.print(msg);
    matrix.write();
    scroll(msg);
    }
void humidity(){
    msg = "Humidity: " + String(hum) +" %";
    length = msg.length() * width;
    //fill with blank spaces the rest of the display area
    for (int i = 8; i>0; i--){
      msg += " ";
    }
    matrix.setCursor((numberOfHorizontalDisplays*8-length)/2,0); // Center text 
    matrix.fillScreen(LOW);
    matrix.print(msg);
    matrix.write();
    scroll(msg);
}

//Ticker Text
void scroll(String tickerText){
  for ( int i = 0 ; i < width * tickerText.length() + matrix.width() - 1 - spacer; i++ ) {

    matrix.fillScreen(LOW);

    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2; // center the text vertically

    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < tickerText.length() ) {
        matrix.drawChar(x, y, tickerText[letter], HIGH, LOW, 1);
      }

      letter--;
      x -= width;
    }

    matrix.write(); // Send bitmap to display

    delay(wait);
  }
}
//Control funcs. for display - Call them in void loop to test the LEDs
void fullOn(){
  matrix.fillScreen(HIGH);
  matrix.write();
}
void fullOff(){
  matrix.fillScreen(LOW);
  matrix.write();
}
void initMatrix(){
  matrix.setPosition(0, 9, 0); // The first display is at <0, 0>
  matrix.setPosition(1, 8, 0); // The second display is at <1, 0>
  matrix.setPosition(2, 7, 0); // The third display is at <2, 0>
  matrix.setPosition(3, 6, 0); // And the last display is at <3, 0>
  matrix.setPosition(4, 5, 0); // And the last display is at <3, 0>
  matrix.setPosition(5, 4, 0); // And the last display is at <3, 0>
  matrix.setPosition(6, 3, 0); // And the last display is at <3, 0>
  matrix.setPosition(7, 2, 0); // And the last display is at <3, 0>
  matrix.setPosition(8, 1, 0); // And the last display is at <3, 0>
  matrix.setPosition(9, 0, 0); // And the last display is at <3, 0>
  /*For rotation change the second number as:
   * 0: No rotation
   * 1: 90 deg clockwise
   * 2: 180 deg 
   * 3: 90 deg counter clockwise
    */
  matrix.setRotation(0, 1);
  matrix.setRotation(1, 1);
  matrix.setRotation(2, 1);
  matrix.setRotation(3, 1);
  matrix.setRotation(4, 1);
  matrix.setRotation(5, 1);
  matrix.setRotation(6, 1);
  matrix.setRotation(7, 1);
  matrix.setRotation(8, 1);
  matrix.setRotation(9, 1);

}
