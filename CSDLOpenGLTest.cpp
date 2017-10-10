/* --------------------------------------------------------------
Project:	SDL OpenGL, streaming image Test
Purpose:	TEST only !!!
Author:		Ho-Jung Kim (godmode2k@hotmail.com)
Date:		Since Oct 7, 2017
Filename:	CSDLOpenGLTest.cpp

Last modified: Oct 7, 2017
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

}

// Release
void CSDLOpenGLTest::__release(void) {
	//__LOGT__( TAG, "__release()" );
	fprintf( stdout, "__release()\n" );

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
	SDL_Window* screen = NULL;
#endif
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;


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
		fprintf( stderr, "main(): Unable to initalized SDL: %s\n", SDL_GetError() );
		//return -1;
		goto _RELEASE;
	}


	// Video
	{
#if __SDLv1__
		// SDL_ANYFORMAT, SDL_DOUBLEBUF
		screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_DOUBLEBUF );	
		if ( screen == NULL ) {
			fprintf( stderr, "main(): Unable to set Video mode: %s\n", Mix_GetError() );
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
		//screen = SDL_CreateWindow( "OpenGL, streaming image Test",
		//							SDL_WINDOWPOS_UNDEFINED,
		//							SDL_WINDOWPOS_UNDEFINED,
		//							__WINDOW_W, __WINDOW_H,
		//							SDL_WINDOW_OPENGL );
		SDL_CreateWindowAndRenderer( SCREEN_WIDTH, SCREEN_HEIGHT,
									0, &screen, &renderer );
		if ( screen == NULL ) {
			fprintf( stderr, "main(): Unable to set Video mode: %s\n", Mix_GetError() );
			goto _RELEASE;
		}

		IMG_Init( IMG_INIT_JPG );
		texture = IMG_LoadTexture( renderer, "test.jpg" );
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


					//IMG_Init( IMG_INIT_JPG );
					//texture = IMG_LoadTexture( renderer, "test.jpg" );
					//
					SDL_RenderCopy( renderer, texture, NULL, NULL );
					SDL_RenderPresent( renderer );
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
					SDL_RenderCopy( renderer, texture, NULL, NULL );
					SDL_RenderPresent( renderer );

					// sendto()
					// ...
#endif
				}
			}
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

		if ( screen ) {
			SDL_DestroyWindow( screen );
			screen = NULL;
		}
#endif
	}



	SDL_Quit();

	if ( app ) {
		delete app;
		app = NULL;
	}

	return 0;
}
