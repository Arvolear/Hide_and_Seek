# Building on macOS (Apple Silicon)

Tested on macOS 26 / M1 Pro with Apple clang and Homebrew.

## Dependencies

Most come from Homebrew:

```
brew install bullet tinyxml2 assimp glfw glew glm
```

SOIL2 is not packaged anywhere, so build it once from source. Only two of its
functions are used (`SOIL_load_image`, `SOIL_free_image_data`), so the five C
files can be compiled directly — no CMake needed:

```
git clone --depth 1 https://github.com/SpartanJ/SOIL2.git
cd SOIL2/src/SOIL2
for f in SOIL2 image_DXT image_array image_helper wfETC; do
    clang -c $f.c -o $f.o -O2 -fPIC -I.
done
ar rcs libsoil2.a *.o
mkdir -p ~/.local/lib ~/.local/include/SOIL2
cp libsoil2.a ~/.local/lib/
cp SOIL2.h image_array_helper.h ~/.local/include/SOIL2/
```

`nv_dds` needs no setup — it is vendored under `client/code/vendor/nv_dds`
and built as part of the client. See its `README.vendor.md`.

## Build

```
cd server && make
cd client && make
```

## Run

Start the server first, then the client, each from its own directory so the
relative `levels/` path resolves:

```
cd server && ./server          # listens on :5040
cd client && ./Hide_and_Seek   # ENTER starts the game, ESC quits
```

## Platform notes

- The bundled `lib/` directories were removed. They held Linux x86-64 ELF
  objects and cannot link here.
- OpenGL and windowing come from the OpenGL, Cocoa and IOKit frameworks.
  There is no `-lGL`, no `-lX11` (the code never referenced X11) and no
  `-ldl` (dlopen is in libSystem).
- macOS caps OpenGL at 4.1 and only grants a core context when
  `GLFW_OPENGL_FORWARD_COMPAT` is set. The renderer targets GL 3.3 /
  GLSL 330, which is within that ceiling.
- Apple silicon does expose `GL_EXT_texture_compression_s3tc`, so the
  DXT-compressed DDS textures load natively.
