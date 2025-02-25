#!/bin/bash
# CHATGPT-GENERATED
# ------------------------------------------------------------
# simplify: Unifica archivos .cpp y .h de un proyecto de C++ en un solo archivo .cpp y otro .h.
# Uso:
#   ./simplify <CARPETA_CPP> <CARPETA_H> <NOMBRE_ARCHIVO_UNIFICADO> <CARPETA_SALIDA>
# Ejemplo:
#   ./simplify src inc proyecto output
#   Resultado:
#       output/proyecto.cpp
#       output/proyecto.h
# ------------------------------------------------------------

if [ "$#" -lt 4 ]; then
    echo "Uso: $0 <CARPETA_CPP> <CARPETA_H> <NOMBRE_ARCHIVO_UNIFICADO> <CARPETA_SALIDA>"
    exit 1
fi

CPP_DIR=$1
H_DIR=$2
UNIFIED_NAME=$3
OUTPUT_DIR=$4

COMBINED_CPP="$OUTPUT_DIR/$UNIFIED_NAME.cpp"
COMBINED_H="$OUTPUT_DIR/$UNIFIED_NAME.h"

# Crear el directorio de salida si no existe
mkdir -p "$OUTPUT_DIR"

# --- Procesar archivos .h ---
# Resolver el orden de los archivos de encabezado
declare -A dependencies
for file in "$H_DIR"/*.h; do
    filename=$(basename "$file")
    includes=$(grep -oP '#include\s+"(.*?)"' "$file" | cut -d'"' -f2)
    dependencies["$filename"]="$includes"
done

resolved=()        # Archivos ya ordenados
unresolved=()      # Archivos en proceso de resolución

resolve_dependencies() {
    local file=$1
    if [[ " ${resolved[@]} " =~ " $file " ]]; then
        return
    fi
    if [[ " ${unresolved[@]} " =~ " $file " ]]; then
        echo "Error: Dependencias circulares detectadas en $file."
        exit 1
    fi

    unresolved+=("$file")

    for dep in ${dependencies["$file"]}; do
        dep_file=$(basename "$dep")
        if [[ -f "$H_DIR/$dep_file" ]]; then
            resolve_dependencies "$dep_file"
        fi
    done

    unresolved=("${unresolved[@]/$file}")
    resolved+=("$file")
}

for file in "${!dependencies[@]}"; do
    resolve_dependencies "$file"
done

# Crear el archivo combinado .h
> "$COMBINED_H"
echo "Orden de archivos combinados en $UNIFIED_NAME.h:"
for file in "${resolved[@]}"; do
    echo "$file"
    cat "$H_DIR/$file" >> "$COMBINED_H"
    echo "" >> "$COMBINED_H"
done

# --- Procesar archivos .cpp ---
> "$COMBINED_CPP"
echo "#include \"$UNIFIED_NAME.h\"" > "$COMBINED_CPP"

echo "Orden de archivos combinados en $UNIFIED_NAME.cpp:"
for file in "${resolved[@]}"; do
    cpp_file="$CPP_DIR/${file%.*}.cpp"
    if [[ -f "$cpp_file" ]]; then
        echo "$cpp_file"
        cat "$cpp_file" >> "$COMBINED_CPP"
        echo "" >> "$COMBINED_CPP"
    else
        echo "Advertencia: No se encontró el archivo $cpp_file para $file"
    fi

done

# Mensaje de éxito
echo "Archivos combinados creados en la carpeta '$OUTPUT_DIR':"
echo " - $UNIFIED_NAME.h"
echo " - $UNIFIED_NAME.cpp"
