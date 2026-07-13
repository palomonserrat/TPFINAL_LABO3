# VideoDSP — Procesamiento de video en tiempo real (TP Final Labo 3)

El presente proyecto corresponde al trabajo final de la materia Laboratorio de Electrónica III (25.24). A lo largo del mismo del mismo se desarrolla un sistema de procesamiento digital de señales de video en tiempo real utilizando una webcam, Python/OpenCV y una estructura de procesamiento basada en C++.
En este tratamiento cada frame adquirido es tratado como una se˜nal bidimensional discreta, sobre la cual se implementan filtros digitales espaciales y frecuenciales. De este modo, el sistema permite aplicar distintos efectos de procesamiento, tales como suavizado, realce de bordes, filtrado pasa-bajos/pasa-altos y eliminación de interferencias periódicas mediante filtros notch en el dominio de la FFT 2D.

![alt text](Messi_procesado.png)

## Estructura del repositorio

```
.
├── .gitignore
├── .vs/
├── .vscode/
├── CMakeLists.txt
├── CMakePresets.json
├── Informe.pdf          # Informe del proyecto
├── Messi.jpg            # Imagen ilustrativa
├── README.md
├── WindowsSetUp.md
├── build/               # Directorio de salida de la compilación (CMake)
├── include/
│   ├── camera.hpp
│   ├── spatialFilters.hpp
│   └── videoProcessor.hpp
├── out/                 # Archivos de salida temporales
├── src/
│   ├── main.cpp
│   ├── camera.cpp
│   ├── videoProcessor.cpp
│   ├── spatialFilters.cpp
│   └── frequencyFilters.cpp
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
