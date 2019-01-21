# SHT25-with-Nodered-Dashboard
Display SHT25 Temperature and Humidity sensor data in node red dashboard

A Step forward process to make special IoT hardware to work with different IoT platforms and create advanced IoT applications. 
In this BlogTut we are going to Display the readings of Temperature sensor (interface with ESP8266) in Node-Red Dashboard using MQTT node in Node-Red Flow.

## Hardware Used

### [SHT25 - Temperature and Humidity Sensor](https://store.ncd.io/product/sht25-humidity-and-temperature-sensor-%C2%B11-8rh-%C2%B10-2c-i2c-mini-module/) 

![alt text](https://github.com/varul29/SHT25_ESP8266_MQTT_Node-Red_dashboard/blob/master/SHT25_Product%20Snaps/SHT25_I2CS-600x400.png)

### [Adfruit Huzzah I2C shield](https://store.ncd.io/product/i2c-shield-for-adafruit-huzzah-esp8266-integrated-usb-and-i2c-port/)

![alt text](https://github.com/varul29/SHT25_ESP8266_MQTT_Node-Red_dashboard/blob/master/SHT25_Product%20Snaps/ESP8266I2C_1-600x400.png)

### [ESP8266](https://www.adafruit.com/product/2471)

![alt text](https://github.com/varul29/SHT25_ESP8266_MQTT_Node-Red_dashboard/blob/master/SHT25_Product%20Snaps/Huzzah.jpg)

### [I2C Cable](https://store.ncd.io/product/i%C2%B2c-cable/)

![alt text](https://github.com/varul29/SHT25_ESP8266_MQTT_Node-Red_dashboard/blob/master/SHT25_Product%20Snaps/I2C%20cable.png)

## Software Used

- NodeJs
- Node-Red
- Arduino IDE

## ESP8266 MQTT Arduino IDE

We are using Adafruit Huzzah Board to use with SHT25 I2C module(Temperature and Humidity Sensor). To avoid the manual PCB soldering and wiring connection, we are using the I2C electronic Shield to connect huzzah board and using the I2C cable to connect with Temperature and Humidity Sensor.

The Newbie can setup ESP8266 boards  in Arduino IDE from [here](https://github.com/varul29/ESP8266). 

## Esp8266 Wifi-client setup 

For Wifi Client setup we will be using ESP8266WiFi library which helps us to connect easily with WIFI routers by just mentioning the SSID password using wifi example

	#include <ESP8266WiFi.h>
	const char *wifissid = "SSID";
	const char *wifipass = "Password";
	void setup() 
	{
  	// put your setup code here, to run once:
 	Serial.begin(115200);
 
	}

	void loop() 
	{
  	// put your main code here, to run repeatedly:
	delay(1000);
	reconnectWiFi();
	}

	void reconnectWiFi()
	{
 	 WiFi.mode(WIFI_STA);
  	delay(200);
  	WiFi.begin(wifissid,wifipass);
     	while(WiFi.status()!= WL_CONNECTED){
          	Serial.print(".");
          	delay(500);
	}
 	Serial.println("Connected to: \t");
 	Serial.println(WiFi.localIP());

Thanks to the latest Azure IoT hub article we can connect the ESP devices to any wifi router any time by just entering the wifi credentials anywhere from PCs as well as from Smartphones by just opening the local server page in any web browser. 

## About SHT25 Temperature Sensor and I2C Code

The SHT25 (humidity and temperature sensor) use the high precision, calibration, linearized sensor signals in digital, I2C format. The on-board power regulation and I2C level-shifting circuits adapts the I2C voltage standard with plug and play features.
To fetch the data from I2C devices and convert them and shift them in 8 bits data as per the requirements by:

- Initialize the I2C library
	
      #include <Wire.h>

- Initialize the I2C registers of sensor module which is specified to work with 2 wire protocol.

      #define Addr 0x40 

- After Initialization, begin the I2C transmission and request for 2 bytes of data from where I2C bus will read the data from the I2C sensor module.

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

- If 2 bytes of data is available then use the mentioned below formula will help to convert the data bytes and display desired values

      float humidity = (((data[0] * 256.0 + data[1]) * 125.0) / 65536.0) - 6;  
      float cTemp = (((data[0] * 256.0 + data[1]) * 175.72) / 65536.0) - 46.85; 
      float fTemp = (cTemp * 1.8) + 32;

- Using Serial.print you will be able to read the sensor data in the serial monitor screen.

## MQTT

The Publish-Subscribe based messaging using TCP/IP protocol called MQTT(Message Queuing Telemetry Transport).specially designed for remote location communications with different internet based online servers. These kinds of Publish-Subscribe messaging pattern requires a message broker. 

## MQTT ESP8266

For Sending the temperature sensor data to Node-Red by using MQTT, we will be using PubSub Client library, available on Github. For other functions, use the API documentation for specified code manipulations. 

## Publish/Subscribe Code

For Publish/Subscribe messaging with the server, we are going to use this library with I2C code for the sensor

- Define MQTT Server and the topics you want to publish using MQTT client.

    #define mqtt_server "iot.eclipse.org”
    #define humidity_topic "humid"
    #define temperature_C "Ctemp"
    #define temperature_F "Ftemp"
    PubSubClient client;

Note: you can use any free server name to publish like mentioned above in code, you can also use “mqtt-broker.org” or any kind of “default IPs” also to create the servers. 

- Use the Reconnection loop function which helps you to reconnect to your MQTT server easily

      void reconnect() 
      {
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

- After making the connection of MQTT client with the online server or a local server, The pubsub client  will publish the message in below format, 

      void loop()
      {
    
      if (!client.connected()) {
      reconnect();
      }

      client.publish(Topic to publish, Payload(message to publish), Return value (true or false));
      client.loop();
      }

The MQTT broker will finally send the payload to the server in a mentioned format which also directly execute the data in string format.

For Installation connect the ESP8266 Data with NODE RED use the Tutorial setup from [here](https://github.com/varul29/Node-Red_MQTT_Tutorial)
