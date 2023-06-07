/*including required libraries*/
#include <Arduino.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ThingSpeak.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include "HTTPClient.h"
#include "time.h"
#include <ArduinoJson.h>

int visitors = 0;//initiating visitors to be 0
/*declaring pins for components based on connections*/
int sensor = 19;//sensor at entry
int sensor1 = 4;//sensor at exit
int buzzer = 23;
int ledenter=22;
int ledstop=2;
int capacity=6;//fixing capacity of room
WebServer server(80);
#define WIFI_SSID "Redmi"
#define WIFI_PASSWORD "diamond1"
WiFiClient WIFI_CLIENT;

String cse_ip = "192.168.101.51"; // YOUR IP from ipconfig/ifconfig
String cse_port = "8080";
String server1 = "http://192.168.101.51:8080//in-cse/in-name/"; //+ cse_ip + ":" + cse_port + "//in-cse/in-name/";
String ae = "PIR_SENSOR";
String cnt = "Node1";

/*Giving the credentials of thingspeak*/
const char* servers="mqtt3.thingspeak.com";
const char* mqttUserName="JQMnPDkhFSkwHScoFiExBj0";
const char* mqttPass="1JCLQbSAKJxa/fo+y5R+G8K9";
long writeChannelID = 1764879;
const char* writeAPIKey="HXOT8OKW327RK31I";
const char* clientID = "JQMnPDkhFSkwHScoFiExBj0";
int mqttPort=1883;
int delay_in_conn = 1;
WiFiClient client;
PubSubClient mqttClient(client);
void createCI(String val){
HTTPClient http;
http.begin(server1 + ae + "/" + cnt + "/");
http.addHeader("X-M2M-Origin", "admin:admin");
http.addHeader("Content-Type", "application/json;ty=4");
int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": \"" + String(val) + "\"}}");
http.end();
}

/*SMTP Server settings*/
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
  Serial.begin(115200);//setting baurd rate
  Serial.println();
  /*handling WIFI connections*/
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  /*initializing the hardware*/
  Serial.println("Visitor counter");
  Serial.println("Number of people inside the room:");
  Serial.println(visitors);
  pinMode(buzzer,OUTPUT);
  pinMode(ledenter,OUTPUT);
  digitalWrite(buzzer,HIGH);
    
  /*info of wifi connections*/
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  mqttClient.setServer( servers,mqttPort);
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
    }
  }
}
 
 /*Function for sending data to thingspeak where it plots a graph*/
void mqttPublish(long pubChannelID, String pubWriteAPIKey, int dataArray, int fieldArray[])
{
  String string="1="+String(dataArray);
  String topicstring="channels/" +String(pubChannelID) + "/publish";
  mqttClient.publish(topicstring.c_str(), string.c_str());
}

void loop(){
  /*verifying MQTT connections*/
  if(!mqttClient.connected())
    mqttConnect();
  mqttClient.loop();
  delay(10);
  
int field_to_publish[8]={1,0,0,0,0,0,0,0};//details of field in thingspeak channel where we have to publish data
delay(1000);

 /*Read data from sensors*/
 int sensorstate = digitalRead(sensor);
 int sensor1state = digitalRead(sensor1);

  /*Incrementing count when PIR sensor at entry detects motion*/
  if(sensorstate == HIGH){
       visitors++;
    delay(500);
    }
  /*Decrementing count when PIR sensor at exit detects motion when the room is not empty*/
  else if (sensor1state == HIGH) {
    if(visitors>0)
    visitors--;
    delay(500);
  }
  /*Controlling actions of 2 LEDS*/
  if(visitors>capacity)
  {
    digitalWrite(buzzer,LOW);
    digitalWrite(ledenter,LOW);
    digitalWrite(ledstop,HIGH);
  }
  else if(visitors<capacity+1)
  {
    digitalWrite(buzzer,HIGH);
    digitalWrite(ledenter,HIGH);
    digitalWrite(ledstop,LOW);
  }
   String ans=String(visitors);
   createCI(ans);
   Serial.print("Uploaded val : ");
   Serial.println(visitors);
   mqttPublish(writeChannelID, writeAPIKey,visitors,field_to_publish);//publishing data in thingspeak by calling a function

/*For sending email to recepeint when the room is empty*/
if(visitors==0)
{
  /* Callback function to get the Email sending status */

/** Enable the debug via Serial port
   * none debug or 0
   * basic debug or 1*/
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

  /*Send HTML message*/
  String htmlMsg = "<div style=\"color:#2f4468;\"><h1>Dear User!</h1><p>Room is Empty!!!</p><p>Electrical appliances to be switched off</p><p>Air conditioners->x</p><p>Fans->y</p><p>Lights->z</p><p>- Sent from room</p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
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
   * basic debug or 1*/
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

  /*Send HTML message*/
  String htmlMsg = "<div style=\"color:#2f4468;\"><h1>Dear User!</h1><p>COVID PROTOCOL IS VIOLATED!!!</p><p>MORE PEOPLE IN!!!</p><p>CHECK! CHECK! CHECK!</p><p>- Sent from room</p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return;

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
}
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
}
