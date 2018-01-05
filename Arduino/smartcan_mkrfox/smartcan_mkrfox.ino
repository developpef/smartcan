#include <Wire.h>
#include <SigFox.h>
#include <ArduinoLowPower.h>

#define SENSOR_ADRS 0x40 // I2C address of GP2Y0E03 
#define DISTANCE_ADRS 0x5E // Data address of Distance Value // Functions to process only at power-on and reset 

int ans ;
byte c[2];
int rouge = 4;
int jaune = 5;
float taux = 0.0;

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
  uint16_t unused2;
  uint16_t unused3;
  uint16_t unused4;
  uint16_t unused5;
} SigfoxMessage;

// stub for message which will be sent
SigfoxMessage msg;

void setup () {
  // Initialize serial communication
  Wire.begin(); // Initialize I2C,

  //Serial.begin(9600);
  //while (!Serial) {};

  delay(1000); // start after 1 second
  pinMode(rouge, OUTPUT);
  pinMode(jaune, OUTPUT);
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

void loop () {
  Wire.beginTransmission (SENSOR_ADRS); // start communication processing
  Wire.write (DISTANCE_ADRS); // specify the address of the table storing the distance value
  ans = Wire.endTransmission(); // send and close data
  delay(200);

  if (ans == 0) {
    blankState();

    msg.poubelleNum = 34936; // ID C8Y
    msg.alarmState = 0;
    msg.tauxRemplissage = 0;

    ans = Wire.requestFrom(SENSOR_ADRS, 2) ;
    c[0] = Wire.read(); // Read the 11th to 4th bits of data c [1]
    c[1] = Wire.read(); // Read the 3rd and 0th bits of the data
    ans = ((c [0] * 16 + c [1]) / 16) / 4; // distance

    // if == 255 => mesure fausse (trop près ou trop loin)
    if (c[0] != 255) {
      //Serial.print(ans);
      //Serial.println ("cm");

      // calcul tu taux de remplissage, pour une poubelle de 60cm
      taux = ((60.0 - ans) / 60.0) * 100.0;
      msg.tauxRemplissage = (int) taux;

      if (taux < 80 && taux > 60) {
        digitalWrite(jaune, HIGH);
        digitalWrite(rouge, LOW);
        msg.alarmState = 1;
      } else if (taux > 80) {
        digitalWrite(rouge, HIGH);
        digitalWrite(jaune, LOW);
        msg.alarmState = 2;
      }
    }

    sendMessage();
    delay(1000 * 60 * 11); // sleep 11 min

  } else {
    //Serial.print ("ERROR NO. ="); // Can not communicate with GP2Y0E03
    //Serial.println (ans);
  }
}

void blankState() {
  digitalWrite(rouge, LOW);
  digitalWrite(jaune, LOW);
}

