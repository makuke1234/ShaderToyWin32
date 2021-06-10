# ShaderToyWin32
![Release version](https://img.shields.io/badge/release-v1.0.0-green.svg)

An OpenGL toy screen saver to test fragment shaders. Everything has been written in pure C, only Win32 API has been utilised to make it work. All OpenGL functions that must be linked, are linked at runtime, thus no OpenGL library is needed.


# Get started

To try it out download the 32-bit version of the screen saver from [here](https://github.com/makuke1234/ShaderToyWin32/raw/main/ShaderScreenSaver_gl.scr) or 64-bit version from [here](https://github.com/makuke1234/ShaderToyWin32/raw/main/ShaderScreenSaver_gl64.scr) and the testing fragment shader for it from [here](https://raw.githubusercontent.com/makuke1234/ShaderToyWin32/main/shader.glsl).

You can use any shader for it as long as the name of the shader file is `shader.glsl`.


# Examples

There are some shaders to "play" with in the subfolder [testingshaders](https://github.com/makuke1234/ShaderToyWin32/tree/main/testingshaders), these originate from [Shadertoy](https://www.shadertoy.com/) and have been modified to work with this shader engine.


# License

As stated, the project uses MIT License.
