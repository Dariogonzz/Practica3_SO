#!/bin/bash

# Script para Windows (ejecutándose en Git Bash / WSL)
SERVIDOR="ServidorCamara.py"
CLIENTE="cliente.cpp"
EJECUTABLE="./cliente.exe" # En Windows el ejecutable lleva .exe

echo "--- Iniciando entorno de pruebas (Bash en Windows) ---"

# Comprobar que los archivos existan
if [ ! -f "$SERVIDOR" ]; then
    echo "Error: No encuentro el archivo $SERVIDOR."
    exit 1
fi

if [ ! -f "$CLIENTE" ]; then
    echo "Error: No encuentro el archivo $CLIENTE."
    exit 1
fi

# 1. Compilar el código C++ enlazando la librería de red de Windows (ws2_32)
echo "Compilando $CLIENTE..."
g++ $CLIENTE -o cliente.exe -lws2_32

if [ $? -ne 0 ]; then
    echo "Error de compilación. Revisa tu código C++."
    exit 1
fi

# 2. Levantar el servidor Python en segundo plano
echo "Iniciando servidor Python..."
# Usamos 'python' o 'python3' dependiendo de cómo esté mapeado en tu Windows
python $SERVIDOR &
PID_PYTHON=$!

# Pausa de 2 segundos para asegurar que el socket esté escuchando
sleep 2

# 3. Ejecutar el cliente
echo "Ejecutando cliente..."
echo "------------------------------------------------"
$EJECUTABLE
ESTADO_CLIENTE=$?
echo "------------------------------------------------"

if [ $ESTADO_CLIENTE -eq 0 ]; then
    echo "Ejecución del cliente finalizada correctamente."
else
    echo "Aviso: El cliente terminó con código de error ($ESTADO_CLIENTE)."
fi

# 4. Matar el proceso del servidor de forma segura en Windows
echo "Limpiando procesos en el puerto 12345..."
# En Windows, a veces el PID de Bash ($PID_PYTHON) no mata directamente al proceso nativo de Windows,
# por lo que es más seguro liberar el puerto buscando qué proceso lo está usando:
PID_WIN=$(netstat -aon | grep :12345 | grep LISTENING | awk '{print $5}' | head -n 1)

if [ ! -z "$PID_WIN" ]; then
    taskkill //F //PID $PID_WIN 2>/dev/null  # Doble barra necesaria en Git Bash
else
    kill -9 $PID_PYTHON 2>/dev/null
fi

echo "--- Fin del script ---"