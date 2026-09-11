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

## Porting notes

Most of the work in getting this running again was not the platform. Four
defects were latent bugs that Linux, libstdc++ or an older dependency simply
tolerated:

- **GLM 0.9.9 stopped initialising default-constructed types.** The code
  declares `mat4 res;` and compounds into it. Built without
  `GLM_FORCE_CTOR_INIT`, every bone transform starts from stack garbage and
  skinned meshes collapse or mangle. The flag is set in both Makefiles.
- **GLM fixed `decompose`'s conjugated quaternion.** `GameObject`'s
  `setLocal*` accessors conjugated the result to undo that bug, which now
  inverts the rotation instead of preserving it.
- **libc++ does not tolerate incrementing a map iterator past `end()`.**
  `Skeleton::renderBonesMatrices` did; libstdc++ let it slide.
- **`errno` values are not portable.** `EAGAIN` is 11 on Linux and 35 here,
  and one comparison used the literal.

Two were genuine platform gaps: `bind` resolving to `std::bind` under
`using namespace std;`, and macOS refusing a core context without
`GLFW_OPENGL_FORWARD_COMPAT`.

Dependency version differences worth knowing:

- **assimp 5+ invents FBX bone-tip bones** with no vertex weights. The
  humanoid reports 51 bones against 32 real ones, which is why
  `MAX_BONES_AMOUNT` was exactly 50.
- **Bullet 3.25 needed no source changes**; it still ships the Bullet 2 API.

### Known issues

- `Atmosphere::updateSunPos` rotates per frame, not per unit time, so the
  day/night rate follows the framerate.
- The rifle's FBX assigns its *metalness* texture to the NormalMap slot and
  never references `ar15_Material_Normal.tga.png`. An authoring bug in the
  asset, not an importer one; the renderer consequently has no metalness map
  for it.
- Apple's GL driver warns once that the `GL_R8` static-depth G-buffer
  attachment is "unloadable". The framebuffer passes its completeness check
  and the game renders correctly.
