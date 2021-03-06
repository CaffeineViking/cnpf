Curl Noise Particle Flow
========================

<p align="center">
    <img width=100% src="/docs/share/merged_shaders.png" alt="Visualization of a Curl-Noise Field"/>
</p>

Based on the *Curl-Noise for Procedural Fluid Flow* by Bridson et al. from 2007, we designed an implemented an application which enables the simulation and visualization of particles in real-time by calculating each curl-noise iteration purely on the GPU by using OpenCL kernels. We did this by generating a 3-D *Simplex noise*, and converted it to a vector field by finding the gradient in each point in space. Afterwards, we apply the curl operator on the vector field, which provides a divergence-free vector field (and no particles will therefore get stuck together at any point in space). With this, we can in each step, for every particle, calculate the new position of the particle by assuming a point in the curl-field is the velocity (e.g. in a fluid) of a particle in that position. Since we apply these steps on the GPU by using OpenCL (for the simulation) and OpenGL (for the visualization), and share the buffer state by OpenCL-OpenGL buffer interoperability, we never leave GPU-space, and can therefore do this in real-time, even when rendering a huge number of particle billboards.

You can find our *very* short article describing this technique over here: [Fluid Animation in Real-Time using 3-D Curl-Noise](https://eriksvjansson.net/papers/firmtdr.pdf)

Dependencies
--------

### For windows:

* Download and place [premake5](https://premake.github.io/download.html) in your path.
* Install [MinGW](http://www.mingw.org/) with everything except ADA and Fortan under `C:\MinGW` (should be default location).
* Download [GLM](http://glm.g-truc.net/0.9.8/index.html) and place the `glm` folder with all includes into `C:\MinGW\include`.
* Fetch the latest [OpenCL](https://www.khronos.org/registry/OpenCL/) headers and find your runtimes (usually the drivers).
* Download the pre-compiled [GLFW](https://github.com/glfw/glfw/releases/download/3.2.1/glfw-3.2.1.bin.WIN32.zip) binaries.
  * Copy the `include` directory into `C:\MinGW\include`.
  * Copy the content of `lib-mingw` into `C:\MinGW\lib` except the `glfw3.dll`which you will place under the `bin` folder for this repo.
* Download the source for [GLEW](https://sourceforge.net/projects/glew/files/glew/2.0.0/glew-2.0.0.zip/download) and copy the `compile_glew.bat` from the `misc` folder provided with this repo to the root of the glew source folder.
  * Run the bat and copy the libraries from `lib` into `C:\MinGW\lib`. Copy the `glew32.dll` into the `C:\Windows` folder. 
* Download [CUDA](https://developer.nvidia.com/cuda-downloads) for Nvidia devices.
* Copy content of `C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\lib\Win32` to `C:\MinGW\lib`.
* Copy `C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\include\CL` to to `C:\MinGW\include`.

If you encounter a run-time error about some basic_string funcitonallity, it's caused by the wrong libstdc++-6.dll is first in the path. The easiest solution is to copy it from C:\MinGW\bin to C:\Windows, or to the folder where the binaries are run from.

### For linux or bsd (maybe macos):

* Using your favorite package manager, install:
    * GLFW with development packages.
    * GLEW with development packages.
    * Maybe OpenCL hardware runtimes.
    * Oh yea, the OpenCL headers too.
    * The standard MESA schenanigans.
    * AntTweakBar by building source.
    * The Premake5 build system.
* If it doesn't work, ask Erik!!

Building
--------

1. Make sure library dependencies are in your path.
2. Place yourself in the root directory of this project.
3. Acquire the latest version of the [premake5](https://premake.github.io/download.html) build system (add it to your PATH in windows).
4. Thereafter, execute `premake5 gmake` if building on Make.
5. Finally, issue the command `make -C build` and wait.
6. When complete, either `bin` or `lib` are built.
7. Tests can be run with `bin/*-tests` program.
8. **Shortcuts**: `make run` and `make test`.

Contributing
------------

1. See if there any updates upstream: `git remote update`
2. If you are already working on a feature branch, jump to step 4.
3. Create a new branch for your feature: `git branch <feature>`
4. Change to the feature branch by: `git checkout <feature>`
5. Start/continue building the feature. Jump to 7 if done.
6. Push your changes to your remote feature branch with:
   * `git add -A` (everything) or `git add -u` (updates).
   * `git commit -m "Message describing the additions."`
   * Publish remotely: `git push origin <feature>`
   * Go back to step 1 or 5 (if working quickly).
7. Your feature is done (single/many commits).
8. Fetch the changes from upstream first:
   * Go to master: `git checkout -b master`
   * Pull changes: `git pull origin master`
8. Rebase your changes on top of `master`:
   * Back to feature: `git checkout <feature>`
   * Thereafter, issue: `git rebase master`
   * Fix any conflicts that may happen...
   * Changes now on top of the Git tree.
9. Finally, go to GitHub and issue PR.
   * Listen in on other's feedback!
   * Make changes if necessary.
10. Back to the master: `git checkout master`
11. Pat yourself on the back, then goto 1 again.

Structure
---------

* `assets`: contains all resources required at runtime (shaders, textures,...).
* `bin`: contains the built software and accompanying testing suite.
* `build`: stores intermediate object files and generated GNU Make files.
    * `obj`: has all of the generated object files given under compilation.
    * `Makefile`: automatically generated by executing `premake5 gmake`.
    * `*.make`: program specific make config for augmenting `Makefile`.
* `docs`: any generated documentation for this project is over here.
* `include`: both external and internal project headers are here.
    * `foreign`: any foreign headers which should be included.
    * `project directories`: internal headers for the project.
* `lib`: any generated libraries from the project reside here.
* `license.md`: please look through this very carefully.
* `premake5.lua`: configuration file for the build system.
* `readme.md`: this file, contains information on the project.
* `share`: any extra data that needs to be bundled should be here.
* `src`: all source code for the project should be located below here.
    * `project directories`: source code for specific project build.
    * `foreign`: any external source files which might be needed.
* `tests`: source code for the project's testing suite, using Catch syntax.
    * `project directories`: project specific testing suite for one build.
