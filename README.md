<div align="center">
<h1>pimu</h1>

Haz mediciones de movimiento y rotacion con los datos de giroscopio y acelerometro de un <a href="https://www.amazon.com/HiLetgo-Gyroscope-Acceleration-Accelerator-Magnetometer/dp/B01I1J0Z7Y">MPU9250</a> en un Raspberry Pi 3.
</div>

## ⚠️ Instalación
Para poder usar este proyecto en C++ o Python sigue los siguientes pasos.

### Para CPP
Este proyecto es una libreria header-only todo en uno.
Solo copia el archivo <a href="https://github.com/TeamAguarai/pimu/blob/main/pimu.hpp">pimu.hpp</a> e incluyelo en tu proyecto con directivas de preprocesador.
```cpp
#include "pimu.hpp"
```

### Para Python
Compila y crea los bindinds de C++ a Python ejecutando los archivos scripts.
```bash
git clone https://github.com/TeamAguarai/pimu
cd pimu/build
sudo python3 build_python_library.py
```
Una vez creado la libraria compartida, se instalara en tu Raspberry Pi y podras usarla.
```python
import pimu
```
