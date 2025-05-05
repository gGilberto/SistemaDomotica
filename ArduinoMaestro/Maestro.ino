#include <Wire.h>  // Biblioteca para la comunicación I2C
#include <DHT.h>   // Biblioteca para el sensor DHT11 (temperatura y humedad)

// Definición de pines de sensores
#define PIN_PIR 2        // Sensor de movimiento PIR
#define PIN_LDR A0       // Sensor de luz ambiental LDR
#define PIN_DHT 4        // Sensor de temperatura y humedad DHT11
#define PIN_SUELO A1     // Sensor de humedad del suelo

#define TIPO_DHT DHT11  
DHT sensorDHT(PIN_DHT, TIPO_DHT);  // Inicialización del sensor DHT11

void setup() {
    Wire.begin();       // Configurar Arduino como maestro en la comunicación I2C
    sensorDHT.begin();  // Iniciar sensor de temperatura y humedad
    pinMode(PIN_PIR, INPUT);  // Configurar el sensor PIR como entrada
}

void loop() {
    // Lectura de sensores
    int estadoMovimiento = digitalRead(PIN_PIR);  // Leer estado del sensor PIR
    int nivelLuz = analogRead(PIN_LDR);          // Leer nivel de luz ambiente
    float temperatura = sensorDHT.readTemperature(); // Leer temperatura actual
    int humedadSuelo = analogRead(PIN_SUELO);    // Leer humedad del suelo

    // Envío de datos al Arduino Esclavo utilizando I2C
    Wire.beginTransmission(8); // Dirección del Arduino esclavo
    Wire.write(estadoMovimiento);  // Enviar estado del sensor PIR
    Wire.write((int)temperatura);  // Enviar temperatura como número entero
    Wire.write(nivelLuz);          // Enviar nivel de luz
    Wire.write(humedadSuelo);      // Enviar humedad del suelo
    Wire.endTransmission(); // Finalizar la transmisión de datos

    delay(500); // Esperar medio segundo antes de la próxima lectura
}
