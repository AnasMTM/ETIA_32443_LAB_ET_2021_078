// =======================
// Pin Definitions
// =======================
const int LDR_PIN      = 27;   // LDR Analog Output
const int POT_PIN      = 25;   // Potentiometer
const int GAS_PIN      = 34;   // MQ-2 Analog Output

const int LIGHT_LED    = 16;   // Purple LED
const int GAS_LED      = 0;   // Red LED

// Fixed gas threshold
const int GAS_THRESHOLD = 2000;

void setup()
{
  Serial.begin(115200);

  pinMode(LIGHT_LED, OUTPUT);
  pinMode(GAS_LED, OUTPUT);

  digitalWrite(LIGHT_LED, LOW);
  digitalWrite(GAS_LED, LOW);
}

void loop()
{
  // Read Analog Sensors
  int ldrValue = analogRead(LDR_PIN);
  int potValue = analogRead(POT_PIN);
  int gasValue = analogRead(GAS_PIN);

  // -------------------------
  // Adaptive Lighting
  // -------------------------
  // If LDR value is lower than the potentiometer threshold,
  // turn ON the light LED.

  if (ldrValue < potValue)
    digitalWrite(LIGHT_LED, LOW);
  else
    digitalWrite(LIGHT_LED, HIGH);

  // -------------------------
  // Gas Detection
  // -------------------------
  if (gasValue > GAS_THRESHOLD)
    digitalWrite(GAS_LED, HIGH);
  else
    digitalWrite(GAS_LED, LOW);

  // Display values
  Serial.print("LDR: ");
  Serial.print(ldrValue);

  Serial.print("   POT: ");
  Serial.print(potValue);

  Serial.print("   GAS: ");
  Serial.print(gasValue);

  Serial.print("   Light LED: ");
  Serial.print(digitalRead(LIGHT_LED));

  Serial.print("   Gas LED: ");
  Serial.println(digitalRead(GAS_LED));

  delay(100);
}