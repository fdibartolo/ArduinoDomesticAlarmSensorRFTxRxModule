#include <VirtualWire.h>

const int pinInputSensor1 = 2;
const int pinInputSensor2 = 3;
const int pinOutputLed = 13;

const char expectedMessage[5] = {'b','o','o','m'};
char actualMessage[5];

void setup() {                
  pinMode(pinInputSensor1, INPUT);     
  pinMode(pinInputSensor2, INPUT);     
  pinMode(pinOutputLed, OUTPUT);  

  vw_setup(2000); //Bits per sec  
  vw_rx_start();

  digitalWrite(pinOutputLed, LOW);
  Serial.begin(9600);
}

void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  
  //listening for wireless sensors
  if (vw_get_message(buf, &buflen)) // Non-blocking 
  {
    Serial.print("New message in: ");
    int i; 
    for (i = 0; i < buflen; i++) {
      actualMessage[i] = buf[i];
    }
    Serial.println(actualMessage);
    
    if (IsValidMessage())
      NotifySensorIsTriggered();
  }

  if (IsWiredSensorTriggered())
    NotifySensorIsTriggered();
}

boolean IsValidMessage(){
  return ((actualMessage[0] == expectedMessage[0]) && (actualMessage[1] == expectedMessage[1]) && (actualMessage[2] == expectedMessage[2]) && (actualMessage[3] == expectedMessage[3]));
}

boolean IsWiredSensorTriggered(){
  return ((digitalRead(pinInputSensor1) == HIGH) || (digitalRead(pinInputSensor2) == HIGH));
}

void NotifySensorIsTriggered(){
  // temporarily stop the receiver, so this station is not receiving its own message
  vw_rx_stop();
  delay(500);

  digitalWrite(pinOutputLed, HIGH);
  Serial.println("Message sent to receiver!");
  SendMessageToReceiver();
  digitalWrite(pinOutputLed, LOW);

  // start listening again
  delay(1000);
  vw_rx_start();
}

void SendMessageToReceiver(){
  const char *msg = "boom";

  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx(); // Wait until the whole message is gone
  delay(200);
}
