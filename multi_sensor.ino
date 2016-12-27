int ledPin = 13;      // select the pin for the LED
int sensorPins[] = {A0, A1};
int sensorValue[] = {0, 0};  // variable to store the value coming from the sensor
int threshold[] = {50, 50};
int coolDownMillis = 500;
unsigned long alarmCooldownStart[2];
boolean alarmRaised[2];
unsigned long heartbearStart;

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  Serial.flush();
}

void loop() {
  for (int i = 0; i<2; i++) { 
    checkSensor(i);
    maybeSendHeartBeat();
    delay(5);
  }
}

void checkSensor(int sensor) {
  sensorValue[sensor] = analogRead(sensorPins[sensor]);
  //Serial.println(sensorValue[sensor]);

  if (sensorValue[sensor] > threshold[sensor]) {
    alarmCooldownStart[sensor] = millis();  
    if (!alarmRaised[sensor]) {
      alarmRaised[sensor] = true;
      String msg = String("N0") + sensor;
      Serial.println(msg);
      digitalWrite(ledPin, HIGH);  
    } 
  } else {
    if (millis() - alarmCooldownStart[sensor] >= coolDownMillis && alarmRaised[sensor]) {
      alarmRaised[sensor] = false;
      if (!hasRaisedAlarms()) {
        digitalWrite(ledPin, LOW); 
      }
      String msg = String("Y0") + sensor;
      Serial.println(msg);        
    }
  }
}

void maybeSendHeartBeat() {
  if (millis() - heartbearStart > 1000) {
     heartbearStart = millis();
     sendHeartBeat();     
  }
}

void sendHeartBeat() {
  String msg = String("H0");
  if (hasRaisedAlarms()) {
    msg += "Y";
  } else {
    msg += "N";
  }
  Serial.println(msg);
}

boolean hasRaisedAlarms() {
  for (int i = 0; i < 2; i++) {
    if (alarmRaised[i] == true) {
      return true;
    }
  }

  return false;
}

