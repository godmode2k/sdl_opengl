#ifndef __CSDL_OPENGL_TEST_H__
#define __CSDL_OPENGL_TEST_H__

/* --------------------------------------------------------------
Project:	SDL OpenGL, streaming image Test
Purpose:	TEST only !!!
Author:		Ho-Jung Kim (godmode2k@hotmail.com)
Date:		Since Oct 7, 2017
Filename:	CSDLOpenGLTest.h

Last modified: Oct 10, 2017
License:

*
* Copyright (C) 2015 Ho-Jung Kim (godmode2k@hotmail.com)
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
-----------------------------------------------------------------
Note:
-----------------------------------------------------------------
1. Build:
	$ gcc -o ... -Wall -O2 -lSDL
-------------------------------------------------------------- */



//! Header and Definition
// ---------------------------------------------------------------
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cmath>

/*
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_video.h>
*/

#ifdef __WIN32_CYGWIN__
#define SDL_MAIN_HANDLED
	#ifdef __SDLv1__
		#include <SDL/SDL.h>
		#include <SDL/SDL_mixer.h>
		#include <SDL/SDL_video.h>
		#include <SDL/SDL_image.h>
		#include <SDL/SDL_opengl.h>
		//#include <SDL/SDL_opengles.h>
	#elif __SDLv2__
		#include <SDL2/SDL.h>
		#include <SDL2/SDL_mixer.h>
		#include <SDL2/SDL_video.h>
		#include <SDL2/SDL_image.h>
		#include <SDL2/SDL_opengl.h>
		//#include <SDL2/SDL_opengles.h>
	#else
	#endif
#else
	#ifdef __SDLv1__
		#include <SDL/SDL.h>
		#include <SDL/SDL_mixer.h>
		#include <SDL/SDL_video.h>
		#include <SDL/SDL_image.h>
		#include <SDL/SDL_opengl.h>
		//#include <SDL/SDL_opengles.h>
	#elif __SDLv2__
		#include <SDL2/SDL.h>
		#include <SDL2/SDL_mixer.h>
		#include <SDL2/SDL_video.h>
		#include <SDL2/SDL_image.h>
		#include <SDL2/SDL_opengl.h>
		//#include <SDL2/SDL_opengles.h>
	#else
	#endif
#endif

#include <errno.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


#include <turbojpeg.h>
// ---------------------------------------------------------------



//! Definition
// ---------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#define WINDOW_WIDTH				640
#define WINDOW_HEIGHT				480
#define IMAGE_BPP					3		// byte

#define DEFAULT_IPV4_ADDR			"127.0.0.1"
#define DEFAULT_PORT				8080
#define DEFAULT_BACKLOG				10
#define DEFAULT_RECV_BUF_SIZE		1024
#define DEFAULT_DATA_BUF_SIZE		(1024 * 500)
// ---------------------------------------------------------------



//! Prototype
// ---------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
// htof(), ntohf()
// Source: https://beej.us/guide/bgnet/examples/pack.c
uint32_t htonf(float f);
float ntohf(uint32_t p);
int test_htonf_ntohf(void);
#ifdef __cplusplus
}
#endif
// ---------------------------------------------------------------



//! Class
// ---------------------------------------------------------------
class CSDLOpenGLTest {
private:
	const char* TAG;

	// test only
	unsigned char m_image_buffer[WINDOW_WIDTH * WINDOW_HEIGHT * IMAGE_BPP];
	unsigned char* m_image_buffer_compressed;
	long unsigned int m_image_buffer_compressed_size;
	unsigned char* m_image_buffer_decompressed;
protected:
public:
	// Ctor/Dtor
	explicit CSDLOpenGLTest(void);
	virtual ~CSDLOpenGLTest();

	void __init(void);
	void __release(void);

	// ---------------------------------------------------------------

	//void usage(const char** argv);

	int tj_compress(void);
	int tj_decompress(void);
	// ---------------------------------------------------------------
};
// ---------------------------------------------------------------


#endif	// __CSDL_OPENGL_TEST_H__
