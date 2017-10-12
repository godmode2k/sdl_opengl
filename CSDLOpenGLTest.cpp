/* --------------------------------------------------------------
Project:	SDL OpenGL, streaming image Test
Purpose:	TEST only !!!
Author:		Ho-Jung Kim (godmode2k@hotmail.com)
Date:		Since Oct 7, 2017
Filename:	CSDLOpenGLTest.cpp

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
#include "CSDLOpenGLTest.h"
// ---------------------------------------------------------------



//! Definition
// ---------------------------------------------------------------
// ---------------------------------------------------------------



//! Prototype
// ---------------------------------------------------------------
// ---------------------------------------------------------------



//! IMPLEMENTATION
// ---------------------------------------------------------------
// Ctor
CSDLOpenGLTest::CSDLOpenGLTest(void) : TAG("CSDLOpenGLTest") {
	//__LOGT__( TAG, "CSDLOpenGLTest()" );
	fprintf( stdout, "CSDLOpenGLTest()\n" );

	__init();
}

// Dtor
CSDLOpenGLTest::~CSDLOpenGLTest() {
	//__LOGT__( TAG, "~CSDLOpenGLTest()" );
	fprintf( stdout, "~CSDLOpenGLTest()\n" );

	__release();
}

// Initialize
void CSDLOpenGLTest::__init(void) {
	//__LOGT__( TAG, "__init()" );
	fprintf( stdout, "__init()\n" );

	m_image_buffer_compressed = NULL;
	m_image_buffer_compressed_size = 0;
}

// Release
void CSDLOpenGLTest::__release(void) {
	//__LOGT__( TAG, "__release()" );
	fprintf( stdout, "__release()\n" );

	if ( m_image_buffer_compressed ) {
		tjFree( m_image_buffer_compressed );
		m_image_buffer_compressed = NULL;
	}
}
// ---------------------------------------------------------------
uint32_t htonf(float f) {
	uint32_t p;
	uint32_t sign;

	if (f < 0) { sign = 1; f = -f; }
	else { sign = 0; }
		
	p = ((((uint32_t)f)&0x7fff)<<16) | (sign<<31); // whole part and sign
	p |= (uint32_t)(((f - (int)f) * 65536.0f))&0xffff; // fraction

	return p;
}

float ntohf(uint32_t p) {
	float f = ((p>>16)&0x7fff); // whole part
	f += (p&0xffff) / 65536.0f; // fraction

	if (((p>>31)&0x1) == 0x1) { f = -f; } // sign bit set

	return f;
}

// htof(), ntohf()
// Source: https://beej.us/guide/bgnet/examples/pack.c
int test_htonf_ntohf(void) {
	float f = 3.1415926, f2;
	uint32_t netf;

	netf = htonf(f);
	f2 = ntohf(netf);

	printf("Original: %f\n", f);
	printf(" Network: 0x%08X\n", netf);
	printf("Unpacked: %f\n", f2);

	return 0;
}
// ---------------------------------------------------------------
int CSDLOpenGLTest::tj_compress(void) {
	fprintf( stdout, "tj_compress()\n" );
	int ret = -1;

	if ( m_image_buffer_compressed ) {
		tjFree( m_image_buffer_compressed );
		m_image_buffer_compressed = NULL;
	}

	tjhandle handle = tjInitCompress();
	int pitch = 0;
	int pixel_format = TJPF_RGB;
	long unsigned int jpeg_size = 0;
	int sub_sampling = TJSAMP_444;
	int jpeg_quality = 75;
	int flags = TJFLAG_FASTDCT;

	ret = tjCompress2( handle, m_image_buffer,
						WINDOW_WIDTH, pitch, WINDOW_HEIGHT,
						pixel_format,
						&m_image_buffer_compressed,
						&jpeg_size,
						sub_sampling,
						jpeg_quality,
						flags );

	m_image_buffer_compressed_size = jpeg_size;

	tjDestroy( handle );

	return ret;
}

int CSDLOpenGLTest::tj_decompress(void) {
	fprintf( stdout, "tj_decompress()\n" );
	int ret = -1;

	if ( !m_image_buffer_compressed ) {
		fprintf( stderr, "tj_decompress(): buffer == NULL\n" );
		return ret;
	}

	if ( m_image_buffer_compressed_size <= 0 ) {
		fprintf( stderr, "tj_decompress(): buffer size == NULL\n" );
		return ret;
	}

	tjhandle handle = tjInitDecompress();
	long unsigned int jpeg_size = m_image_buffer_compressed_size;
	int sub_sampling, width, height;
	int pitch = 0;
	int pixel_format = TJPF_RGB;
	int flags = TJFLAG_FASTDCT;

	ret = tjDecompressHeader2( handle, m_image_buffer_compressed,
						jpeg_size, &width, &height, &sub_sampling );
	if ( ret < 0 ) {
		fprintf( stderr, "tj_decompress(): error: get header\n%s\n", tjGetErrorStr() );
		return ret;
	}

	m_image_buffer_decompressed = new unsigned char[width * height * IMAGE_BPP];

	ret = tjDecompress2( handle, m_image_buffer_compressed,
					jpeg_size,
					m_image_buffer_decompressed,
					width, pitch, height, pixel_format, flags );

	tjDestroy( handle );

	return ret;
}
// ---------------------------------------------------------------



//! MAIN
// ---------------------------------------------------------------
//void CSDLOpenGLTest::usage(const char** argv) {
//	fprintf( stdout, "Usage: %s {filter type} {wav filename}\n"
//			"{filter type}\n"
//			"    --filter-hpf\n"
//			, argv[0] );
//}

int main(int argc, char* argv[]) {
	fprintf( stdout, "usage: %s [ipaddr: client mode]\n\n", argv[0] );

	CSDLOpenGLTest* app = new CSDLOpenGLTest();

	// Video
#if __SDLv1__
	SDL_Surface* screen = NULL;
#elif __SDLv2__
	SDL_Window* window = NULL;
#endif
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;
	SDL_GLContext gl_context = NULL;


	// Network
	bool client_mode = false;
	int sockfd;
	char* ipv4_addr = DEFAULT_IPV4_ADDR;
	int port = DEFAULT_PORT;
    int data_bytes = 0;
    uint8_t data_buf[data_bytes];

	// Server
	struct sockaddr_in server_sockaddr;
	unsigned int server_addr_len = 0;
	int optVal = 1;

	// Client
	int sockfd_client = -1;
	struct sockaddr_in client_sockaddr;
	socklen_t client_sockaddr_len = sizeof( sockaddr_in );



	if ( argc > 1 ) {
		//ip6_addr = argv[1];
		ipv4_addr = argv[1];
		client_mode = true;
	}

	if ( client_mode )
		fprintf( stdout, "main(): [CLIENT] connect to %s\n", ipv4_addr );
	else
		fprintf( stdout, "main(): [SERVER]\n" );

	if ( !app ) {
		fprintf( stderr, "app == NULL\n" );
		return -1;
	}


//	if ( argc < 3 ) {
//		app->usage( (const char**)argv );
//
//		//return -1;
//		goto _RELEASE;
//	}


	// Init
	// SDL_INIT_VIDEO, SDL_INIT_AUDIO, SDL_INIT_EVERYTHING
	if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0 ) {
		fprintf( stderr, "main(): SDL_Init(): %s\n", SDL_GetError() );
		goto _RELEASE;
	}


	// Video
	{
#if __SDLv1__
		// SDL_ANYFORMAT, SDL_DOUBLEBUF
		screen = SDL_SetVideoMode( WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_DOUBLEBUF );	
		if ( screen == NULL ) {
			fprintf( stderr, "main(): SDL_SetVideoMode(): %s\n", SDL_GetError() );
			goto _RELEASE;
		}

		SDL_WM_SetCaption( "OpenGL, streaming image Test", NULL );

		{
			SDL_Rect rect = { 10, 10, 50, 50 };
			Uint32 color = SDL_MapRGB( screen->format, 0xff, 0x00, 0x00 );

			// Clear the screen
			//SDL_FillRect( screen, NULL, 0 );

			// Draw the rectangle
			SDL_FillRect( screen, &rect, color );
			//SDL_Flip( screen );
			SDL_UpdateRect( screen, 10, 10, 50, 50 );
		}
#elif __SDLv2__
		window = SDL_CreateWindow( "OpenGL, streaming image Test",
						SDL_WINDOWPOS_UNDEFINED,
						SDL_WINDOWPOS_UNDEFINED,
						WINDOW_WIDTH, WINDOW_HEIGHT,
						SDL_WINDOW_OPENGL );
		//SDL_CreateWindowAndRenderer( WINDOW_WIDTH, WINDOW_HEIGHT,
		//							0, &window, &renderer );
		if ( window == NULL ) {
			fprintf( stderr, "main(): SDL_CreateWindow(): %s\n", SDL_GetError() );
			goto _RELEASE;
		}

		renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
		if ( renderer == NULL ) {
			fprintf( stderr, "main(): SDL_CreateRenderer(): %s\n", SDL_GetError() );
			goto _RELEASE;
		}

		IMG_Init( IMG_INIT_JPG );
		texture = IMG_LoadTexture( renderer, "test.jpg" );
#endif
	}


	// OpenGL
	{
#if __SDLv1__
#elif __SDLv2__
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
		//SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

		gl_context = SDL_GL_CreateContext( window );
		if ( gl_context == NULL ) {
			fprintf( stderr, "main(): SDL_GL_CreateContext(): %s\n", SDL_GetError() );
			goto _RELEASE;
		}

		fprintf( stdout, "main(): GL_VERSION: %s\n", glGetString(GL_VERSION) );
		fprintf( stdout, "main(): GL_VENDOR : %s\n", glGetString(GL_VENDOR) );
		fprintf( stdout, "main(): GL_RENDERER: %s\n", glGetString(GL_RENDERER) );

		//SDL_GL_SetSwapInterval( 0 );

		if ( SDL_GL_MakeCurrent(window, gl_context) < 0 ) {
			fprintf( stderr, "main(): SDL_GL_MakeCurrent(): %s\n", SDL_GetError() );
			goto _RELEASE;
		}
#endif
	}



	{
		if ( client_mode ) {
			{
				if ( (sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 ) {
					fprintf( stderr, "error: socket()\n" );
					goto _RELEASE;
				}

				if ( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal)) < 0 ) {
					fprintf( stderr, "error: setsockopt()\n" );
					goto _RELEASE;
				}

				//memset( (void*)&client_sockaddr, 0x00, sizeof(sockaddr_in) );
				bzero( &client_sockaddr, sizeof(sockaddr_in) );
				client_sockaddr.sin_family = AF_INET;
				client_sockaddr.sin_port = htons( (unsigned int)port );
				//client_sockaddr.sin_addr.s_addr = inet_addr( ipv4_addr );
				client_sockaddr.sin_addr.s_addr = htonl( INADDR_ANY );
			}
		}
		else {
			if ( (sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ) {
				fprintf( stderr, "error: socket()\n" );
				goto _RELEASE;
			}

			if ( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal)) < 0 ) {
				fprintf( stderr, "error: setsockopt()\n" );
				goto _RELEASE;
			}

			//memset( (void*)&server_sockaddr, 0x00, sizeof(sockaddr_in) );
			bzero( &server_sockaddr, sizeof(sockaddr_in) );
			server_sockaddr.sin_family = AF_INET;
			server_sockaddr.sin_port = htons( (unsigned int)port );
			//server_sockaddr.sin_addr.s_addr = inet_addr( ipv4_addr );
			server_sockaddr.sin_addr.s_addr = htonl( INADDR_ANY );

			if ( bind(sockfd, (struct sockaddr *)&server_sockaddr, sizeof(sockaddr)) < 0 ) {
				fprintf( stderr, "error: bind()\n" );
				goto _RELEASE;
			}

			if ( listen(sockfd, DEFAULT_BACKLOG) < 0 ) {
				fprintf( stderr, "error: listen(): %d (%s)\n", errno, strerror(errno) );
				goto _RELEASE;
			}
		}
	}



	/*
	// Event
	{
		SDL_Event event;

		while ( 1 ) {
			while ( SDL_PollEvent(&event) ) {
				if ( event.type == SDL_QUIT ) {
					goto _RELEASE;
				}
			}
		}
	}
	*/
	// Event
	{
		SDL_Event event;

		while ( 1 ) {
			while ( SDL_PollEvent(&event) ) {
				if ( event.type == SDL_QUIT ) {
					goto _RELEASE;
				}

				if ( event.type == SDL_KEYDOWN ) {
#if __SDLv1__
					SDLKey key = event.key.keysym.sym;
#elif __SDLv2__
					SDL_Keycode key = event.key.keysym.sym;
#endif

					// Annoying applied other keys
					/*
					switch ( key ) {
						case SDLK_ESCAPE:
							{
								goto _RELEASE;
							} break;
					}
					*/

					if ( key == SDLK_ESCAPE ) {
						goto _RELEASE;
					}
				}
			} // while ()

			{
				if ( client_mode ) {
					int bytes = 0;
					int buf_data_len = 0;
					unsigned char buf[DEFAULT_RECV_BUF_SIZE] = {0,};
					unsigned char buf_data[DEFAULT_DATA_BUF_SIZE] = {0,};

					// send
					//
					fprintf( stdout, "send...\n" );
					bytes = send( sockfd_client, buf, sizeof(buf), 0 );
					//bytes = write( sockfd_client, buf, sizeof(buf) );
					if ( bytes <= 0 ) {
						fprintf( stderr, "error: send(): %d bytes\n", bytes );
						//break;
					}
					else {
						fprintf( stdout, "send(): sent = %d (%d bytes)\n", buf, bytes );
						bytes = 0;

						// receive
						//! TODO: select()
						while ( 1 ) {
							// TCP
							bytes = recv( sockfd_client, buf, sizeof(buf), 0 );
							//bytes = read( sockfd_client, buf, sizeof(buf) );
							if ( bytes <= 0 ) {
								fprintf( stderr, "error: recv(): %d bytes\n", bytes );
								break;
							}
							//! TODO: checks received size
							memcpy( buf_data, buf, buf_data_len );
							buf_data_len += bytes;


							// UDP
							// recvfrom()
						}
					}


					// stream data
					// ...


#if __SDLv1__
#elif __SDLv2__
					//IMG_Init( IMG_INIT_JPG );
					//texture = IMG_LoadTexture( renderer, "test.jpg" );
					//
					SDL_RenderCopy( renderer, texture, NULL, NULL );
					SDL_RenderPresent( renderer );
#endif
				}
				else {
					if ( sockfd_client < 0 ) {
						fprintf( stdout, "listen...\n" );
						sockfd_client = accept( sockfd, (struct sockaddr *)&client_sockaddr, &client_sockaddr_len );
						if ( sockfd_client < 0 ) {
							fprintf( stderr, "error: accept(): client (%d)\n", sockfd_client );
							continue;
						}
					}
#if __SDLv1__
#elif __SDLv2__
					//SDL_RenderCopy( renderer, texture, NULL, NULL );
					//SDL_RenderPresent( renderer );

					glClearColor( 1.0f, 0.0f, 0.0f, 1.0f );
					glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
					glLoadIdentity();

					// sendto()
					// ...
#endif
				}
			}

#if __SDLv1__
#elif __SDLv2__
			SDL_GL_SwapWindow( window );
#endif
		} // while ()
	}


_RELEASE:
	{
		if ( client_mode ) {
			close( sockfd_client );
		}
		else {
			close( sockfd );
		}

#if __SDLv1__
		if ( screen ) {
			SDL_DestroySurface( screen );
			screen = NULL;
		}
#elif __SDLv2__
		if ( texture ) {
			SDL_DestroyTexture( texture );
			texture = NULL;
		}
		
		IMG_Quit();

		if ( renderer ) {
			SDL_DestroyRenderer( renderer );
			renderer = NULL;
		}

		if ( window ) {
			SDL_DestroyWindow( window );
			window = NULL;
		}
#endif
	}

	SDL_GL_DeleteContext( gl_context );
	SDL_Quit();

	if ( app ) {
		delete app;
		app = NULL;
	}

	return 0;
}
