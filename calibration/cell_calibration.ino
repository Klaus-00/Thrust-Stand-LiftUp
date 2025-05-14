
#include <HX711.h>

#define DT_LEFT 3
#define SCK_LEFT 2
#define DT_RIGHT 7
#define SCK_RIGHT 6
#define DT_THRUST 5
#define SCK_THRUST 4

const int cellSelect = 0;
const float calibrationFactor = -213499.00;
HX711 scale;

void setup() {
  Serial.begin(9600);
  int dt, sck;
  if (cellSelect == 0) { dt = DT_THRUST; sck = SCK_THRUST; Serial.println("Thrust"); }
  else if (cellSelect == 1) { dt = DT_LEFT; sck = SCK_LEFT; Serial.println("Torque Left"); }
  else if (cellSelect == 2) { dt = DT_RIGHT; sck = SCK_RIGHT; Serial.println("Torque Right"); }
  else { Serial.println("Errore"); while (true); }

  scale.begin(dt, sck);
  while (!scale.is_ready()) { Serial.println("Attesa cella..."); delay(500); }
  scale.set_scale(calibrationFactor);
  delay(2000);

  long sum = 0;
  for (int i = 0; i < 10; i++) { sum += scale.read_average(); delay(50); }
  long offset = sum / 10;
  scale.set_offset(offset);
  Serial.print("Offset: "); Serial.println(offset);
  Serial.println("Posiziona peso noto.");
}

void loop() {
  float g = scale.get_units(10) * 1000.0;
  Serial.print("Peso misurato: "); Serial.print(g, 1); Serial.println(" g");
  delay(1000);
}
