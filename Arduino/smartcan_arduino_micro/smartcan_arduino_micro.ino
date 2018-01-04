// Preliminary pass at getting GP2Y0E03 working.
#include <Wire.h>

#define SENSOR_ADRS 0x40 // I2C address of GP2Y0E03 
#define DISTANCE_ADRS 0x5E // Data address of Distance Value // Functions to process only at power-on and reset 

int ans ;
byte c[2];

int rouge = 4;
int jaune = 5;

void setup () {
  // Initialize serial communication
  Wire.begin(); // Initialize I2C,
  delay(1000); // start after 1 second
  Serial.begin(9600);
  pinMode(rouge, OUTPUT);
  pinMode(jaune, OUTPUT);
}

void loop () {
  Wire.beginTransmission (SENSOR_ADRS); // start communication processing
  Wire.write (DISTANCE_ADRS); // specify the address of the table storing the distance value
  ans = Wire.endTransmission(); // send and close data
  delay(200);
  if (ans == 0) {
    ans = Wire.requestFrom(SENSOR_ADRS, 2) ;
    c[0] = Wire.read(); // Read the 11th to 4th bits of data c [1]
    c[1] = Wire.read(); // Read the 3rd and 0th bits of the data
    /*Serial.print("ans:");
      Serial.print(c[0]);
      Serial.print(" ");
      Serial.println(c[1]);*/
    ans = ((c [0] * 16 + c [1]) / 16) / 4; // distance

    // if == 255 => mesure fausse (trop près ou trop loin)
    if (c[0] != 255) {
      Serial.print(ans);
      Serial.println ("cm"); //to display on serial monitor;
      Serial.println ("");
      if (ans < 50 && ans > 15) {
        digitalWrite(jaune, HIGH);
        digitalWrite(rouge, LOW);
      } else if (ans < 15) {
        digitalWrite(rouge, HIGH);
        digitalWrite(jaune, LOW);
      } else {
        digitalWrite(rouge, LOW);
        digitalWrite(jaune, LOW);
      }
    } else {
      digitalWrite(rouge, LOW);
      digitalWrite(jaune, LOW);
    }
  } else {
    Serial.print ("ERROR NO. ="); // Can not communicate with GP2Y0E03
    Serial.println (ans);
  }
}
