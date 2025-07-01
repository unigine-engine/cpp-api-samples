# UNIGINE C++ Samples

This repository contains [C++ UNIGINE samples](https://developer.unigine.com/en/docs/latest/sdk/api_samples/cpp/?rlang=cpp) showcasing various features and techniques implemented via code. Each sample's description is stored alongside the sample and can be viewed at runtime.

## Requirements

- [**UNIGINE SDK Browser**](https://developer.unigine.com/en/docs/latest/start/installing_sdk?rlang=cpp)
- **UNIGINE SDK Community** or **Engineering** edition (**Sim** upgrade supported)
- **Visual Studio 2022** (recommended)

## Running the Samples

1. **Clone or download** this repository.
2. **Run the `download_samples_content.py` file** to automatically download the sample content.
> [!Note]
> Only code files are included in this repository. The script will fetch the necessary assets into a separate content folder.
3. **Open SDK Browser** and make sure you have the latest version.
4. **Add the project to SDK Browser**:
   - Go to the *My Projects* tab.
   - Click *Add Existing*, select the `.project` file from the cloned folder (matching your OS - `*-win-*`/`*-lin-*`, edition, precision), and click *Import Project*.
     
     ![Add Project](https://developer.unigine.com/en/docs/latest/sdk/api_samples/third_party/photon/add_project.png)
> [!NOTE]
> If you're using **UNIGINE SDK *Sim***, select the ***Engineering*** `*-eng-sim-*.project` file when importing the sample. After import, you can upgrade the project to the **Sim** version directly in SDK Browser - just click *Upgrade*, choose the SDK **Sim** version, and adjust any additional settings you want to use in the configuration window that opens.

5. **Repair the project**:
   - After importing, you'll see a **Repair** warning - this is expected, as only essential project files are stored in the Git repository. SDK Browser will restore the rest.
   
   ![Repair Project](https://developer.unigine.com/en/docs/latest/sdk/api_samples/third_party/repair_project.png)
   - Click *Repair* and then *Configure Project*.
     
6.  **Open** the project in your IDE:

    - Launch the recommended Visual Studio 2022 (other C++ IDE with CMake support can be used as well).
    - Load the `/source` folder containing `CMakeLists.txt`. If everything is set up correctly, the `CMakeLists.txt` file will be highlighted in **bold** in the *Solution Explorer* window, indicating that the project is ready to build.

> [!WARNING]
   > By default, the project uses **single precision (float)**. To enable **double precision**, modify `CMakeLists.txt`:
   >```diff
   > - set(UNIGINE_DOUBLE False CACHE BOOL "Double coords")
   > + set(UNIGINE_DOUBLE True CACHE BOOL "Double coords")
   >```
   > Make sure this setting matches the `.project` file you selected (i.e, `*-double.project` for double precision builds).

7. **Build** and **Run** the project.

### If the sample fails to run:
- Double-check all setup steps above to ensure nothing was skipped.
- Check that `UNIGINE_DOUBLE` in `CMakeLists.txt` matches the current build type (double/float).
- Use the correct `.project` file for your SDK edition/platform.
- Ensure your SDK version is not older than the project's specified version.
- If CMake issues occur in Visual Studio, right-click the project and select: **Delete Cache and Reconfigure** and then **Build** again.