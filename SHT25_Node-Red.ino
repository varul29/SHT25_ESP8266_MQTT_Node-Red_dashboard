#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Wire.h>

#define Addr 0x40

//Initilize Wifi Credentials
#define wifi_ssid "SSID"
#define wifi_password "Password"

// Initialize MQTT Server and Topics 
#define mqtt_server "iot.eclipse.org"

#define humidity_topic "humid"
#define temperature_C "Ctemp"
#define temperature_F "Ftemp"

WiFiClient espClient;
PubSubClient client;

// ***Software Interrupts*** //
unsigned long Timer = 0;
unsigned long Interval = 5000;


volatile float Ctemp,Ftemp,humid; // using volatile with variable - It tells the compiler that the value of the variable may change at any time--without any action being taken by the code the compiler finds nearby.

void setup() {
  Wire.begin(2,14);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setClient(espClient);
  }

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
      if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop()
{
  delay(300);
  Timer = millis();
  while(millis()- Timer<=Interval)// use intervels as per mentioned earlier
  {
  tempTask();
  delay(500);
  }
  
  if (!client.connected()) {
    reconnect();
  }
 
//Mentioned below directly executed in String url
  
  Serial.print("Degree C temperature:");
  Serial.println(String(Ctemp).c_str());
  client.publish(temperature_C, String(Ctemp).c_str(), true);

  Serial.print("Degree F temperature:");
  Serial.println(String(Ftemp).c_str());
  client.publish(temperature_F, String(Ftemp).c_str(), true);
  
  Serial.print("New humidity:");
  Serial.println(String(humid).c_str());
  client.publish(humidity_topic, String(humid).c_str(), true);
  client.loop();
  }
  

void tempTask()
{
  unsigned int data[2];
  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Send humidity measurement command, NO HOLD master
  Wire.write(0xF5);
  // Stop I2C transmission
  Wire.endTransmission();
  delay(500);
  
  // Request 2 bytes of data
  Wire.requestFrom(Addr, 2);

  // Read 2 bytes of data
  // humidity msb, humidity lsb
  if(Wire.available() == 2)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
  }
  
  // Convert the data
  float humidity = (((data[0] * 256.0 + data[1]) * 125.0) / 65536.0) - 6; 
  humid = humidity;
    
  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Send temperature measurement command, NO HOLD master
  Wire.write(0xF3);
  // Stop I2C transmission
  Wire.endTransmission();
  delay(500);

  // Request 2 bytes of data
  Wire.requestFrom(Addr, 2);

  // Read 2 bytes of data 1 byte = 8 bits total 16 bits 
  // temp msb, temp lsb
  if(Wire.available() == 2)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();  
  }

    // Data Shifting 
    
    float cTemp = (((data[0] * 256.0 + data[1]) * 175.72) / 65536.0) - 46.85;
    float fTemp = (cTemp * 1.8) + 32;

    // Output data to Serial Monitor
    Ctemp = cTemp;
    Ftemp = fTemp;
    delay(300);  
}
