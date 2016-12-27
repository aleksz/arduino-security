const char SHORT_CISRCUIT = 'S';
const char SECURE = 'Y';
const char ALARM = 'A';
const char CUT = 'C';
const char HEALTH_CHECK = 'H';

const int ledPin = 13;      // select the pin for the LED

int sensorPins[] = {A0, A1};
int sensorValue[] = {0, 0};  // variable to store the value coming from the sensor
int alarmThreshold[] = {150, 150};
int cutThreshold[] = {900, 900};
int secureThreshold[] = {70, 70};
char sensorState[] = {SECURE, SECURE};
int coolDownMillis = 500;
unsigned long alarmCooldownStart[2];
boolean alarmRaised[2];
unsigned long heartbearStart;

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);

  for (int i = 0; i < (sizeof(sensorPins)/sizeof(int)); i++) { 
    initSensorPin(i);
  }

  Serial.begin(9600);
  Serial.flush();
}

void loop() {
  for (int i = 0; i < (sizeof(sensorPins)/sizeof(int)); i++) { 
    checkSensor(i);
    maybeSendHeartBeat();
    delay(5);
  }
}

void initSensorPin(int sensor) {
  pinMode(sensorPins[sensor], INPUT);
  digitalWrite(sensorPins[sensor], HIGH);  // set pullup on analog pin 0 
}

void checkSensor(int sensor) {
  sensorValue[sensor] = analogRead(sensorPins[sensor]);
//  Serial.println(sensorValue[sensor]);

  if (sensorValue[sensor] > cutThreshold[sensor]) {
    sensorState[sensor] = CUT;
    maybeRaiseAlarm(CUT, sensor);
  } else if (sensorValue[sensor] > alarmThreshold[sensor] && sensorValue[sensor] <= cutThreshold[sensor]) {
    alarmCooldownStart[sensor] = millis();  
    sensorState[sensor] = ALARM;
    maybeRaiseAlarm(ALARM, sensor);
  } else if (sensorValue[sensor] > secureThreshold[sensor] && sensorValue[sensor] <= alarmThreshold[sensor]) {
    if (millis() - alarmCooldownStart[sensor] >= coolDownMillis && alarmRaised[sensor]) {
      alarmRaised[sensor] = false;
      sensorState[sensor] = SECURE;
      if (!hasRaisedAlarms()) {
        digitalWrite(ledPin, LOW); 
      }
      String msg = String(SECURE) + sensor;
      Serial.println(msg);        
    }
  } else {
    sensorState[sensor] = SHORT_CISRCUIT;
    maybeRaiseAlarm(SHORT_CISRCUIT, sensor);
  }
}

void maybeRaiseAlarm(char alarmType, int sensor) {
  if (!alarmRaised[sensor]) {
    alarmRaised[sensor] = true;
    String msg = String(alarmType) + sensor;
    Serial.println(msg);
    digitalWrite(ledPin, HIGH);
  }    
}

void maybeSendHeartBeat() {
  if (millis() - heartbearStart > 1000) {
     heartbearStart = millis();
     sendHeartBeat();     
  }
}

void sendHeartBeat() {
  String msg = String(HEALTH_CHECK);

  for (int i = 0; i < (sizeof(sensorPins)/sizeof(int)); i++) { 
    msg += sensorState[i];
  }

  Serial.println(msg);
}

boolean hasRaisedAlarms() {
  for (int i = 0; i < (sizeof(sensorPins)/sizeof(int)); i++) {
    if (alarmRaised[i] == true) {
      return true;
    }
  }

  return false;
}

