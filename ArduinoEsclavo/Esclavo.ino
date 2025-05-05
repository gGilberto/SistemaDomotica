#include <Wire.h>    // Biblioteca para comunicación I2C
#include <Servo.h>   // Biblioteca para control de servomotor
#include <Keypad.h>  // Biblioteca para el teclado matricial 4x4

// Definición de pines de actuadores
#define PIN_RELE_LUZ 3   // Relé para controlar iluminación inteligente
#define PIN_RELE_VENTILADOR 5  // Relé para activar ventilador
#define PIN_BUZZER 7    // Buzzer para sistema de alarma
#define PIN_SERVO 17    // Servomotor para control de acceso
#define PIN_RELE_BOMBA 8  // Relé para activar bomba de agua (riego automático)

// Configuración del teclado matricial
const byte FILAS = 4, COLUMNAS = 4;
char teclas[FILAS][COLUMNAS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
byte pinesFilas[FILAS] = {9, 10, 11, 12}, pinesColumnas[COLUMNAS] = {13, 14, 15, 16};

Keypad teclado = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, FILAS, COLUMNAS);
Servo motorPuerta;

// Código de acceso
String codigoIngresado = "";  // Variable para almacenar el código ingresado
String codigoCorrecto = "1234";  // Código de acceso correcto

// Variables para recibir datos del Arduino Maestro
int estadoMovimiento, temperatura, nivelLuz, humedadSuelo;

void setup() {
    Wire.begin(8); // Configurar Arduino como esclavo con dirección 8
    Wire.onReceive(recibirDatos); // Función que se ejecuta cuando recibe datos del maestro

    // Configuración de actuadores
    pinMode(PIN_RELE_LUZ, OUTPUT);
    pinMode(PIN_RELE_VENTILADOR, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_RELE_BOMBA, OUTPUT);
    
    motorPuerta.attach(PIN_SERVO);
}

void recibirDatos(int numBytes) {
    // Leer los datos enviados por el maestro
    estadoMovimiento = Wire.read();
    temperatura = Wire.read();
    nivelLuz = Wire.read();
    humedadSuelo = Wire.read();
}

void loop() {
    // Control de Iluminación Inteligente
    if (estadoMovimiento == HIGH && nivelLuz < 500) {  
        digitalWrite(PIN_RELE_LUZ, HIGH);  // Encender luz si hay movimiento y poca luz
    } else {
        digitalWrite(PIN_RELE_LUZ, LOW);   // Apagar luz
    }

    // Control de Ventilación
    if (temperatura > 28) {  
        digitalWrite(PIN_RELE_VENTILADOR, HIGH);  // Activar ventilador si temperatura alta
    } else {
        digitalWrite(PIN_RELE_VENTILADOR, LOW);   // Apagar ventilador
    }

    // Sistema de Seguridad y Alarma
    if (estadoMovimiento == HIGH) {  
        digitalWrite(PIN_BUZZER, HIGH);  // Activar alarma si detecta movimiento
    } else {
        digitalWrite(PIN_BUZZER, LOW);   // Apagar alarma
    }

    // Riego Automático
    if (humedadSuelo < 400) { // Si la humedad del suelo es baja, activar bomba
        digitalWrite(PIN_RELE_BOMBA, HIGH);
        delay(5000); // Riego por 5 segundos
        digitalWrite(PIN_RELE_BOMBA, LOW);
    }

    // Control de Acceso con Código de Seguridad
    char teclaPresionada = teclado.getKey(); // Capturar la tecla presionada
    if (teclaPresionada) {
        codigoIngresado += teclaPresionada;
        if (codigoIngresado.length() == 4) {  // Se ingresaron 4 dígitos
            if (codigoIngresado == codigoCorrecto) {  
                motorPuerta.write(90);  // Desbloquear puerta
                delay(3000);
                motorPuerta.write(0);   // Bloquear puerta después de 3 segundos
            }
            codigoIngresado = "";  // Reiniciar código ingresado
        }
    }
}
