#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <SoftwareSerial.h>

#define TRIG_PIN 6    // Pin de Trigger
#define ECHO_PIN 4    // Pin de Echo
#define HUMIDITY_PIN A0  // Pin analógico para el sensor de humedad SK410

// Configuración del Bluetooth (RX, TX)
SoftwareSerial bluetooth(10, 11); // RX, TX

// Variables globales
long duracion;
float distancia;
float humedad;

// Semáforo para manejar la exclusión mutua
SemaphoreHandle_t semaforo;

// Función para medir la distancia
void leerDistancia(void *pvParameters) {
  for (;;) {
    // Asegurarse de que el TRIG_PIN esté en LOW antes de disparar el pulso
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    // Enviar un pulso de 10 microsegundos al TRIG_PIN
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Medir el tiempo que tarda el pulso en regresar al pin ECHO
    duracion = pulseIn(ECHO_PIN, HIGH);

    // Calcular la distancia en centímetros
    distancia = (duracion * 0.0344) / 2;  // La velocidad del sonido es 0.0344 cm/us

    // Si se obtiene una distancia válida, mostrarla en el Monitor Serial
    if (duracion > 0) {
      Serial.print("Distancia: ");
      Serial.print(distancia);
      Serial.println(" cm");
    } else {
      Serial.println("Error en la medición.");
    }

    // Esperar 1 segundo antes de medir nuevamente
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// Función para leer la humedad desde el pin analógico
void leerHumedad(void *pvParameters) {
  for (;;) {
    // Leer el valor analógico del pin A0 (sensor de humedad SK410)
    int valorHumedad = analogRead(HUMIDITY_PIN);

    // Mapear el valor leído a un rango de 0 a 100 para simular porcentaje de humedad
    humedad = map(valorHumedad, 0, 1023, 100, 0);

    // Esperar 2 segundos antes de leer de nuevo
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// Función para enviar los datos por Bluetooth
void enviarBluetooth(void *pvParameters) {
  for (;;) {
    if (xSemaphoreTake(semaforo, portMAX_DELAY) == pdTRUE) {
      // Enviar los datos de distancia por Bluetooth
      bluetooth.print("Distancia: ");
      bluetooth.print(distancia);
      bluetooth.println(" cm");

      // Enviar los datos de humedad por Bluetooth
      bluetooth.print("Humedad: ");
      bluetooth.print(humedad);
      bluetooth.println(" %");

      // Liberar el semáforo después de enviar los datos
      xSemaphoreGive(semaforo);
    }
    // Esperar 500ms antes de enviar de nuevo
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup() {
  // Inicializar comunicación serial y Bluetooth
  Serial.begin(9600);
  bluetooth.begin(9600);

  // Configurar pines para el sensor ultrasónico y de humedad
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(HUMIDITY_PIN, INPUT);

  // Crear semáforo para la exclusión mutua
  semaforo = xSemaphoreCreateMutex();
  if (semaforo == NULL) {
    Serial.println("Error al crear el semáforo");
    return;
  }

  // Crear las tareas de FreeRTOS
  xTaskCreate(leerDistancia, "Tarea Distancia", 128, NULL, 1, NULL);
  xTaskCreate(leerHumedad, "Tarea Humedad", 128, NULL, 1, NULL);
  xTaskCreate(enviarBluetooth, "Tarea Bluetooth", 128, NULL, 2, NULL);
}

void loop() {
  // No hay necesidad de código en loop() porque todo está manejado por FreeRTOS
}
