# VideoDSP — Procesamiento de video en tiempo real (TP Final Labo 3)

Proyecto de procesamiento digital de señales (2D) sobre video en vivo. Combina:

- Un programa en **C++/OpenCV** que captura la cámara y aplica filtros en tiempo real.
- Un set de scripts en **Python** para prototipar y validar los filtros (respuesta en frecuencia, cuantización) antes de llevarlos al código en tiempo real.

La idea general del TP: diseñar un filtro espacial (kernel), cuantizarlo a punto fijo (para que sea barato de correr en tiempo real), validar en Python que la cuantización no rompe la respuesta en frecuencia deseada, y después correrlo en C++ sobre el video de la cámara.

## Estado actual

### C++ (`src/`, `include/`) — pipeline de video en tiempo real

- **`Camera`** (`camera.hpp/.cpp`): wrapper de `cv::VideoCapture`. Abre la cámara por índice y fija ancho/alto.
- **`VideoProcessor`** (`videoProcessor.hpp/.cpp`): convierte el frame a escala de grises y despacha según el `mode`:
  - `mode 0`: gris sin procesar.
  - `mode 1`: box blur 5x5 en Q1.15 (`SpatialFilters::boxBlurQ15`).
- **`SpatialFilters::boxBlurQ15`** (`spatialFilters.cpp`): convolución 5x5 manual, en aritmética de punto fijo Q1.15 (coeficientes `int16_t`, acumulador `int32_t`, `border = clamp` / replicación de borde). El kernel está corregido en DC para que la suma dé exactamente `2^15`.
- **`main.cpp`**: loop principal — muestra `Original` y `Procesado`, y permite cambiar de modo con las teclas `0`/`1`, salir con `q` o `ESC`.
- **`frequencyFilters.cpp`**: **vacío, todavía no implementado**. Es el próximo paso natural (ver Roadmap).

### Python (`python/`) — prototipado y validación (offline, no tiempo real)

- **`kernels.py`**: genera el kernel box blur en float, y lo cuantiza a Q1.15 (`quantize_q15`), devolviendo tanto los coeficientes enteros como la reconstrucción en float para comparar el error de cuantización.
- **`frequency_response.py`**: calcula la respuesta en frecuencia 2D de un kernel vía FFT (con zero-padding a `fft_size`), y tiene helpers para graficar el kernel espacial, el mapa de magnitud en dB, y un corte central `H(ωx, ωy=0)`.
- **`test_kernels.py`**: script "main" que ata todo: genera el kernel, lo cuantiza, imprime el error de cuantización, y grafica kernel + respuesta en frecuencia (float vs. Q1.15) para comparar visualmente.

**En criollo:** el flujo pensado es *Python decide el filtro y valida que la cuantización no lo arruina* → *C++ lo corre en vivo sobre la cámara*. Hoy sólo existe el filtro espacial (box blur); el filtrado en frecuencia todavía no se trasladó a C++.

## Estructura del repo

```
.
├── CMakeLists.txt
├── CMakePresets.json
├── include/
│   ├── camera.hpp
│   ├── spatialFilters.hpp
│   └── videoProcessor.hpp
├── src/
│   ├── main.cpp
│   ├── camera.cpp
│   ├── videoProcessor.cpp
│   ├── spatialFilters.cpp
│   └── frequencyFilters.cpp     # vacío, placeholder
└── python/
    ├── kernels.py
    ├── frequency_response.py
    ├── test_kernels.py
    └── requirements.txt
```

## Requisitos

**C++:**
- CMake ≥ 3.19 (por `CMakePresets.json`)
- Compilador C++17
  - Linux/macOS: g++/clang, cualquiera sirve.
  - **Windows: tiene que ser MSVC** (Visual Studio). Ver nota de compatibilidad más abajo — con MinGW/Strawberry Perl el link falla.
- OpenCV (probado con la serie 4.x)

**Python:**
- Python ≥ 3.9
- `numpy`, `matplotlib` (ver `python/requirements.txt`)

## ⚠️ Nota de compatibilidad importante (Windows)

En Windows, **el compilador con el que armás tu ejecutable tiene que ser el mismo tipo con el que está compilado OpenCV**. Esto no es específico de este proyecto, es una limitación general de C++ en Windows: MSVC y MinGW no son binariamente compatibles (ABI distinta), así que mezclarlos rompe el link con errores tipo `undefined reference to cv::Mat::Mat()` aunque CMake encuentre las librerías sin problema.

En criollo:
- Si instalás OpenCV con vcpkg usando el triplet **`x64-windows`** (el estándar, recomendado), **tenés que compilar tu proyecto con MSVC**, no con MinGW/g++ (aunque lo tengas instalado por otro lado, ej. Strawberry Perl).
- El triplet `x64-mingw-dynamic` de vcpkg (para compilar OpenCV con MinGW) es "community" y no está garantizado que compile — en la práctica nos falló al buildear OpenCV.

**Recomendación: usar siempre MSVC en Windows.** Ver instrucciones de instalación abajo.

## Cómo compilar y correr (multiplataforma)

### Linux

```bash
sudo apt install libopencv-dev
cmake --preset default
cmake --build build
./build/VideoDSP
```

### macOS

```bash
brew install opencv
cmake --preset default
cmake --build build
./build/VideoDSP
```

### Windows

1. **Instalar vcpkg** (si no lo tenés):
   ```powershell
   git clone https://github.com/microsoft/vcpkg C:\dev\vcpkg
   C:\dev\vcpkg\bootstrap-vcpkg.bat
   ```

2. **Instalar OpenCV con el triplet x64-windows** (importante: no usar x86-windows ni x64-mingw-dynamic, ver nota de compatibilidad arriba):
   ```powershell
   C:\dev\vcpkg\vcpkg.exe install opencv:x64-windows
   ```
   Tarda bastante (15-25 min), compila OpenCV desde cero.

3. **Instalar Visual Studio Build Tools** (si no tenés Visual Studio):
   Descargar de https://visualstudio.microsoft.com/downloads/ → "Build Tools for Visual Studio" → tildar el workload **"Desktop development with C++"**.

4. **Setear la variable de entorno `VCPKG_ROOT`** (una sola vez, para que `CMakePresets.json` la encuentre sin hardcodear tu ruta):
   ```powershell
   setx VCPKG_ROOT "C:\dev\vcpkg"
   ```
   Cerrá y volvé a abrir la terminal/VS Code después de esto para que tome efecto.

5. **Compilar** desde la terminal **"x64 Native Tools Command Prompt for VS 2022"** (buscala en el menú inicio — es la que tiene el compilador MSVC en el PATH):
   ```cmd
   cmake --preset windows-vcpkg
   cmake --build build --config Debug
   ```

6. **Correr**:
   ```cmd
   build\Debug\VideoDSP.exe
   ```

### Controles en ejecución

- `0`: ver original en gris
- `1`: ver box blur 5x5 Q1.15
- `q` / `ESC`: salir

## Trabajar desde VS Code (Windows)

Para no tener que abrir la terminal especial de Visual Studio cada vez:

1. Instalar las extensiones **C/C++** y **CMake Tools** (ambas de Microsoft).
2. Asegurarte de haber seteado `VCPKG_ROOT` como variable de entorno del sistema (ver paso 4 arriba) y de tener instalado Visual Studio / Build Tools con el workload C++.
3. Abrir la carpeta del proyecto en VS Code.
4. `Ctrl+Shift+P` → **"CMake: Select a Kit"** → elegir el kit de Visual Studio (ej. "Visual Studio Community 2022 Release - amd64"). CMake Tools arma el entorno de MSVC automáticamente, no hace falta la terminal especial.
5. `Ctrl+Shift+P` → **"CMake: Select Configure Preset"** → elegir `windows-vcpkg`.
6. `Ctrl+Shift+P` → **"CMake: Build"** (o el botón de build en la barra de estado inferior).
7. `F5` compila y corre con el debugger conectado.

En Linux/macOS los mismos pasos aplican pero eligiendo el preset `default` y cualquier kit de GCC/Clang detectado.

### Python (prototipado offline)

```bash
cd python
pip install -r requirements.txt
python test_kernels.py
```

Esto imprime el kernel float, su versión Q1.15, el error de cuantización, y abre las figuras de respuesta en frecuencia.

## Roadmap / cómo seguir

1. **Implementar `frequencyFilters.cpp`**: llevar al C++ un filtrado en el dominio de la frecuencia (FFT2D con OpenCV `cv::dft`), análogo a lo que hoy sólo se prototipa en `frequency_response.py`. Definir si va a ser un filtro pasa-bajos/pasa-altos configurable, y agregar un nuevo `mode` en `VideoProcessor::process` para probarlo en vivo.
2. **Generalizar `boxBlurQ15`**: hoy el kernel 5x5 y el formato Q1.15 están hardcodeados. Se podría parametrizar tamaño de kernel y formato Qm.n, reusando `python/kernels.py` para generar los coeficientes y pegarlos (o cargarlos desde archivo) en C++.
3. **Puente Python ↔ C++**: hoy son dos mundos separados (Python prototipa, C++ corre). Una mejora es exportar los coeficientes cuantizados desde Python a un archivo (`.h` o `.json`) que C++ lea, para no tener que copiar números a mano entre `kernels.py` y `spatialFilters.cpp`.
4. **Portabilidad de cámara**: `Camera` abre por índice fijo (0) y fuerza resolución 640x480; estaría bueno loguear si el `set()` de resolución realmente fue aceptado por el driver, y permitir elegir cámara/resolución por línea de comandos.
5. **Tests**: no hay tests automatizados ni en C++ ni en Python. Al menos para `kernels.py`/`frequency_response.py` (que son puramente numéricos) sería fácil agregar unit tests con `pytest`.
6. **CI opcional**: si el repo va a vivir en GitHub, un workflow simple que compile el C++ (con OpenCV vía apt) y corra los tests de Python en cada push ayudaría a detectar roturas de portabilidad como las que motivaron este README.

## Notas de portabilidad (qué se corrigió)

- `CMakeLists.txt`: se sacó el `set(OpenCV_DIR "C:/Users/palom/...")` hardcodeado. Ahora `find_package(OpenCV REQUIRED)` busca la instalación del sistema.
- `CMakePresets.json`: nuevo. Define un preset `default` (Linux/macOS, OpenCV del sistema) y `windows-vcpkg` (Windows, MSVC + vcpkg vía la variable de entorno `VCPKG_ROOT`, sin rutas hardcodeadas de ninguna PC en particular).
- `src/camera.cpp`: `cv::CAP_DSHOW` sólo se usa si se compila en Windows (`#ifdef _WIN32`); en otras plataformas se abre la cámara con el backend por defecto de OpenCV.
- `.vscode/launch.json`: se cambiaron las rutas absolutas (`c:/Users/palom/Desktop/TPFINAL_LABO3`) por `${workspaceFolder}`, que es la carpeta del proyecto abierta en VSCode, sea cual sea la compu.
- `.vscode/settings.json`: quedó tal cual porque es específico de la extensión C_Cpp_Runner y de tu toolchain local (esto no rompe el build con CMake de línea de comandos; si otra persona usa otro compilador/IDE, puede ignorar esta carpeta sin problema).