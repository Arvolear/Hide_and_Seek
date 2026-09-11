# nv_dds (vendored)

Source: https://github.com/paroj/nv_dds
Commit: 5a9fc3f2f1c27d7973984e839cae95a8aea60ea6

A maintained fork of NVIDIA's classic nv_dds DDS loader, preserving the
original `CDDSImage` / `CSurface` API that this project uses.

Not available through Homebrew or any other package manager, so it is
vendored here.

## Local modifications

- `nv_dds.h`: the GL header include now resolves to `<GL/glew.h>` instead of
  `<OpenGL/gl.h>` / `<GL/gl.h>`. The project loads GL through GLEW, and on
  macOS the legacy `<OpenGL/gl.h>` does not declare the S3TC compressed
  format enums that `get_format()` returns.
