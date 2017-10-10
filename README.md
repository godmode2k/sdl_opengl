SDL OpenGL, Streaming image test
===============
> work in-progress and TEST only !!!


Prerequisites
----------
> build all and tested on Cygwin (Windows), Ubuntu Linux 16.10

    Cygwin: https://www.cygwin.com/
    SDL2: https://www.libsdl.org/
    SDL_image 2.0: https://www.libsdl.org/projects/SDL_image/
    libjpeg-turbo: https://libjpeg-turbo.org/
    test image: test.jpg

    Ubuntu Linux:
    $ apt-get install libsdl2-2.0-0 libsdl2-dev
    $ apt-get install libsdl2-image-2.0-0 libsdl2-image-dev
    $ apt-get install libjpeg-turbo8 libjpeg-turbo8-dev


Build
----------
>

    Cygwin:
    $ sh build_sdl_opengl_test_win32.sh

    Ubuntu Linux:
    $ sh build_sdl_opengl_test.sh


Run
----------

    Server-mode:
    $ ./CSDLOpenGLTest

    Client-mode:
    $ ./CSDLOpenGLTest 127.0.0.1

