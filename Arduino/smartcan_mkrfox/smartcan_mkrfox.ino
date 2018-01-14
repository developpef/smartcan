#include <Wire.h>
#include <SigFox.h>
#include <ArduinoLowPower.h>

#define SENSOR_ADRS 0x40 // I2C address of GP2Y0E03 
#define DISTANCE_ADRS 0x5E // Data address of Distance Value // Functions to process only at power-on and reset 
#define TAILLE_POUBELLE 35.0 // cm
#define LUM_SENSOR A1
#define POIDS_SENSOR A2
#define rouge 4
#define jaune 5

int ans ;
byte c[2];
float taux = 0.0;
unsigned long time;
bool isOpened;
bool isClosed;
int lum;
float dist = 0.0;
int poidsNewton = 0;

/*
    ATTENTION - the structure we are going to send MUST
    be declared "packed" otherwise we'll get padding mismatch
    on the sent data - see http://www.catb.org/esr/structure-packing/#_structure_alignment_and_padding
    for more details
*/
typedef struct __attribute__ ((packed)) sigfox_message {
  uint16_t poubelleNum;
  uint8_t alarmState;
  uint8_t tauxRemplissage;
  uint8_t nbOuvertures;
  uint16_t poids;
  uint8_t unused2;
  uint16_t unused4;
  uint16_t unused5;
} SigfoxMessage;

// stub for message which will be sent
SigfoxMessage msg;


/**
   CAPTEUR POIDS
*/
int fsrReading;      // the analog reading from the FSR resistor divider
int fsrVoltage;     // the analog reading converted to voltage
unsigned long fsrResistance;  // The voltage converted to resistance, can be very big so make "long"
unsigned long fsrConductance;
long fsrForce;       // Finally, the resistance converted to force



void setup () {
  // Initialize serial communication
  Wire.begin(); // Initialize I2C,

  //Serial.begin(9600);
  //while (!Serial) {};

  delay(1000); // start after 1 second
  pinMode(rouge, OUTPUT);
  pinMode(jaune, OUTPUT);
  pinMode(LUM_SENSOR, INPUT_PULLDOWN);
  pinMode(POIDS_SENSOR, INPUT_PULLDOWN);
  //
  if (!SigFox.begin()) {
    //Serial.println("Shield error or not present!");
    return;
  }
  /*String version = SigFox.SigVersion();
    String ID = SigFox.ID();
    String PAC = SigFox.PAC();

    // Display module informations
    Serial.println("MKRFox1200 Sigfox first configuration");
    Serial.println("SigFox FW version " + version);
    Serial.println("ID  = " + ID);
    Serial.println("PAC = " + PAC);
    Serial.println("");*/
  // Send the module to the deepest sleep
  SigFox.end();
}

void sendMessage() {
  //Serial.println("Sending message...");

  // Start the module
  SigFox.begin();
  // Wait at least 30ms after first configuration (100ms before)
  delay(100);
  // Clears all pending interrupts
  SigFox.status();
  delay(1);
  SigFox.beginPacket();
  SigFox.write((uint8_t*)&msg, 12);
  int ret = SigFox.endPacket();

  // Send the module to the deepest sleep
  SigFox.end();
  delay(100);

  /*Serial.begin(9600);
    while (!Serial) {};
    Serial.print("Status : ");
    Serial.println(ret);
    Serial.print("Sent message.");*/
}

/**
        CAPTEUR DISTANCE
*/
float mesureDistance() {
  dist = 0.0;
  ans = Wire.requestFrom(SENSOR_ADRS, 2) ;
  c[0] = Wire.read(); // Read the 11th to 4th bits of data c [1]
  c[1] = Wire.read(); // Read the 3rd and 0th bits of the data
  ans = ((c [0] * 16 + c [1]) / 16) / 4; // distance cm
  dist += ans;
  //Serial.print(ans);
  //Serial.println ("cm");
  return dist;
}

/**
         CAPTEUR POIDS
*/
int mesurePoids() {
  poidsNewton = 0;
  for (int i = 0; i < 5; i++) {
    fsrReading = analogRead(POIDS_SENSOR);
    // analog voltage reading ranges from about 0 to 1023 which maps to 0V to 5V (= 5000mV)
    fsrVoltage = map(fsrReading, 0, 1023, 0, 5000);
    // The voltage = Vcc * R / (R + FSR) where R = 10K and Vcc = 5V
    // so FSR = ((Vcc - V) * R) / V        yay math!
    fsrResistance = 5000 - fsrVoltage;     // fsrVoltage is in millivolts so 5V = 5000mV
    fsrResistance *= 10000;                // 10K resistor
    fsrResistance /= fsrVoltage;
    fsrConductance = 1000000;           // we measure in micromhos so
    fsrConductance /= fsrResistance;
    // Use the two FSR guide graphs to approximate the force
    if (fsrConductance <= 1000) {
      fsrForce = fsrConductance / 80;
    } else {
      fsrForce = fsrConductance - 1000;
      fsrForce /= 30;
    }
    poidsNewton += fsrForce;
    delay(100);
  }
  poidsNewton /= 5;
  return poidsNewton;
}

void loop () {
  /**
     NB OUVERTURES
  */
  lum = analogRead(LUM_SENSOR);
  if (lum < 800) {
    isOpened = true;
  } else if (isOpened) {
    msg.nbOuvertures++;
    isOpened = false;
  }


  //wait 11 min and check can is closed
  if ((time == 0 || millis() - time >= 1000 * 60 * 11) &&  lum >= 900) {
    Wire.beginTransmission (SENSOR_ADRS); // start communication processing
    Wire.write (DISTANCE_ADRS); // specify the address of the table storing the distance value
    ans = Wire.endTransmission(); // send and close data
    delay(200);

    digitalWrite(rouge, LOW);

    if (ans == 0) {
      time = millis();
      blankState();

      msg.poubelleNum = 34936; // ID C8Y
      msg.alarmState = 0;
      msg.tauxRemplissage = 0;
      msg.poids = mesurePoids();

      dist = mesureDistance();

      // if == 255 => mesure fausse (trop près ou trop loin)
      if (c[0] != 255) {
        // calcul tu taux de remplissage, pour une poubelle de TAILLE_POUBELLE
        taux = ((TAILLE_POUBELLE - dist) / TAILLE_POUBELLE) * 100.0;
        msg.tauxRemplissage = (int) taux;
      }

      if (taux < 80.0 && taux > 60.0) {
        digitalWrite(jaune, HIGH);
        digitalWrite(rouge, LOW);
        msg.alarmState = 1;
      } else if (taux > 80.0) {
        digitalWrite(rouge, HIGH);
        digitalWrite(jaune, LOW);
        msg.alarmState = 2;
      }

      sendMessage();

      msg.nbOuvertures = 0;
    } else {
      digitalWrite(rouge, HIGH);
    }
  }
}

void blankState() {
  digitalWrite(rouge, LOW);
  digitalWrite(jaune, LOW);
}

