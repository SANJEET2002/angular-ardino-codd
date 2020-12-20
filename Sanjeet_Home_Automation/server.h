#ifndef SERVER_H__
#define SERVER_H__

  #ifndef PRINT
  #define PRINT
    #define var(x) #x
    #define Print(x) Serial.println(String(var(x)) + " : " + (x))
  #endif

  #include "JSON.h"
  #include "Device.h"
  #include <WiFi.h>
  #include <WebServer.h>

  float temprature;
  float humidity;
  
  WebServer server(80);
  void prepareResponse() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Max-Age", "10000");
    server.sendHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "*");
  }
  void handleRoot() {
    JSON response;
    response.set("status", "okay");
    Serial.println("/");
    prepareResponse();
    server.send(200, "application/json", response.stringify());
  }

  
  void changeState() {
    JSON requestArray = server.arg("data");
    int i;
    // var deviceStates = [{state:1,id:0},{state:0,id:1},{id:4,state:0},{state:1,id:3},{state:1,id:7}]
    // var requestURL = 'http://192.168.4.1/changeState?data=' + JSON.stringify(deviceStates)
    
    Serial.println("/changeState");
    for(i=0; i<requestArray.length(); i++) {
      int id = requestArray[i]["id"].toInt();
      Print(id);
      if (id > -1 && id < Device::length()) {      
        bool state = requestArray[i]["state"].toInt();
        device[id].setState(state);
      } else {
        prepareResponse();
        return server.send(404, "application/json", JSON("{error:Invalid device ID}").stringify());
      }
    }
    prepareResponse();
    server.send(200, "application/json", JSON("{status:okay}").stringify());
  }

  void createSchedule() {
    JSON request = server.arg("data");
//    var requestURL = 'http://192.168.4.1/stopSchedule?data=' + JSON.stringify({id: deviceID, wait: 3, state: 1})
    Serial.println("/createSchedule");
    uint8_t id = request["id"].toInt();      
    if (id < Device::length() && id >= 0) {
      device[id].createSchedule(
        request["wait"].toInt(),
        request["state"].toInt() ? ON : OFF   
      );
      prepareResponse();  
      server.send(200, "application/json", JSON("{status:okay}").stringify());
    } else {      
      prepareResponse();
      server.send(404, "application/json", JSON("{error:Invalid device ID}").stringify());
    }
  }

  void stopSchedule() {
    JSON request = server.arg("data");
//    var requestURL = 'http://192.168.4.1/stopSchedule?data=' + JSON.stringify({id: deviceID})
    Serial.println("/stopSchedule");
    uint8_t id = request["id"].toInt();      
    if (id < Device::length() && id > 0) {
      device[id].stop();
      prepareResponse();
      server.send(200, "application/json", JSON("{status:okay}").stringify());
    } else {     
      prepareResponse(); 
      server.send(404, "application/json", JSON("{error:Invalid device ID}").stringify());
    }
  }

  void getData() {
    // var requestURL = 'http://192.168.4.1/getData?ids=[1,2,3,4]'
    JSON ids = server.arg("ids");
    JSON response = "[]";
    for(int i=0;i<ids.length();i++) {
      JSON dev;
      int id = ids[i].toInt();
      if (id > -1 && id < 8) {
        dev.set("id", id);
        dev.set("state", device[i].getState());
        response.push(dev.stringify());
      }
    }
    prepareResponse(); 
    server.send(200, "application/json", response.stringify());    
  }
  
  void getDHTData() {
    JSON response;
    response.set("temprature", temprature);
    response.set("humidity", humidity);
    prepareResponse(); 
    server.send(200, "application/json", response.stringify());    
  }
  
  void initializeServer() {
    server.on("/", handleRoot);
    server.on("/getData", getData);
    server.on("/changeState", changeState);
    server.on("/createSchedule", createSchedule);
    server.on("/stopSchedule", stopSchedule); 
    server.on("/getDHTData", getDHTData);    
    server.begin();
  }


  
#endif
