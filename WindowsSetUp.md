# Setup en Windows — VideoDSP

Guía paso a paso para dejar el proyecto compilando en una PC con Windows nueva. Seguí los pasos en orden, no te saltees ninguno.

## 0. Requisitos previos

- Git instalado
- VS Code instalado
- Conexión a internet estable (vas a descargar/compilar OpenCV, pesa varios GB y tarda 15-25 min)

## 1. Clonar el repo

```powershell
git clone <URL-DEL-REPO>
cd <carpeta-del-repo>
```

## 2. Instalar Visual Studio Build Tools (el compilador)

**Importante: tiene que ser MSVC, no MinGW/g++.** Si ya tenés Visual Studio Community instalado con el workload de C++, saltá este paso.

1. Descargar de: https://visualstudio.microsoft.com/downloads/
2. Bajar hasta "Tools for Visual Studio" → **"Build Tools for Visual Studio 2022"**
3. En el instalador, tildar el workload **"Desktop development with C++"**
4. Instalar (puede tardar un rato)

## 3. Instalar vcpkg

Abrí PowerShell y corré:

```powershell
git clone https://github.com/microsoft/vcpkg C:\dev\vcpkg
C:\dev\vcpkg\bootstrap-vcpkg.bat
```

(Si preferís instalarlo en otra ruta, está bien, solo acordate cuál usaste para el paso 5.)

## 4. Instalar OpenCV con vcpkg

```powershell
C:\dev\vcpkg\vcpkg.exe install opencv:x64-windows
```

**Esto tarda 15-25 minutos**, compila OpenCV desde cero. Dejalo correr, no lo canceles.

⚠️ Tiene que ser exactamente `opencv:x64-windows` (no `x86-windows`, no `x64-mingw-dynamic`). Otros triplets están probados que no funcionan con este proyecto.

## 5. Setear la variable de entorno VCPKG_ROOT

```powershell
setx VCPKG_ROOT "C:\dev\vcpkg"
```

(Si instalaste vcpkg en otra ruta en el paso 3, poné esa ruta acá en vez de `C:\dev\vcpkg`.)

**Después de correr esto, cerrá TODAS las ventanas de PowerShell y de VS Code.** La variable no se actualiza en ventanas ya abiertas.

## 6. Instalar extensiones de VS Code

Abrí VS Code y en el panel de extensiones (`Ctrl+Shift+X`) instalá:

- **C/C++** (de Microsoft)
- **CMake Tools** (de Microsoft)

## 7. Abrir el proyecto

```powershell
cd <carpeta-del-repo>
code .
```

## 8. Seleccionar el kit y el preset

1. `Ctrl+Shift+P` → **"CMake: Select a Kit"** → elegir el que diga algo como **"Visual Studio Community 2022 Release - amd64"** o **"Visual Studio Build Tools 2022 Release - amd64"**.
   - Si no aparece ningún kit de Visual Studio en la lista, correr primero `Ctrl+Shift+P` → **"CMake: Scan for Kits"** y volver a intentar.
2. `Ctrl+Shift+P` → **"CMake: Select Configure Preset"** → elegir **`windows-vcpkg`**.

## 9. Compilar

`Ctrl+Shift+P` → **"CMake: Build"**

Si todo salió bien, en la terminal de salida debería verse algo como:

```
-- The CXX compiler identification is MSVC ...
-- Found OpenCV: C:/dev/vcpkg/installed/x64-windows (found version "4.7.0")
-- Configuring done
-- Generating done
```

y al final `VideoDSP.vcxproj -> ...\build\Debug\VideoDSP.exe`

## 10. Correr

```powershell
build\Debug\VideoDSP.exe
```

Controles: `0` gris original, `1` box blur, `q`/`ESC` salir.

---

## Si algo falla

### "Could not find a package configuration file for OpenCV"
Significa que no está usando el toolchain de vcpkg. Revisar:
- ¿Corriste `setx VCPKG_ROOT` y **reiniciaste VS Code completo** después?
- ¿El preset seleccionado (paso 8.2) es `windows-vcpkg` y no `default`?
- Probar: `Ctrl+Shift+P` → **"CMake: Delete Cache and Reconfigure"**

### Errores de link tipo `undefined reference to cv::Mat::Mat()`
Significa que se está compilando con MinGW/g++ en vez de MSVC. Revisar que el kit elegido en el paso 8.1 sea de Visual Studio, no "GCC" ni nada de MinGW/Strawberry.

### No aparece ningún kit de Visual Studio
`Ctrl+Shift+P` → **"CMake: Scan for Kits"**. Si sigue sin aparecer, reinstalar Build Tools verificando que el workload "Desktop development with C++" haya quedado tildado.

### Cualquier otro error
Mandar el output completo de la terminal (no solo el error final, todo desde donde empezó a correr `CMake: Build`).