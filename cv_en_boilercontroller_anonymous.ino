/**
 * @example TCPClientSingle.ino
 * @brief The TCPClientSingle demo of library WeeESP8266. 
 * @author Wu Pengfei<pengfei.wu@itead.cc> 
 * @date 2015.02
 * 
 * @par Copyright:
 * Copyright (c) 2015 ITEAD Intelligent Systems Co., Ltd. \n\n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version. \n\n
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "ESP8266.h"
#include "string.h"
#include <Base64.h>
//#define SERIAL1_RX_BUFFER_SIZE (100);
#define SSID        "yournetworkname"
#define PASSWORD    "networkpassword"
#define HOST_NAME   "ip address of your cube"
#define HOST_PORT   (62910)
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#include <SoftwareSerial.h>
#define _SS_MAX_RX_BUFF 256 // RX buffer size //BEFORE WAS 64
SoftwareSerial mySerial(3,2);
ESP8266 wifi(mySerial);
int valvePos=0;
int burner=11;
int cvPump=12;
int boilerPump=13;
int boilerSense=A3;
int heatDemand=0;
float boilerTemp;
void setup(void)
{
    pinMode(burner,OUTPUT);
    digitalWrite(burner,LOW);
    pinMode(cvPump,OUTPUT);
    digitalWrite(cvPump,LOW);
    pinMode(boilerPump,OUTPUT);
    digitalWrite(boilerPump,LOW);
    lcd.begin(16,2);
    lcd.print("hello world");
    Serial.begin(115200);
    //mySerial.begin(9600);
    Serial.print("setup begin\r\n");
    delay(60000);
    analogReference(INTERNAL);
    Serial.print("FW Version:");
    Serial.println(wifi.getVersion().c_str());
    Serial.print("Alive:");
    Serial.println(wifi.kick());
      
    if (wifi.setOprToStationSoftAP()) {
        Serial.print("to station + softap ok\r\n");
    } else {
        Serial.print("to station + softap err\r\n");
    }
 
    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.print("Join AP success\r\n");
        Serial.print("IP:");
        Serial.println( wifi.getLocalIP().c_str());       
    } else {
        Serial.print("Join AP failure\r\n");
    }
    
    if (wifi.disableMUX()) {
        Serial.print("single ok\r\n");
    } else {
        Serial.print("single err\r\n");
    }
    
    Serial.print("setup end\r\n");
}
 
void loop(void)
{
    uint8_t buffer[256] = {0};
    
    if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
        Serial.print("create tcp ok\r\n");
    } else {
        Serial.print("create tcp err\r\n");
    }
    
    char *lWord = "l:\r\n";
    
    
    /*while (Serial1.available()>0){
      char c=Serial1.read();
      Serial.print(c);
      //delay(10);
    }*/
    
    delay(1000);    /// ignore standard cube response
    //Serial.println(len);
    wifi.send((const uint8_t*)lWord, strlen(lWord));
    //delay(100);
    //memset(buffer,'0',sizeof(buffer));
    uint32_t len = wifi.recv(buffer, sizeof(buffer), 3000);
        Serial.println(len);
    if (buffer[0]=='L') {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
            //delay(10);
          }
        Serial.print("]\r\n");
        for(int i=2;i<len-1;i++){
          buffer[i-2]=buffer[i];
          }
        len=len-2;  //ignore last two characters
    
        int decLen = base64_dec_len((char *)buffer,int(len));
        //Serial.print("decoded Len: ");
        //Serial.println(decLen);
        char decoded[decLen];
        base64_decode(decoded,(char *)buffer,len);
        //Serial.println(decoded[1],HEX);
        //Serial.println(decoded[0],HEX);
        boilerTemp=analogRead(boilerSense)*11/102.3;
        delay(1);
        Serial.print("Boilersense: ");
        Serial.println(boilerTemp);
        lcd.clear();
        lcd.print("Boiler: ");
        lcd.print(boilerTemp);
        lcd.setCursor(0,1);
        lcd.print("Warmtevraag: ");
        lcd.print(processInfo(decoded,decLen));
        heatDemand=processInfo(decoded,decLen);
        
        //burner
        if (heatDemand >0 || boilerTemp<45) digitalWrite(burner,HIGH);
        else if (boilerTemp>54 && heatDemand==0) digitalWrite(burner,LOW);

        //cv pump
        if (heatDemand >0) digitalWrite(cvPump,HIGH);
        else digitalWrite(cvPump,LOW);

        //boilerpump
        if (boilerTemp<45) digitalWrite(boilerPump,HIGH);
        else if (boilerTemp>54) digitalWrite(boilerPump,LOW); 
        
    }
    
    if (wifi.releaseTCP()) {
        Serial.print("release tcp ok\r\n");
    } else {
        Serial.print("release tcp err\r\n");
    }
    delay(15000);
}



int processInfo(char *decoded,int len){
     
  int i =0;
  int j=0;
  int demand=0;
  int valvePos[10]={0};
  
    while(i<len-1){
      //Serial.println(decoded[i],DEC);
      switch ((byte)decoded[i]){
      case 11:
        /*Serial.print("RF Address: ");
        Serial.print(byte(decoded[i+1]),HEX);
        Serial.print(byte(decoded[i+2]),HEX);
        Serial.println(byte(decoded[i+3]),HEX);*/
        Serial.print("Klepstand: ");
        valvePos[j]=decoded[i+7];
        Serial.println(valvePos[j]);
        j+=1;
        /*Serial.print("Setpoint: ");
        Serial.println(int(decoded[i+8])/2);
        Serial.print("kamertemperatuur: ");
        Serial.println(decoded[i+9],DEC);*/
        i+=12;
        break;
       default:
        Serial.println("Unknown device");
        Serial.println(int(decoded[i]));
        i+=int(decoded[i])+1;
        break;
      }  
    }
    
    while (j>=0){
      if (valvePos[j]>30) demand+=1;
      j-=1;
    }
   return demand;
}

