import serial
import tkinter as tk
from tkinter import ttk
import threading

# Configura tu puerto serie (cambia COMx por el puerto correcto)
# Por ejemplo: COM3 (Windows), /dev/ttyUSB0 (Linux), /dev/ttyAMA0 (Raspberry Pi)
puerto_serie = "COM10"  # Cambia esto según tu configuración de puerto
baud_rate = 9600

# Crear una instancia del puerto serie
ser = serial.Serial(puerto_serie, baud_rate, timeout=1)

# Función para leer datos del puerto serie y actualizar la interfaz gráfica
def leer_datos():
    while True:
        try:
            # Leer línea desde el puerto serie e ignorar errores de decodificación
            linea = ser.readline().decode('utf-8', errors='ignore').strip()
            if linea:
                # Verificar si la línea contiene los datos esperados (Distancia y Humedad)
                if "Distancia" in linea:
                    # Dividir la línea en base al ":" y verificar si tiene suficientes partes
                    partes = linea.split(":")
                    if len(partes) > 1:  # Asegurarse de que haya al menos 2 elementos
                        distancia = partes[1].strip()
                        distancia_label.config(text=f"Distancia: {distancia}")
                elif "Humedad" in linea:
                    # Dividir la línea en base al ":" y verificar si tiene suficientes partes
                    partes = linea.split(":")
                    if len(partes) > 1:  # Asegurarse de que haya al menos 2 elementos
                        humedad = partes[1].strip()
                        humedad_label.config(text=f"Humedad: {humedad}")
        except serial.SerialException:
            break

# Crear la ventana principal de la interfaz gráfica
root = tk.Tk()
root.title("Monitor de Distancia y Humedad")

# Etiqueta para mostrar la distancia
distancia_label = ttk.Label(root, text="Esperando datos de distancia...", font=("Arial", 24))
distancia_label.pack(padx=20, pady=20)

# Etiqueta para mostrar la humedad
humedad_label = ttk.Label(root, text="Esperando datos de humedad...", font=("Arial", 24))
humedad_label.pack(padx=20, pady=20)

# Iniciar un hilo para leer datos en segundo plano
leer_datos_thread = threading.Thread(target=leer_datos, daemon=True)
leer_datos_thread.start()

# Iniciar la interfaz gráfica
root.mainloop()
