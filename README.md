### :warning: DOES NOT WORK :warning:

Learning to 3d by doing and porting. Simple OpenGL 4.5 context with SDL2 and
GLEW 2 for cross platform Window and extension management.

This project will try and use C because it's an itch I want to scratch. This means
no copy semantics from C++ and no operator overloading.

There is no real aim other than a simple starting point for getting pixels to
the screen with a modern OpenGL pipeline.

Currently the only thing this program does is render a triangle and will exit
once the escape is pressed.


### Other Branches

- [stb_truetype](https://github.com/sbryant/3dlearns/tree/stb_truetype) -
example using the
[stb_truetype](https://github.com/nothings/stb/blob/master/stb_truetype.h)
library from Sean Barrett to render a bitmap font and textured quad

- [debug](https://github.com/sbryant/3dlearns/tree/debug) - buidling on the 
[stb_truetype](https://github.com/sbryant/3dlearns/tree/stb_truetype) branch for
rendering text with true type fonts to bake a bitmap and render characters to quads
as lines of text out to the screen. :warning:WIP:warning
