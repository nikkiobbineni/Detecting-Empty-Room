#include <Arduino.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ThingSpeak.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include "HTTPClient.h"
#include "time.h"
#include <ArduinoJson.h>
int visitors = 0;
int sensor = 19;
int sensor1 = 4;
int buzzer = 23;
int ledenter=22;
int ledstop=5;
//WebServer server(80);
WiFiServer server(80);
void send_event(const char *event);
// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";


// Assign output variables to GPIO pins
const int output26 = 15;


// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

#define WIFI_SSID "Galaxy"
#define WIFI_PASSWORD "12345678"
WiFiClient WIFI_CLIENT;

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <title>Live Update</title>
    <meta charset="UTF-8">
</head>
<style>
#liveData{
  position: absolute;
    left: 80px;
    top: 60px;
    font-color: white;
    font-size: 50px;
    font-family: sans-serif;
 
}


form { 
    color: white;
    font-size: 25px;
    font-family: sans-serif;
    position: absolute;
    left: 70px;
    top: 40px;
    width: 400px;
    height: 700px;
}
</style>
<body>

    <img src="https://static.vecteezy.com/system/resources/previews/006/699/629/non_2x/internet-of-thing-cloud-digital-technology-design-iot-world-security-and-data-information-internet-log-in-connecting-finger-scanner-free-vector.jpg" 
     style="max-width: 100%;max-height:100%" />

   <form>
 
             
   <p id="liveData">
   </p>

    
                
                <script>
  window.addEventListener('load', function()
{
    var xhr = null;

    getXmlHttpRequestObject = function()
    {
        if(!xhr)
        {               
            // Create a new XMLHttpRequest object 
            xhr = new XMLHttpRequest();
        }
        return xhr;
    };

    updateLiveData = function()
    {
               
        xhr = getXmlHttpRequestObject();
        xhr.onreadystatechange = evenHandler;
        // asynchronous requests
        xhr.open("GET","read_data", true);
        // Send the request over the network
        xhr.send();
    };

    updateLiveData();

    function evenHandler()
    {
        // Check response is ready or not
        if(xhr.readyState == 4 && xhr.status == 200)
        {
            dataDiv = document.getElementById('liveData');
            // Set current data text
            dataDiv.innerHTML = xhr.responseText;
            // Update the live data every 1 sec
            setTimeout(updateLiveData(), 3000);
        }
    }
});

    </script>
                
        
    </form>
</body>
</html>
)=====";
void handleRoot() {
 String s = MAIN_page; 
 server.send(200, "text/html", s); 
}


void read_data() {
  delay(500);                         
    String data = "\"Count\":\""+ String(visitors) +"\"";;
    //server.send(200, "text/plane", data); 
    
}
String cse_ip = "192.168.101.51"; // YOUR IP from ipconfig/ifconfig
String cse_port = "8080";
String server1 = "http://192.168.101.51:8080//in-cse/in-name/"; //+ cse_ip + ":" + cse_port + "//in-cse/in-name/";
String ae = "PIR_SENSOR";
String cnt = "Node1";
//char ssid[]="Galaxy";
//char password[]="12345678";
const char* servers="mqtt3.thingspeak.com";
const char* mqttUserName="JQMnPDkhFSkwHScoFiExBj0";
const char* mqttPass="1JCLQbSAKJxa/fo+y5R+G8K9";
long writeChannelID = 1764879;
const char* writeAPIKey="HXOT8OKW327RK31I";
const char* clientID = "JQMnPDkhFSkwHScoFiExBj0";
int mqttPort=1883;
int delay_in_conn = 1;
int data1 =0;
//#define POWER_PIN  26
//#define SIGNAL_PIN 35
WiFiClient client;
PubSubClient mqttClient(client);
void createCI(String val){
HTTPClient http;
http.begin(server1 + ae + "/" + cnt + "/");
http.addHeader("X-M2M-Origin", "admin:admin");
http.addHeader("Content-Type", "application/json;ty=4");
int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": \"" + String(val) + "\"}}");
//Serial.println(code);
if (code == -1) {
Serial.println("UNABLE TO CONNECT TO THE SERVER");
}
http.end();
}
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 587

/* The sign in credentials */
#define AUTHOR_EMAIL "hiw89239@gmail.com"
#define AUTHOR_PASSWORD "bzjm weuo jdfy bwlt"

/* Recipient's email*/
#define RECIPIENT_EMAIL "lakshmichitturi18@gmail.com"

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);

void setup(){
  Serial.begin(115200);
  Serial.println();
  pinMode(output26, OUTPUT);
   WiFi.mode(WIFI_STA);
  // Set outputs to LOW
  digitalWrite(output26, HIGH);
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
     Serial.println("Visitor counter");
  Serial.println("Number of people inside the room:");
  Serial.println(visitors);
  pinMode(buzzer,OUTPUT);
  pinMode(ledenter,OUTPUT);
  digitalWrite(buzzer,HIGH);  
  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  server.on("/", handleRoot);     
server.on("/read_data", read_data);
server.begin();
 mqttClient.setServer( servers,mqttPort);
 server.begin();
ThingSpeak.begin(client);
}
void mqttConnect()
{
  while ( !mqttClient.connected() )
  {
    // Connect to the MQTT broker.
    if ( mqttClient.connect(clientID,mqttUserName,mqttPass) ) {
      Serial.print( "MQTT to " );
      Serial.print( servers );
      Serial.print (" at port ");
      Serial.print( mqttPort );
      Serial.println( " successful." );
    }
    else {
 
      Serial.print( "MQTT connection failed, rc = " );
 
      Serial.print( mqttClient.state() );
 
      Serial.println( " Will try again in a few seconds" );
 
      //delay( delay_in_conn*1000 );
 
    }
 
  }
 
}
 
 
void mqttPublish(long pubChannelID, String pubWriteAPIKey, int dataArray, int fieldArray[])
{
 // int var;
  String string="1="+String(dataArray);
  // Serial.println("data");
 
    //Serial.println(dataArray);
 
    /////

  //int x =
  //ThingSpeak.setField(1, visitors);
 // ThingSpeak.writeFields(pubChannelID,pubWriteAPIKey);
 
    String topicstring="channels/" +String(pubChannelID) + "/publish";//+String(pubWriteAPIKey);
 
    
 
    mqttClient.publish(topicstring.c_str(), string.c_str());
 
 //   Serial.println(pubChannelID);
 
  
 
}
 
void loop(){
 WiFiClient client = server.available();   // Listen for incoming clients
  server.handleClient();
  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(output26, HIGH);
            }
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>GPIO 26 - State " + output26State + "</p>");
            // If the output26State is off, it displays the ON button       
            if (output26State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  if(!mqttClient.connected())
 
  mqttConnect();
 
mqttClient.loop();
  delay(10); 
int field_to_publish[8]={1,0,0,0,0,0,0,0};
 
delay(1000);
 
 int sensorstate = digitalRead(sensor);
int sensor1state = digitalRead(sensor1);
  
  if(sensorstate == HIGH){
       visitors++;
    delay(500);
    }
  
  else if (sensor1state == HIGH) {
    if(visitors>0)
    visitors--;
    delay(500);
  }
  if(visitors>6)
  {
    digitalWrite(buzzer,LOW);
    digitalWrite(ledenter,LOW);
    digitalWrite(ledstop,LOW);
  }
  else if(visitors>6&&visitors<9)
  {
    digitalWrite(buzzer,HIGH);
    digitalWrite(ledstop,HIGH);
    //delay(1000);
    digitalWrite(ledenter,LOW);
  }
  else if(visitors<6)
  {
    digitalWrite(buzzer,HIGH);
    digitalWrite(ledenter,HIGH);
    digitalWrite(ledstop,LOW);
  }
   String ans=String(visitors);
     createCI(ans);
     Serial.print("Uploaded val : ");
 Serial.println(visitors);
     // Serial.println(visitors);
mqttPublish(writeChannelID, writeAPIKey,visitors,field_to_publish);

}


void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");
  }
if(visitors==0)
{
  /* Callback function to get the Email sending status */

/** Enable the debug via Serial port
   * none debug or 0
   * basic debug or 1
  */
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the session config data */
  ESP_Mail_Session session;

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  /* Declare the message class */
  SMTP_Message message;

  /* Set the message headers */
  message.sender.name = "ESP";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "ESP Test Email";
  message.addRecipient("Sara", RECIPIENT_EMAIL);

//  /Send HTML message/
  //String htmlMsg = "<div style=\"color:#2f4468;\"><h1>Dear User!</h1><p>COVID PROTOCOL IS VIOLATED!!!</p><p>MORE PEOPLE IN!!!</p><p>CHECK! CHECK! CHECK!</p><p>- Sent from room</p></div>";
   String htmlMsg = "<div style=\"color:#2f4468;\"><h1>Dear User!</h1><p>Room is Empty!!!</p><p>Electrical appliances to be switched off</p><p>Air conditioners->x</p><p>Fans->y</p><p>Lights->z</p><p>- Sent from room</p></div>";
  
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  /*
  //Send raw text message
  String textMsg = "Hello World! - Sent from ESP board";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;*/

  /* Set the custom message header */
  //message.addHeader("Message-ID: <abcde.fghij@gmail.com>");

  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return;

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
}

if(visitors>6)
{
/* Callback function to get the Email sending status */

/** Enable the debug via Serial port
   * none debug or 0
   * basic debug or 1
  */
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the session config data */
  ESP_Mail_Session session;

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  /* Declare the message class */
  SMTP_Message message;

  /* Set the message headers */
  message.sender.name = "ESP";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "ESP Test Email";
  message.addRecipient("Sara", RECIPIENT_EMAIL);

//  /Send HTML message/
  String htmlMsg = "<div style=\"color:#2f4468;\"><h1>Dear User!</h1><p>COVID PROTOCOL IS VIOLATED!!!</p><p>MORE PEOPLE IN!!!</p><p>CHECK! CHECK! CHECK!</p><p>- Sent from room</p></div>";
   //String htmlMesg = "<div style=\"color:#2f4468;\"><h1>Dear User!</h1><p>Room is Empty!!!</p><p>Electrical appliances to be switched off</p><p>Air conditioners->x</p><p>Fans->y</p><p>Lights->z</p><p>- Sent from room</p></div>";
  
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  /*
  //Send raw text message
  String textMsg = "Hello World! - Sent from ESP board";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;*/

  /* Set the custom message header */
  //message.addHeader("Message-ID: <abcde.fghij@gmail.com>");
  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return;

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
}

  
}
