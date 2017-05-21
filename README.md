# 3D lava flow viewer

Lava flows visualization on a real topographic surface. It is an interactive 3D application which displays a topographic surface and a lava flow on it. The application is general, allowing the visualization of datasets of any size. The data is read from files. It is possible to display data from different points of view and navigate in the scene.

Compiling:
I used the LearnOpenGL kit, which you can find here: https://learnopengl.com/. It uses GLFW and other libraries, which are included in the kit. You can download, compile and run the examples following the tutorial on the website and in the README included in the kit. In particular, these are the main instructions:

  git clone --recursive [https://github.com/stephane-lb/LearnOpenGL.git]
  cd LearnOpenGL
  mkdir build
  cd build
  cmake ../.
  make -j8

To use my source files, you have to add the folder "8.lava_viewer" in "src/1.getting_started/" and modify the file CMakeLists.txt as following:

  Add "8.lava_viewer" after "7.camera" (line 71)

Since I used absolute paths in my code, please also make sure that you point to existing files.

Note: most of data allocation code is in the main function. This is not a good practice, but I was obliged because using subfunctions I experienced insufficient-memory errors. Probably OpenGL always copies function arguments. If you are running your code in a more powerful machine, you can move that code.

Developed in 2016.
