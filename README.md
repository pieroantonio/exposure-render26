
# Exposure Render

Exposure Render is a CUDA based volume raycaster, enhanced with physically based light transport. More information about the rendering algorithm can be found here:
* [An interactive photo-realistic volume rendering framework](http://graphics.tudelft.nl/Publications/kroes_exposure_2012)  
T. Kroes, F. H. Post, C. P. Botha
* [Visibility sweeps for joint-hierarchical importance sampling of direct lighting for stochastic volume rendering](http://graphics.tudelft.nl/Publications-new/2015/KEE1)  
T. Kroes, M. Eisemann, E. Eisemann

The latest windows executable can be found [here](https://github.com/ThomasKroes/exposure-render/releases/tag/1.1.0)
 repository and follow  [these]() instructions 
![https://graphics.tudelft.nl/publications/](/Images/manix.png)

## Building Exposure Render from source code
If you are eager to build Exposure Render yourself you should clone the release repository. Latest build instructions (tested with Visual Studio 2017, Qt5.11.1, VTK 7.1.1 and Cuda 9.2) can be found [here](https://github.com/ThomasKroes/exposure-render.release110/blob/qt5_vtk7_cuda9_vs_2017/build.md).

To build with newer CUDA toolkits, update your `EXPOSURE_RENDER_CUDA_ARCHITECTURES` cache entry in CMake (default targets include modern Ampere/Ada/Hopper architectures) and point CMake at the desired CUDA toolkit installation.

### Windows 11 (Visual Studio Code + CUDA 12 + RTX 4070 laptop)
1. Install **Visual Studio Build Tools** (or full Visual Studio) with the "Desktop development with C++" workload.
2. Install **CMake** and **Ninja** (or use the VS generator).
3. Install the **CUDA toolkit** (latest) and confirm `nvcc --version` works.
4. Install **GDCM** (for direct DICOM loading). Using vcpkg:
   ```
   vcpkg install gdcm
   ```
5. Configure the build (PowerShell):
   ```
   cmake -S . -B build -G "Visual Studio 17 2022" -A x64 ^
     -DEXPOSURE_RENDER_USE_GDCM=ON ^
     -DCMAKE_TOOLCHAIN_FILE=C:\path\to\vcpkg\scripts\buildsystems\vcpkg.cmake
   ```
6. Build:
   ```
   cmake --build build --config Release
   ```

## System requirements

* Microsoft Windows XP, Vista, or 7.
* At least 1GB of system memory.
* NVIDIA CUDA-compatible GPU with compute capability 5.2+ and sufficient VRAM for your volume sizes.
* Larger data sets are supported by 64-bit buffer sizing, but rendering is still bounded by available GPU memory.

## DICOM input
Exposure Render's core library focuses on GPU volume rendering. A direct DICOM series loader is available when built with GDCM (`-DEXPOSURE_RENDER_USE_GDCM=ON`). Use `DicomSeriesReader::LoadSeries` to populate a `Buffer3D<unsigned short>` and spacing for rendering.

## Modernization roadmap (suggested)
If you plan to modernize this codebase further, the following areas are the highest-impact upgrades:

1. **Build system**
   * Move to a newer CMake baseline and target-based configuration.
   * Replace legacy `FindCUDA` usage with modern `FindCUDAToolkit`.
2. **Dependencies**
   * Update VTK/Qt to currently supported versions.
   * Align CUDA toolkit and GPU drivers with your target hardware.
3. **Data ingest**
   * Add a first-class DICOM ingestion path (e.g., a small loader module based on VTK/ITK/GDCM) so datasets can be opened without a separate conversion step.
4. **Large datasets**
   * Introduce streaming/tile-based volume rendering or out-of-core caching to render volumes larger than GPU VRAM.

## Developer(s)

Thomas Kroes

Affiliations:

**Delft University of Technology (TU Delft)**  
Computer Graphics and Visualization (CGV)  
*t.kroes at tudelft.nl*

**Leids Universitair medisch centrum (LUMC)**  
Laboratorium voor Klinische en Experimentele Beeldverwerking (LKEB)  
*t.kroes at lumc.nl*

## Acknowledgements

* Osirix Imaging Software for sharing the medical data sets
* Volvis website for the engine and bonsai data set
* Fugue icons for the icon database

## Tested system configurations

Exposure Render has been tested on the following system configurations using Nvidia hardware:

* Windows 7 (64 bit) + Quadro FX1700
* Windows 7 (64 bit) + GTS240
* Windows 7 (64 bit) + GTS250
* Windows 7 (64 bit) + GTS450
* Windows 7 (64 bit) + GTX260
* Windows 7 (64 bit) + GTX270
* Windows 7 (64 bit) + GTX460
* Windows 7 (64 bit) + GTX470
* Windows 7 (64 bit) + GTX560
* Windows 7 (64 bit) + GTX570
* Windows 7 (64 bit) + GTX580

*Please mention your complete system setup when you report a bug: (OS (32/64 bit), graphics card, driver version etc.), possibly along with screen shots and error messages. Help make Exposure Render stable!*
