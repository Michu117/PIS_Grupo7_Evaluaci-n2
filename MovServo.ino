#include <Servo.h>

// Definimos los pines para los dos servomotores y las dos fotorresistencias
const int servoPin1 = 9;   // Pin para el servo de orientación
const int servoPin2 = 10;  // Pin para el servo de inclinación
const int ldrPin1 = A0;    // Pin para la fotorresistencia horizontal
const int ldrPin2 = A1;    // Pin para la fotorresistencia vertical

Servo servo1;  // Servo para orientación
Servo servo2;  // Servo para inclinación

int pos1 = 90;  // Posición inicial del servo de orientación
int pos2 = 90;  // Posición inicial del servo de inclinación

void setup() {
  servo1.attach(servoPin1);  // Adjuntar el servo de orientación al pin correspondiente
  servo2.attach(servoPin2);  // Adjuntar el servo de inclinación al pin correspondiente

  servo1.write(pos1);  // Mover el servo de orientación a la posición inicial
  servo2.write(pos2);  // Mover el servo de inclinación a la posición inicial
}

void loop() {
  int ldrValue1 = analogRead(ldrPin1);  // Leer el valor de la fotorresistencia horizontal
  int ldrValue2 = analogRead(ldrPin2);  // Leer el valor de la fotorresistencia vertical

  // Ajustar la posición del servo de orientación en función del valor de la fotorresistencia horizontal
  if (ldrValue1 > 600) {
    pos1 += 1;
  } else if (ldrValue1 < 400) {
    pos1 -= 1;
  }

  // Ajustar la posición del servo de inclinación en función del valor de la fotorresistencia vertical
  if (ldrValue2 > 600) {
    pos2 += 1;
  } else if (ldrValue2 < 400) {
    pos2 -= 1;
  }

  // Limitar los rangos de los servos para evitar que se muevan fuera de sus límites físicos
  pos1 = constrain(pos1, 0, 180);
  pos2 = constrain(pos2, 0, 180);

  // Mover los servos a las nuevas posiciones
  servo1.write(pos1);
  servo2.write(pos2);

  // Esperar un pequeño intervalo antes de la siguiente lectura
  delay(100);
}
