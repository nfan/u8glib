/*

  Many parts are taken over from m2tklib
  
*/


#include "u8g.h"
#include "SDL.h"
#include "SDL_video.h"
#include <assert.h>

#define HEIGHT 64
#define WIDTH 128

SDL_Surface *u8g_sdl_screen;
int u8g_sdl_multiple = 2;
Uint32 u8g_sdl_color[4];

void u8g_sdl_set_pixel(int x, int y, int idx)
{
  Uint32  *ptr;
  Uint32 offset;
  int i, j;
  
  if ( y >= HEIGHT )
    return;
  if ( y < 0 )
    return;
  if ( x >= WIDTH )
    return;
  if ( x < 0 )
    return;
  
  for( i = 0; i < u8g_sdl_multiple; i++ )
    for( j = 0; j < u8g_sdl_multiple; j++ )
    {
      offset = 
	(y * u8g_sdl_multiple + j) * u8g_sdl_screen->w * u8g_sdl_screen->format->BytesPerPixel + 
	(x * u8g_sdl_multiple + i) * u8g_sdl_screen->format->BytesPerPixel;
      assert( offset < u8g_sdl_screen->w * u8g_sdl_screen->h * u8g_sdl_screen->format->BytesPerPixel );
      ptr = u8g_sdl_screen->pixels + offset;
      *ptr = u8g_sdl_color[idx];
    }
}


void u8g_sdl_init(void)
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) 
  {
    printf("Unable to initialize SDL:  %s\n", SDL_GetError());
    exit(1);
  }
  
  /* http://www.libsdl.org/cgi/docwiki.cgi/SDL_SetVideoMode */
  u8g_sdl_screen = SDL_SetVideoMode(WIDTH*u8g_sdl_multiple,HEIGHT*u8g_sdl_multiple,32,SDL_SWSURFACE|SDL_ANYFORMAT);
  if ( u8g_sdl_screen == NULL ) 
  {
    printf("Couldn't set video mode: %s\n", SDL_GetError());
    exit(1);
  }
  printf("At %d bits-per-pixel mode\n", u8g_sdl_screen->format->BitsPerPixel);
  
  u8g_sdl_color[0] = SDL_MapRGB( u8g_sdl_screen->format, 0, 0, 0 );
  u8g_sdl_color[1] = SDL_MapRGB( u8g_sdl_screen->format, 25, 64, 0 );
  u8g_sdl_color[2] = SDL_MapRGB( u8g_sdl_screen->format, 50, 128, 0 );
  u8g_sdl_color[3] = SDL_MapRGB( u8g_sdl_screen->format, 100, 255, 0 );

  /*
  u8g_sdl_set_pixel(0,0);
  u8g_sdl_set_pixel(1,1);
  u8g_sdl_set_pixel(2,2);
  */

  /* update all */
  /* http://www.libsdl.org/cgi/docwiki.cgi/SDL_UpdateRect */
  SDL_UpdateRect(u8g_sdl_screen, 0,0,0,0);

  atexit(SDL_Quit);
  return;
}

void u8g_sdl_start(void)
{
  Uint32 color = SDL_MapRGB( u8g_sdl_screen->format, 0, 0, 0 );
  /* http://www.libsdl.org/cgi/docwiki.cgi/SDL_FillRect */
  SDL_FillRect(u8g_sdl_screen, NULL, color);
  u8g_sdl_set_pixel(0,0,3);
  u8g_sdl_set_pixel(10,0,3);
  u8g_sdl_set_pixel(20,0,3);
  u8g_sdl_set_pixel(30,0,3);
  u8g_sdl_set_pixel(40,0,3);
  u8g_sdl_set_pixel(50,0,3);
}

void u8g_sdl_end(void)
{
}

/* return ascii key value or -1 */
int u8g_sdl_get_key(void)
{
  SDL_Event event;
	/* http://www.libsdl.org/cgi/docwiki.cgi/SDL_PollEvent */
  if ( SDL_PollEvent(&event) != 0 )
  {
    switch (event.type) 
    {
        case SDL_QUIT:
            exit(0);
            break;
        case SDL_KEYDOWN:
          switch( event.key.keysym.sym )
          {
            case SDLK_a: return 'a';
            case SDLK_b: return 'b';
            case SDLK_c: return 'c';
            case SDLK_d: return 'd';
            case SDLK_e: return 'e';
            case SDLK_f: return 'f';
            case SDLK_g: return 'g';
            case SDLK_h: return 'h';
            case SDLK_i: return 'i';
            case SDLK_j: return 'j';
            case SDLK_k: return 'k';
            case SDLK_l: return 'l';
            case SDLK_m: return 'm';
            case SDLK_n: return 'n';
            case SDLK_o: return 'o';
            case SDLK_p: return 'p';
            case SDLK_q: return 'q';
            case SDLK_r: return 'r';
            case SDLK_s: return 's';
            case SDLK_t: return 't';
            case SDLK_u: return 'u';
            case SDLK_v: return 'v';
            case SDLK_w: return 'w';
            case SDLK_x: return 'x';
            case SDLK_y: return 'y';
            case SDLK_z: return 'z';
            default: return 0;
          }
    }
  }
  return -1;
}

uint8_t u8g_dev_sdl_1bit_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg)
{
  
  switch(msg)
  {
    case U8G_DEV_MSG_INIT:
      u8g_sdl_init();
      break;
    case U8G_DEV_MSG_STOP:
      break;
    case U8G_DEV_MSG_PAGE_FIRST:
      u8g_sdl_start();
      break;
    case U8G_DEV_MSG_PAGE_NEXT:
      {
        u8g_pb_t *pb = (u8g_pb_t *)(dev->dev_mem);
        uint8_t i, j;
        uint8_t page_height;
        page_height = pb->p.page_y1;
        page_height -= pb->p.page_y0;
        page_height++;
        for( j = 0; j < page_height; j++ )
        {
          for( i = 0; i < WIDTH; i++ )
          {
            if ( (   ((uint8_t *)(pb->buf))[i] & (1<<j)) != 0 )
              u8g_sdl_set_pixel(i, j+pb->p.page_y0, 3);
          }
        }
      }
      /* update all */
      /* http://www.libsdl.org/cgi/docwiki.cgi/SDL_UpdateRect */
      SDL_UpdateRect(u8g_sdl_screen, 0,0,0,0);
      break;    /* continue to base fn */
  }
  return u8g_dev_pb8v1_base_fn(u8g, dev, msg, arg);
}

uint8_t u8g_dev_sdl_2bit_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg)
{
  
  switch(msg)
  {
    case U8G_DEV_MSG_INIT:
      u8g_sdl_init();
      break;
    case U8G_DEV_MSG_STOP:
      break;
    case U8G_DEV_MSG_PAGE_FIRST:
      u8g_sdl_start();
      break;
    case U8G_DEV_MSG_PAGE_NEXT:
      {
        u8g_pb_t *pb = (u8g_pb_t *)(dev->dev_mem);
        uint8_t i, j, v;
        uint8_t page_height;
        page_height = pb->p.page_y1;
        page_height -= pb->p.page_y0;
        page_height++;
        for( j = 0; j < page_height; j++ )
        {
          for( i = 0; i < WIDTH; i++ )
          {
            v = ((uint8_t *)(pb->buf))[i];
            v >>= j*2;
            v &= 3;
            u8g_sdl_set_pixel(i, j+pb->p.page_y0, v);
          }
        }
      }
      /* update all */
      /* http://www.libsdl.org/cgi/docwiki.cgi/SDL_UpdateRect */
      SDL_UpdateRect(u8g_sdl_screen, 0,0,0,0);
      break;    /* continue to base fn */
  }
  return u8g_dev_pb8v2_base_fn(u8g, dev, msg, arg);
}

U8G_PB_DEV(u8g_dev_sdl_1bit, WIDTH, HEIGHT, 8, u8g_dev_sdl_1bit_fn, NULL);
U8G_PB_DEV(u8g_dev_sdl_2bit, WIDTH, HEIGHT, 4, u8g_dev_sdl_2bit_fn, NULL);