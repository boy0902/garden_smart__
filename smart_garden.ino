int CBDoAm = A0;
float Doamdat; //Variable soil moisture
#define motor D5
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define DHTPIN 2
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;
LiquidCrystal_I2C lcd(0x27,16,2);

char auth[] = "F79tnVuL1ibFiH3pBhw9HgE4AMWATYde";
char ssid[] = "Buivandong";
char pass[] = "0865457580";

WiFiClient client;

// ThingSpeak Settings
const int channelID = 1706643;
String writeAPIKey = "3F5Z5ZABL2MXZ10I";
const char* server = "api.thingspeak.com";
const int postingInterval = 1000; // post data every 1 seconds

byte degree[8] = { //Declaration of binary code for LCD screen, displaying degrees Celsius.
0B01110,
0B01010,
0B01110,
0B0000,
0B0000,
0B0000,
0B0000,
0B00000
};

void sendSensor()
{
delay(postingInterval);
float h = dht.readHumidity();
float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
Doamdat=analogRead(CBDoAm);
Doamdat=map(Doamdat,0,1023,100,0);
  
if (isnan(h) || isnan(t)) {
Serial.println("Failed to read from DHT sensor!");
return;
}
else{
Serial.print("Due to: ");
Serial.println(t); //Output temperature
Serial.print("Do am: ");
Serial.println(h); //Export humidity
Serial.println(); //Down the line
}
if (client.connect(server, 80)) {
// Construct API request body
String body = "field1=" + String(t, 1) + "&field2=" + String(h, 1)+ "&field3=" + String(Doamdat, 2);

client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: " + writeAPIKey + "\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(body.length());
client.print("\n\n");
client.print(body);
client.print("\n\n");
                
}
client.stop();
 
lcd.setCursor(2,0); //Place the cursor to row 0, column 2
lcd.print(round(t)); //Output temperature
lcd.print(" ");
lcd.write(1);
lcd.print("C");
lcd.setCursor(10,0); //Put the cursor to row 0, column 10
lcd.print(round(h)); //Export humidity
lcd.print(" %");
 
Blynk.virtualWrite(V5, h);
Blynk.virtualWrite(V6, t);
}

void setup()
{
// Debug console
Serial.begin(9600);
pinMode(motor,OUTPUT);
// pinMode(den,OUTPUT);
lcd.begin(); //Start screen. Start allowing the Arduino to use the display.
lcd.backlight(); //Turn on the backlight
lcd.print("T: ");
lcd.setCursor(8,0);
lcd.print("H: ");
lcd.setCursor(0,1);
lcd.print("Do am data: ");
lcd.createChar(1, degree);
  
Blynk.begin(auth, ssid, pass);
// You can also specify server:
//Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
//Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);

dht.begin();
WiFi.begin(ssid, pass);
while (WiFi.status() != WL_CONNECTED) {
Serial.print(".");
delay(100);
}

// Setup a function to be called every second
timer.setInterval(1000L, sendSensor);
}
void motorState(int State)
{
digitalWrite(5,State);
}
void loop()
{
Blynk.run();
timer.run();
  
Doamdat=analogRead(CBDoAm);
Doamdat=map(Doamdat,0,1023,100,0);
  
lcd.setCursor(10,1); //Put the cursor to row 1, column 11
lcd.print(Doamdat); //Export soil moisture
lcd.print("%");

//Pump water when the soil is dry
if(Doamdat<60)
{
//digitalWrite(den,HIGH); //Bat den led
//lcd.print("severe country");
Serial.println("Bad country, old age");
digitalWrite(motor,HIGH);
      
}
//Stop pumping when the soil is wet
else
{
//digitalWrite(den,LOW);
//lcd.print("courtesy of the country");
Serial.println("Severe country");
digitalWrite(motor,LOW);
        
}
Blynk.virtualWrite(V7,Doamdat);
delay(1000);
}
