#!/bin/bash
# ------------------------------------------------------------
# wiringPi.sh: instala wiringPi en tu Raspberry Pi en caso de no tenerlo
# ------------------------------------------------------------

if command -v gpio 2>&1 >/dev/null
then
    echo "WiringPi está instalado. (Omitiendo su instalacion)"
else
    echo "WiringPi no está instalado. Instalando ahora...."
    sudo apt-get purge wiringpi
    hash -r
    git clone https://github.com/WiringPi/WiringPi.git
    cd WiringPi
    ./build
fi
