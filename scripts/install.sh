#!/bin/bash
# CHATGPT-GENERATED
# ------------------------------------------------------------
# install: Compila e instala una librería de C++ en un Raspberry Pi.
# Uso:
#   ./install <SOURCE_CPP> <HEADER_FILE> <LIBRARY_NAME>
# Ejemplo:
#   ./install simplified/proyecto.cpp simplified/proyecto.h pampas
#   Resultado:
#       - Librería instalada en /usr/lib/libpampas.so
#       - Encabezado instalado en /usr/local/include/pampas.h
# ------------------------------------------------------------

if [ "$#" -lt 3 ]; then
    echo "Uso: $0 <SOURCE_CPP> <HEADER_FILE> <LIBRARY_NAME>"
    exit 1
fi

# Parámetros
COMBINED_SRC=$1
COMBINED_HDR=$2
LIBRARY_NAME=$3
OBJ="build/${LIBRARY_NAME}.o"
LIBRARY_SO="build/lib${LIBRARY_NAME}.so"

# Crear directorio de compilación si no existe
mkdir -p build

# --- Compilar la librería ---
echo "Compilando la librería..."
g++ -fPIC -Ibuild -c "$COMBINED_SRC" -o "$OBJ" -Iinclude -lwiringPi -DPAMPAS_DEV
if [ $? -ne 0 ]; then
    echo "Error: Fallo al compilar $COMBINED_SRC"
    exit 1
fi

g++ -shared -o "$LIBRARY_SO" "$OBJ"
if [ $? -ne 0 ]; then
    echo "Error: Fallo al crear la librería compartida $LIBRARY_SO"
    exit 1
fi

echo "Librería compilada exitosamente: $LIBRARY_SO"

# --- Instalar la librería ---
echo "Instalando la librería..."
sudo mv -f "$LIBRARY_SO" /usr/lib/
sudo cp "$COMBINED_HDR" /usr/local/include/${LIBRARY_NAME}.h
if [ $? -eq 0 ]; then
    echo "Librería instalada exitosamente."
    echo " - /usr/lib/lib${LIBRARY_NAME}.so"
    echo " - /usr/local/include/${LIBRARY_NAME}.h"
else
    echo "Error: Fallo al instalar la librería."
    exit 1
fi
