/*******************************************************************
    A sample project for making a HTTP/HTTPS GET request on an ESP8266
    It will connect to the given request and print the body to
    serial monitor
    Parts:
    D1 Mini ESP8266 * - http://s.click.aliexpress.com/e/uzFUnIe
 *  * = Affilate
    If you find what I do usefuland would like to support me,
    please consider becoming a sponsor on Github
    https://github.com/sponsors/witnessmenow/
    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Tindie: https://www.tindie.com/stores/brianlough/
    Twitter: https://twitter.com/witnessmenow
 *******************************************************************/
/*Updated for personal use by Saksham Yadav
  https://github.com/sakshamssy  */
/* Get all possible data from MPU6050
   Accelerometer values are given as multiple of the gravity [1g = 9.81 m/s²]
   Gyro values are given in deg/s
   Angles are given in degrees
   Note that X and Y are tilt angles and not pitch/roll.

   License: MIT
//*/
int ENB =20; //For Enable Pin of Motor Driver
int al1 = 21; //For Motor Driver Input Pin1
int al2 = 22; //For Motor Driver Input Pin2
//#include "Wire.h"
//#include <MPU6050_light.h>
//MPU6050 mpu(Wire);

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
//------- Replace the following! ------
char ssid[] = "Galaxy M31s9DF6";         // your network SSID (name)
char password[] = "ogzn5578"; // your network key
float mindiff = 1;
float diff;
char t[] = "";
// For Non-HTTPS requests
// WiFiClient client;

// For HTTPS requests
WiFiClientSecure client;

// Just the base of the URL you want to connect to

#define TEST_HOST "api.thingspeak.com"
// OPTIONAL - The finferprint of the site you want to connect to.
#define TEST_HOST_FINGERPRINT "271892dda426c30709b97ae6c521b95b48f716e1"
// The finger print will change every few months.

void setup()
{
 
  Serial.begin(115200);

  // Connect to the WiFI
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
 pinMode(ENB, OUTPUT);
  pinMode(al1, OUTPUT); // Configure pin 10 as an Output
  pinMode(al2, OUTPUT); // Configure pin 11 as an Output
  digitalWrite(ENB, HIGH); ///for Actuator
  //--------

  // If you don't need to check the fingerprint
  // client.setInsecure();

  // If you want to check the fingerprint
  client.setFingerprint(TEST_HOST_FINGERPRINT); //for https
  //  client.setInsecure();  //for http
  //makeHTTPRequest("/apps/thinghttp/send_request?api_key=DI3UPIO33ATKCCW6");


  //MPU6050 setup
//  Wire.begin();
//
//  byte status = mpu.begin();
//  Serial.print(F("MPU6050 status: "));
//  Serial.println(status);
//  while (status != 0) { } // stop everything if could not connect to MPU6050
//
//  Serial.println(F("Calculating offsets, do not move MPU6050"));
//  delay(1000);
//  mpu.calcOffsets(true, true); // gyro and accelero
//  Serial.println("Done!\n");

}

float makeHTTPRequest(char address[])
{

  // Opening connection to server (Use 80 as port if HTTP,443 if https)
  if (!client.connect(TEST_HOST, 443))
  {
    Serial.println(F("Connection failed"));
    return 0;
  }

  // give the esp a breather
  yield();

  // Send HTTP request
  client.print(F("GET "));
  // This is the second half of a request (everything that comes after the base URL)
  client.print(address);
  // HTTP 1.0 is ideal as the response wont be chunked
  // But some API will return 1.1 regardless, so we need
  // to handle both.
  client.println(F(" HTTP/1.0"));

  //Headers
  client.print(F("Host: "));
  client.println(TEST_HOST);

  client.println(F("Cache-Control: no-cache"));

  if (client.println() == 0)
  {
    Serial.println(F("Failed to send request"));
    return 0;
  }
  //delay(100);
  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));


  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(F("Invalid response"));
    return 0;
  }

  // While the client is still availble read each
  // byte and print to the serial monitor
  int i = 0;
  while (client.available())
  {
    char c = 0;
    client.readBytes(&c, 1);
    t[i] = c;
    //Serial.print(c);
    i = i + 1;
  }
  for (int j = 0; j <= strlen(t); j++)
  {
    if (!(t[i] < 58 && t[i] > 46))
    {
      t[i] = '\0';
    }
  }
  //Serial.print('\n');
  float val;
  char str[4];

  strcpy(str, t);
  val = atof(str);
  return val;
}
//apps/thinghttp/send_request?api_key=DI3UPIO33ATKCCW6
void loop()
{
  Serial.println("New Reading:");

  //makeHTTPRequest("/apps/thinghttp/send_request?api_key=NT3TFKEXNIBK86SY");
  float m = makeHTTPRequest("/apps/thinghttp/send_request?api_key=DI3UPIO33ATKCCW6");
  Serial.println(m);//m here is the Solar Altitude
  delay(2000);
//  mpu.update();
//  float z = mpu.getAngleZ();
//  Serial.print("\tZ: "); Serial.println(mpu.getAngleZ());
float z=Serial.parseFloat();
Serial.print("\tZ: ");Serial.println(z);
  diff = m - z;

  if (abs(diff) > mindiff)
  {
    if ((diff) > 0)
    {
      digitalWrite(al1, HIGH);
      digitalWrite(al2, LOW);
      Serial.println("up");
    }
    else if ((diff) < 0)
    {
      digitalWrite(al1, LOW);
      digitalWrite(al2, HIGH);
      Serial.println("down");
    }
  }
  else
  {
    digitalWrite(al1, LOW);
    digitalWrite(al2, LOW);
    Serial.println("No need to optimise");
  }

}
