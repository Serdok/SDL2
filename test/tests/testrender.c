/**
 * Original code: automated SDL platform test written by Edgar Simo "bobbens"
 * Extended and extensively updated by aschiffler at ferzkopp dot net
 */

#include <stdio.h>

#include "SDL.h"
#include "SDL_test.h"

/* ================= Test Case Implementation ================== */

#define TESTRENDER_SCREEN_W     80
#define TESTRENDER_SCREEN_H     60

#define RENDER_COMPARE_FORMAT  SDL_PIXELFORMAT_ARGB8888
#define RENDER_COMPARE_AMASK   0xff000000 /**< Alpha bit mask. */
#define RENDER_COMPARE_RMASK   0x00ff0000 /**< Red bit mask. */
#define RENDER_COMPARE_GMASK   0x0000ff00 /**< Green bit mask. */
#define RENDER_COMPARE_BMASK   0x000000ff /**< Blue bit mask. */

#define ALLOWABLE_ERROR_OPAQUE	0
#define ALLOWABLE_ERROR_BLENDED	64

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

/* Prototypes for helper functions */

static int _clearScreen (void);
static void _compare(const char *msg, SDL_Surface *s, int allowable_error);
static int _hasTexAlpha(void);
static int _hasTexColor(void);
static SDL_Texture *_loadTestFace(void);
static int _hasBlendModes(void);
static int _hasDrawColor(void);
static int _isSupported(int code);

/**
 * Create software renderer for tests
 */
void InitCreateRenderer(void *arg)
{
  int posX = 100, posY = 100, width = 320, height = 240;
  renderer = NULL;
  window = SDL_CreateWindow("render_testCreateRenderer", posX, posY, width, height, 0);
  SDLTest_AssertPass("SDL_CreateWindow()");
  SDLTest_AssertCheck(window != NULL, "Check SDL_CreateWindow result");
  if (window == NULL) {
      return;
  }
  
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDLTest_AssertPass("SDL_CreateRenderer()");
  SDLTest_AssertCheck(renderer != 0, "Check SDL_CreateRenderer result");
  if (renderer == 0) {
      SDL_DestroyWindow(window);
      return;
  }
}

/*
 * Destroy renderer for tests  
 */
void CleanupDestroyRenderer(void *arg)
{
  if (renderer != NULL) {  
     SDL_DestroyRenderer(renderer);
     SDLTest_AssertPass("SDL_DestroyRenderer()");
  }
  
  if (window != NULL) {  
     SDL_DestroyWindow(window);
     SDLTest_AssertPass("SDL_DestroyWindow");
  }
}


/**
 * @brief Tests call to SDL_GetNumRenderDrivers
 *
 * \sa
 * http://wiki.libsdl.org/moin.cgi/SDL_GetNumRenderDrivers
 */
int
render_testGetNumRenderDrivers(void *arg)
{
  int n;
  n = SDL_GetNumRenderDrivers();
  SDLTest_AssertCheck(n >= 1, "Number of renderers >= 1, reported as %i", n);
  return TEST_COMPLETED;
}


/**
 * @brief Tests the SDL primitives for rendering.
 *
 * \sa
 * http://wiki.libsdl.org/moin.cgi/SDL_SetRenderDrawColor
 * http://wiki.libsdl.org/moin.cgi/SDL_RenderFillRect
 * http://wiki.libsdl.org/moin.cgi/SDL_RenderDrawLine
 *
 */
int render_testPrimitives (void *arg)
{
   int ret;
   int x, y;
   SDL_Rect rect;
   int checkFailCount1;
   int checkFailCount2;

   /* Need drawcolour or just skip test. */
   SDLTest_AssertCheck(_hasDrawColor(), "_hasDrawColor");

   /* Draw a rectangle. */
   rect.x = 40;
   rect.y = 0;
   rect.w = 40;
   rect.h = 80;

   ret = SDL_SetRenderDrawColor(renderer, 13, 73, 200, SDL_ALPHA_OPAQUE );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_SetRenderDrawColor, expected: 0, got: %i", ret);

   ret = SDL_RenderFillRect(renderer, &rect );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_RenderFillRect, expected: 0, got: %i", ret);

   /* Draw a rectangle. */
   rect.x = 10;
   rect.y = 10;
   rect.w = 60;
   rect.h = 40;
   ret = SDL_SetRenderDrawColor(renderer, 200, 0, 100, SDL_ALPHA_OPAQUE );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_SetRenderDrawColor, expected: 0, got: %i", ret);

   ret = SDL_RenderFillRect(renderer, &rect );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_RenderFillRect, expected: 0, got: %i", ret);

   /* Draw some points like so:
    * X.X.X.X..
    * .X.X.X.X.
    * X.X.X.X.. */
   checkFailCount1 = 0;
   checkFailCount2 = 0;
   for (y=0; y<3; y++) {
      for (x = y % 2; x<TESTRENDER_SCREEN_W; x+=2) {
         ret = SDL_SetRenderDrawColor(renderer, x*y, x*y/2, x*y/3, SDL_ALPHA_OPAQUE );
         if (ret != 0) checkFailCount1++;

         ret = SDL_RenderDrawPoint(renderer, x, y );
         if (ret != 0) checkFailCount2++;
      }
   }
   SDLTest_AssertCheck(checkFailCount1 == 0, "Validate results from calls to SDL_SetRenderDrawColor, expected: 0, got: %i", checkFailCount1);
   SDLTest_AssertCheck(checkFailCount2 == 0, "Validate results from calls to SDL_RenderDrawPoint, expected: 0, got: %i", checkFailCount2);

   /* Draw some lines. */
   ret = SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_SetRenderDrawColor");

   ret = SDL_RenderDrawLine(renderer, 0, 30, TESTRENDER_SCREEN_W, 30 );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_RenderDrawLine, expected: 0, got: %i", ret);

   ret = SDL_SetRenderDrawColor(renderer, 55, 55, 5, SDL_ALPHA_OPAQUE );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_SetRenderDrawColor, expected: 0, got: %i", ret);

   ret = SDL_RenderDrawLine(renderer, 40, 30, 40, 60 );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_RenderDrawLine, expected: 0, got: %i", ret);

   ret = SDL_SetRenderDrawColor(renderer, 5, 105, 105, SDL_ALPHA_OPAQUE );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_SetRenderDrawColor, expected: 0, got: %i", ret);

   ret = SDL_RenderDrawLine(renderer, 0, 0, 29, 29 );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_RenderDrawLine, expected: 0, got: %i", ret);

   ret = SDL_RenderDrawLine(renderer, 29, 30, 0, 59 );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_RenderDrawLine, expected: 0, got: %i", ret);

   ret = SDL_RenderDrawLine(renderer, 79, 0, 50, 29 );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_RenderDrawLine, expected: 0, got: %i", ret);

   ret = SDL_RenderDrawLine(renderer, 79, 59, 50, 30 );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_RenderDrawLine, expected: 0, got: %i", ret);

   /* See if it's the same. */
   _compare( "Primitives output not the same.", SDLTest_ImagePrimitives(), ALLOWABLE_ERROR_OPAQUE );

   return TEST_COMPLETED;
}

/**
 * @brief Tests the SDL primitives with alpha for rendering.
 *
 * \sa
 * http://wiki.libsdl.org/moin.cgi/SDL_SetRenderDrawColor
 * http://wiki.libsdl.org/moin.cgi/SDL_SetRenderDrawBlendMode
 * http://wiki.libsdl.org/moin.cgi/SDL_RenderFillRect
 */
int render_testPrimitivesBlend (void *arg)
{
   int ret;
   int i, j;
   SDL_Rect rect;
   int checkFailCount1;
   int checkFailCount2;
   int checkFailCount3;

   /* Need drawcolour and blendmode or just skip test. */
   SDLTest_AssertCheck(_hasDrawColor(), "_hasDrawColor");
   SDLTest_AssertCheck(_hasBlendModes(), "_hasBlendModes");

   /* Create some rectangles for each blend mode. */
   ret = SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0 );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_SetRenderDrawColor, expected: 0, got: %i", ret);

   ret = SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_SetRenderDrawBlendMode, expected: 0, got: %i", ret);

   ret = SDL_RenderFillRect(renderer, NULL );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_RenderFillRect, expected: 0, got: %i", ret);

   rect.x = 10;
   rect.y = 25;
   rect.w = 40;
   rect.h = 25;
   ret = SDL_SetRenderDrawColor(renderer, 240, 10, 10, 75 );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_SetRenderDrawColor, expected: 0, got: %i", ret);

   ret = SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_SetRenderDrawBlendMode, expected: 0, got: %i", ret);

   ret = SDL_RenderFillRect(renderer, &rect );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_RenderFillRect, expected: 0, got: %i", ret);

   rect.x = 30;
   rect.y = 40;
   rect.w = 45;
   rect.h = 15;
   ret = SDL_SetRenderDrawColor(renderer, 10, 240, 10, 100 );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_SetRenderDrawColor, expected: 0, got: %i", ret);

   ret = SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_SetRenderDrawBlendMode, expected: 0, got: %i", ret);

   ret = SDL_RenderFillRect(renderer, &rect );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_RenderFillRect, expected: 0, got: %i", ret);

   rect.x = 25;
   rect.y = 25;
   rect.w = 25;
   rect.h = 25;
   ret = SDL_SetRenderDrawColor(renderer, 10, 10, 240, 125 );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_SetRenderDrawColor, expected: 0, got: %i", ret);

   ret = SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_SetRenderDrawBlendMode, expected: 0, got: %i", ret);

   ret = SDL_RenderFillRect(renderer, &rect );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_RenderFillRect, expected: 0, got: %i", ret);


   /* Draw blended lines, lines for everyone. */
   checkFailCount1 = 0;
   checkFailCount2 = 0;
   checkFailCount3 = 0;
   for (i=0; i<TESTRENDER_SCREEN_W; i+=2)  {
      ret = SDL_SetRenderDrawColor(renderer, 60+2*i, 240-2*i, 50, 3*i );
      if (ret != 0) checkFailCount1++;

      ret = SDL_SetRenderDrawBlendMode(renderer,(((i/2)%3)==0) ? SDL_BLENDMODE_BLEND :
            (((i/2)%3)==1) ? SDL_BLENDMODE_ADD : SDL_BLENDMODE_NONE );
      if (ret != 0) checkFailCount2++;

      ret = SDL_RenderDrawLine(renderer, 0, 0, i, 59 );
      if (ret != 0) checkFailCount3++;
   }
   SDLTest_AssertCheck(checkFailCount1 == 0, "Validate results from calls to SDL_SetRenderDrawColor, expected: 0, got: %i", checkFailCount1);
   SDLTest_AssertCheck(checkFailCount2 == 0, "Validate results from calls to SDL_SetRenderDrawBlendMode, expected: 0, got: %i", checkFailCount2);
   SDLTest_AssertCheck(checkFailCount3 == 0, "Validate results from calls to SDL_RenderDrawLine, expected: 0, got: %i", checkFailCount3);
   
   checkFailCount1 = 0;
   checkFailCount2 = 0;
   checkFailCount3 = 0;
   for (i=0; i<TESTRENDER_SCREEN_H; i+=2)  {
      ret = SDL_SetRenderDrawColor(renderer, 60+2*i, 240-2*i, 50, 3*i );
      if (ret != 0) checkFailCount1++;

      ret = SDL_SetRenderDrawBlendMode(renderer,(((i/2)%3)==0) ? SDL_BLENDMODE_BLEND :
            (((i/2)%3)==1) ? SDL_BLENDMODE_ADD : SDL_BLENDMODE_NONE );
      if (ret != 0) checkFailCount2++;

      ret = SDL_RenderDrawLine(renderer, 0, 0, 79, i );
      if (ret != 0) checkFailCount3++;
   }
   SDLTest_AssertCheck(checkFailCount1 == 0, "Validate results from calls to SDL_SetRenderDrawColor, expected: 0, got: %i", checkFailCount1);
   SDLTest_AssertCheck(checkFailCount2 == 0, "Validate results from calls to SDL_SetRenderDrawBlendMode, expected: 0, got: %i", checkFailCount2);
   SDLTest_AssertCheck(checkFailCount3 == 0, "Validate results from calls to SDL_RenderDrawLine, expected: 0, got: %i", checkFailCount3);

   /* Draw points. */
   checkFailCount1 = 0;
   checkFailCount2 = 0;
   checkFailCount3 = 0;
   for (j=0; j<TESTRENDER_SCREEN_H; j+=3) {
      for (i=0; i<TESTRENDER_SCREEN_W; i+=3) {
         ret = SDL_SetRenderDrawColor(renderer, j*4, i*3, j*4, i*3 );
         if (ret != 0) checkFailCount1++;

         ret = SDL_SetRenderDrawBlendMode(renderer, ((((i+j)/3)%3)==0) ? SDL_BLENDMODE_BLEND :
               ((((i+j)/3)%3)==1) ? SDL_BLENDMODE_ADD : SDL_BLENDMODE_NONE );
         if (ret != 0) checkFailCount2++;

         ret = SDL_RenderDrawPoint(renderer, i, j );
         if (ret != 0) checkFailCount3++;
      }
   }   
   SDLTest_AssertCheck(checkFailCount1 == 0, "Validate results from calls to SDL_SetRenderDrawColor, expected: 0, got: %i", checkFailCount1);
   SDLTest_AssertCheck(checkFailCount2 == 0, "Validate results from calls to SDL_SetRenderDrawBlendMode, expected: 0, got: %i", checkFailCount2);
   SDLTest_AssertCheck(checkFailCount3 == 0, "Validate results from calls to SDL_RenderDrawPoint, expected: 0, got: %i", checkFailCount3);

   /* See if it's the same. */
   _compare( "Blended primitives output not the same.", SDLTest_ImagePrimitivesBlend(), ALLOWABLE_ERROR_BLENDED );

    return TEST_COMPLETED;
}



/**
 * @brief Tests some blitting routines.
 *
 * \sa
 * http://wiki.libsdl.org/moin.cgi/SDL_RenderCopy
 * http://wiki.libsdl.org/moin.cgi/SDL_DestroyTexture
 */
int
render_testBlit(void *arg)
{
   int ret;
   SDL_Rect rect;
   SDL_Texture *tface;
   Uint32 tformat;
   int taccess, tw, th;
   int i, j, ni, nj;
   int checkFailCount1;


   /* Need drawcolour or just skip test. */
   SDLTest_AssertCheck(_hasDrawColor(), "_hasDrawColor)");

   /* Create face surface. */
   tface = _loadTestFace();
   SDLTest_AssertCheck(tface != NULL,  "Verify _loadTestFace() result");
   if (tface == NULL) {
       return TEST_ABORTED;
   }

   /* Constant values. */
   ret = SDL_QueryTexture(tface, &tformat, &taccess, &tw, &th);
   SDLTest_AssertCheck(ret == 0, "Verify result from SDL_QueryTexture, expected 0, got %i", ret);
   rect.w = tw;
   rect.h = th;
   ni     = TESTRENDER_SCREEN_W - tw;
   nj     = TESTRENDER_SCREEN_H - th;

   /* Loop blit. */
   checkFailCount1 = 0;
   for (j=0; j <= nj; j+=4) {
      for (i=0; i <= ni; i+=4) {
         /* Blitting. */
         rect.x = i;
         rect.y = j;
         ret = SDL_RenderCopy(renderer, tface, NULL, &rect );
         if (ret != 0) checkFailCount1++;
      }
   }
   SDLTest_AssertCheck(checkFailCount1 == 0, "Validate results from calls to SDL_RenderCopy, expected: 0, got: %i", checkFailCount1);

   /* Clean up. */
   SDL_DestroyTexture( tface );

   /* See if it's the same */
   _compare( "Blit output not the same.", SDLTest_ImageBlit(), ALLOWABLE_ERROR_OPAQUE );

   return TEST_COMPLETED;
}


/**
 * @brief Blits doing colour tests.
 *
 * \sa
 * http://wiki.libsdl.org/moin.cgi/SDL_SetTextureColorMod
 * http://wiki.libsdl.org/moin.cgi/SDL_RenderCopy
 * http://wiki.libsdl.org/moin.cgi/SDL_DestroyTexture
 */
int
render_testBlitColor (void *arg)
{
   int ret;
   SDL_Rect rect;
   SDL_Texture *tface;
   Uint32 tformat;
   int taccess, tw, th;
   int i, j, ni, nj;
   int checkFailCount1;
   int checkFailCount2;

   /* Create face surface. */
   tface = _loadTestFace();
   SDLTest_AssertCheck(tface != NULL, "Verify _loadTestFace() result");
   if (tface == NULL) {
       return TEST_ABORTED;
   }

   /* Constant values. */
   ret = SDL_QueryTexture(tface, &tformat, &taccess, &tw, &th);
   SDLTest_AssertCheck(ret == 0, "Verify result from SDL_QueryTexture, expected 0, got %i", ret);
   rect.w = tw;
   rect.h = th;
   ni     = TESTRENDER_SCREEN_W - tw;
   nj     = TESTRENDER_SCREEN_H - th;

   /* Test blitting with colour mod. */
   checkFailCount1 = 0;
   checkFailCount2 = 0;
   for (j=0; j <= nj; j+=4) {
      for (i=0; i <= ni; i+=4) {
         /* Set colour mod. */
         ret = SDL_SetTextureColorMod( tface, (255/nj)*j, (255/ni)*i, (255/nj)*j );
         if (ret != 0) checkFailCount1++;

         /* Blitting. */
         rect.x = i;
         rect.y = j;
         ret = SDL_RenderCopy(renderer, tface, NULL, &rect );
         if (ret != 0) checkFailCount2++;
      }
   }
   SDLTest_AssertCheck(checkFailCount1 == 0, "Validate results from calls to SDL_SetTextureColorMod, expected: 0, got: %i", checkFailCount1);
   SDLTest_AssertCheck(checkFailCount2 == 0, "Validate results from calls to SDL_RenderCopy, expected: 0, got: %i", checkFailCount2);

   /* Clean up. */
   SDL_DestroyTexture( tface );

   /* See if it's the same. */
   _compare( "Blit output not the same (using SDL_SetTextureColorMod).",
            SDLTest_ImageBlitColor(), ALLOWABLE_ERROR_OPAQUE );

   return TEST_COMPLETED;
}


/**
 * @brief Tests blitting with alpha.
 *
 * \sa
 * http://wiki.libsdl.org/moin.cgi/SDL_SetTextureAlphaMod
 * http://wiki.libsdl.org/moin.cgi/SDL_RenderCopy
 * http://wiki.libsdl.org/moin.cgi/SDL_DestroyTexture
 */
int
render_testBlitAlpha (void *arg)
{
   int ret;
   SDL_Rect rect;
   SDL_Texture *tface;
   Uint32 tformat;
   int taccess, tw, th;
   int i, j, ni, nj;
   int checkFailCount1;
   int checkFailCount2;


   /* Need alpha or just skip test. */
   SDLTest_AssertCheck(_hasTexAlpha(), "_hasTexAlpha");

   /* Create face surface. */
   tface = _loadTestFace();
   SDLTest_AssertCheck(tface != NULL, "Verify _loadTestFace() result");
   if (tface == NULL) {
       return TEST_ABORTED;
   }

   /* Constant values. */
   ret = SDL_QueryTexture(tface, &tformat, &taccess, &tw, &th);
   SDLTest_AssertCheck(ret == 0, "Verify result from SDL_QueryTexture, expected 0, got %i", ret);
   rect.w = tw;
   rect.h = th;
   ni     = TESTRENDER_SCREEN_W - tw;
   nj     = TESTRENDER_SCREEN_H - th;

   /* Test blitting with alpha mod. */
   checkFailCount1 = 0;
   checkFailCount2 = 0;
   for (j=0; j <= nj; j+=4) {
      for (i=0; i <= ni; i+=4) {
         /* Set alpha mod. */
         ret = SDL_SetTextureAlphaMod( tface, (255/ni)*i );
         if (ret != 0) checkFailCount1++;

         /* Blitting. */
         rect.x = i;
         rect.y = j;
         ret = SDL_RenderCopy(renderer, tface, NULL, &rect );
         if (ret != 0) checkFailCount2++;
      }
   }   
   SDLTest_AssertCheck(checkFailCount1 == 0, "Validate results from calls to SDL_SetTextureAlphaMod, expected: 0, got: %i", checkFailCount1);
   SDLTest_AssertCheck(checkFailCount2 == 0, "Validate results from calls to SDL_RenderCopy, expected: 0, got: %i", checkFailCount2);

   /* Clean up. */
   SDL_DestroyTexture( tface );

   /* See if it's the same. */
   _compare( "Blit output not the same (using SDL_SetSurfaceAlphaMod).",
            SDLTest_ImageBlitAlpha(), ALLOWABLE_ERROR_BLENDED );

   return TEST_COMPLETED;
}

/* Helper functions */

/**
 * @brief Tests a blend mode.
 *
 * \sa
 * http://wiki.libsdl.org/moin.cgi/SDL_SetTextureBlendMode
 * http://wiki.libsdl.org/moin.cgi/SDL_RenderCopy
 */
static void
_testBlitBlendMode( SDL_Texture * tface, int mode )
{
   int ret;
   Uint32 tformat;
   int taccess, tw, th;
   int i, j, ni, nj;
   SDL_Rect rect;
   int checkFailCount1;
   int checkFailCount2;

   /* Clear surface. */
   _clearScreen();

   /* Constant values. */
   ret = SDL_QueryTexture(tface, &tformat, &taccess, &tw, &th);
   SDLTest_AssertCheck(ret == 0, "Verify result from SDL_QueryTexture, expected 0, got %i", ret);
   rect.w = tw;
   rect.h = th;
   ni     = TESTRENDER_SCREEN_W - tw;
   nj     = TESTRENDER_SCREEN_H - th;

   /* Test blend mode. */
   checkFailCount1 = 0;
   checkFailCount2 = 0;
   for (j=0; j <= nj; j+=4) {
      for (i=0; i <= ni; i+=4) {
         /* Set blend mode. */
         ret = SDL_SetTextureBlendMode( tface, (SDL_BlendMode)mode );
         if (ret != 0) checkFailCount1++;

         /* Blitting. */
         rect.x = i;
         rect.y = j;
         ret = SDL_RenderCopy(renderer, tface, NULL, &rect );
         if (ret != 0) checkFailCount2++;
      }
   }
   SDLTest_AssertCheck(checkFailCount1 == 0, "Validate results from calls to SDL_SetTextureBlendMode, expected: 0, got: %i", checkFailCount1);
   SDLTest_AssertCheck(checkFailCount2 == 0, "Validate results from calls to SDL_RenderCopy, expected: 0, got: %i", checkFailCount2);
}


/**
 * @brief Tests some more blitting routines.
 *
 * \sa
 * http://wiki.libsdl.org/moin.cgi/SDL_SetTextureColorMod
 * http://wiki.libsdl.org/moin.cgi/SDL_SetTextureAlphaMod
 * http://wiki.libsdl.org/moin.cgi/SDL_SetTextureBlendMode
 * http://wiki.libsdl.org/moin.cgi/SDL_DestroyTexture
 */
int
render_testBlitBlend (void *arg)
{
   int ret;
   SDL_Rect rect;
   SDL_Texture *tface;
   Uint32 tformat;
   int taccess, tw, th;
   int i, j, ni, nj;
   int mode;
   int checkFailCount1;
   int checkFailCount2;
   int checkFailCount3;
   int checkFailCount4;

   SDLTest_AssertCheck(_hasBlendModes(), "_hasBlendModes");
   SDLTest_AssertCheck(_hasTexColor(), "_hasTexColor");
   SDLTest_AssertCheck(_hasTexAlpha(), "_hasTexAlpha");

   /* Create face surface. */
   tface = _loadTestFace();
   SDLTest_AssertCheck(tface != NULL, "Verify _loadTestFace() result");
   if (tface == NULL) {
       return TEST_ABORTED;
   }

   /* Constant values. */
   ret = SDL_QueryTexture(tface, &tformat, &taccess, &tw, &th);
   SDLTest_AssertCheck(ret == 0, "Verify result from SDL_QueryTexture, expected 0, got %i", ret);
   rect.w = tw;
   rect.h = th;
   ni = TESTRENDER_SCREEN_W - tw;
   nj = TESTRENDER_SCREEN_H - th;

   /* Set alpha mod. */
   ret = SDL_SetTextureAlphaMod( tface, 100 );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_SetTextureAlphaMod, expected: 0, got: %i", ret);

   /* Test None. */
   _testBlitBlendMode( tface, SDL_BLENDMODE_NONE );
   /* See if it's the same. */
   _compare( "Blit blending output not the same (using SDL_BLENDMODE_NONE).",
            SDLTest_ImageBlitBlendNone(), ALLOWABLE_ERROR_OPAQUE ); 


   /* Test Blend. */
   _testBlitBlendMode( tface, SDL_BLENDMODE_BLEND );
   _compare( "Blit blending output not the same (using SDL_BLENDMODE_BLEND).",
            SDLTest_ImageBlitBlend(), ALLOWABLE_ERROR_BLENDED );


   /* Test Add. */
   _testBlitBlendMode( tface, SDL_BLENDMODE_ADD );
   _compare( "Blit blending output not the same (using SDL_BLENDMODE_ADD).",
            SDLTest_ImageBlitBlendAdd(), ALLOWABLE_ERROR_BLENDED );

   /* Test Mod. */
   _testBlitBlendMode( tface, SDL_BLENDMODE_MOD);
   _compare( "Blit blending output not the same (using SDL_BLENDMODE_MOD).",
            SDLTest_ImageBlitBlendMod(), ALLOWABLE_ERROR_BLENDED );

   /* Clear surface. */
   _clearScreen();

   /* Loop blit. */
   checkFailCount1 = 0;
   checkFailCount2 = 0;
   checkFailCount3 = 0;
   checkFailCount4 = 0;
   for (j=0; j <= nj; j+=4) {
      for (i=0; i <= ni; i+=4) {

         /* Set colour mod. */
         ret = SDL_SetTextureColorMod( tface, (255/nj)*j, (255/ni)*i, (255/nj)*j );
         if (ret != 0) checkFailCount1++;

         /* Set alpha mod. */
         ret = SDL_SetTextureAlphaMod( tface, (100/ni)*i );
         if (ret != 0) checkFailCount2++;

         /* Crazy blending mode magic. */
         mode = (i/4*j/4) % 4;
         if (mode==0) mode = SDL_BLENDMODE_NONE;
         else if (mode==1) mode = SDL_BLENDMODE_BLEND;
         else if (mode==2) mode = SDL_BLENDMODE_ADD;
         else if (mode==3) mode = SDL_BLENDMODE_MOD;
         ret = SDL_SetTextureBlendMode( tface, (SDL_BlendMode)mode );
         if (ret != 0) checkFailCount3++;

         /* Blitting. */
         rect.x = i;
         rect.y = j;
         ret = SDL_RenderCopy(renderer, tface, NULL, &rect );
         if (ret != 0) checkFailCount4++;
      }
   }
   SDLTest_AssertCheck(checkFailCount1 == 0, "Validate results from calls to SDL_SetTextureColorMod, expected: 0, got: %i", checkFailCount1);
   SDLTest_AssertCheck(checkFailCount2 == 0, "Validate results from calls to SDL_SetTextureAlphaMod, expected: 0, got: %i", checkFailCount2);
   SDLTest_AssertCheck(checkFailCount3 == 0, "Validate results from calls to SDL_SetTextureBlendMode, expected: 0, got: %i", checkFailCount3);
   SDLTest_AssertCheck(checkFailCount4 == 0, "Validate results from calls to SDL_RenderCopy, expected: 0, got: %i", checkFailCount4);

   /* Clean up. */
   SDL_DestroyTexture( tface );

   /* Check to see if matches. */
   _compare( "Blit blending output not the same (using SDL_BLENDMODE_*).",
            SDLTest_ImageBlitBlendAll(), ALLOWABLE_ERROR_BLENDED);

   return TEST_COMPLETED;
}



/**
 * @brief Checks to see if functionality is supported. Helper function.
 */
static int
_isSupported( int code )
{
   return (code == 0);
}

/**
 * @brief Test to see if we can vary the draw colour. Helper function.
 *
 * \sa
 * http://wiki.libsdl.org/moin.cgi/SDL_SetRenderDrawColor
 * http://wiki.libsdl.org/moin.cgi/SDL_GetRenderDrawColor
 */
static int
_hasDrawColor (void)
{
   int ret, fail;
   Uint8 r, g, b, a;

   fail = 0;

   /* Set colour. */
   ret = SDL_SetRenderDrawColor(renderer, 100, 100, 100, 100 );
   if (!_isSupported(ret))
      fail = 1;
   ret = SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a );
   if (!_isSupported(ret))
      fail = 1;
   /* Restore natural. */
   ret = SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE );
   if (!_isSupported(ret))
      fail = 1;

   /* Something failed, consider not available. */
   if (fail)
      return 0;
   /* Not set properly, consider failed. */
   else if ((r != 100) || (g != 100) || (b != 100) || (a != 100))
      return 0;
   return 1;
}

/**
 * @brief Test to see if we can vary the blend mode. Helper function.
 *
 * \sa
 * http://wiki.libsdl.org/moin.cgi/SDL_SetRenderDrawBlendMode
 * http://wiki.libsdl.org/moin.cgi/SDL_GetRenderDrawBlendMode
 */
static int
_hasBlendModes (void)
{
   int fail;
   int ret;
   SDL_BlendMode mode;

   fail = 0;

   ret = SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND );
   if (!_isSupported(ret))
      fail = 1;
   ret = SDL_GetRenderDrawBlendMode(renderer, &mode );
   if (!_isSupported(ret))
      fail = 1;
   ret = (mode != SDL_BLENDMODE_BLEND);
   if (!_isSupported(ret))
      fail = 1;
   ret = SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD );
   if (!_isSupported(ret))
      fail = 1;
   ret = SDL_GetRenderDrawBlendMode(renderer, &mode );
   if (!_isSupported(ret))
      fail = 1;
   ret = (mode != SDL_BLENDMODE_ADD);
   if (!_isSupported(ret))
      fail = 1;
   ret = SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_MOD );
   if (!_isSupported(ret))
      fail = 1;
   ret = SDL_GetRenderDrawBlendMode(renderer, &mode );
   if (!_isSupported(ret))
      fail = 1;
   ret = (mode != SDL_BLENDMODE_MOD);
   if (!_isSupported(ret))
      fail = 1;
   ret = SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE );
   if (!_isSupported(ret))
      fail = 1;
   ret = SDL_GetRenderDrawBlendMode(renderer, &mode );
   if (!_isSupported(ret))
      fail = 1;
   ret = (mode != SDL_BLENDMODE_NONE);
   if (!_isSupported(ret))
      fail = 1;

   return !fail;
}


/**
 * @brief Loads the test image 'Face' as texture. Helper function.
 *
 * \sa
 * http://wiki.libsdl.org/moin.cgi/SDL_CreateTextureFromSurface
 */
static SDL_Texture *
_loadTestFace(void)
{
   SDL_Surface *face;
   SDL_Texture *tface;

   face = SDLTest_ImageFace();
   if (face == NULL) {
      return NULL;
   }
   
   tface = SDL_CreateTextureFromSurface(renderer, face);
   if (tface == NULL) {
       SDLTest_LogError("SDL_CreateTextureFromSurface() failed with error: %s", SDL_GetError());
   }

   SDL_FreeSurface(face);

   return tface;
}


/**
 * @brief Test to see if can set texture colour mode. Helper function.
 *
 * \sa
 * http://wiki.libsdl.org/moin.cgi/SDL_SetTextureColorMod
 * http://wiki.libsdl.org/moin.cgi/SDL_GetTextureColorMod
 * http://wiki.libsdl.org/moin.cgi/SDL_DestroyTexture
 */
static int
_hasTexColor (void)
{
   int fail;
   int ret;
   SDL_Texture *tface;
   Uint8 r, g, b;

   /* Get test face. */
   tface = _loadTestFace();
   if (tface == 0)
      return 0;

   /* See if supported. */
   fail = 0;
   ret = SDL_SetTextureColorMod( tface, 100, 100, 100 );
   if (!_isSupported(ret))
      fail = 1;
   ret = SDL_GetTextureColorMod( tface, &r, &g, &b );
   if (!_isSupported(ret))
      fail = 1;

   /* Clean up. */
   SDL_DestroyTexture( tface );

   if (fail)
      return 0;
   else if ((r != 100) || (g != 100) || (b != 100))
      return 0;
   return 1;
}

/**
 * @brief Test to see if we can vary the alpha of the texture. Helper function.
 *
 * \sa
 *  http://wiki.libsdl.org/moin.cgi/SDL_SetTextureAlphaMod
 *  http://wiki.libsdl.org/moin.cgi/SDL_GetTextureAlphaMod
 *  http://wiki.libsdl.org/moin.cgi/SDL_DestroyTexture
 */
static int
_hasTexAlpha(void)
{
   int fail;
   int ret;
   SDL_Texture *tface;
   Uint8 a;

   /* Get test face. */
   tface = _loadTestFace();
   if (tface == 0)
      return 0;

   /* See if supported. */
   fail = 0;
   ret = SDL_SetTextureAlphaMod( tface, 100 );
   if (!_isSupported(ret))
      fail = 1;
   ret = SDL_GetTextureAlphaMod( tface, &a );
   if (!_isSupported(ret))
      fail = 1;

   /* Clean up. */
   SDL_DestroyTexture( tface );

   if (fail)
      return 0;
   else if (a != 100)
      return 0;
   return 1;
}

static _renderCompareCount = 0;

/**
 * @brief Compares screen pixels with image pixels. Helper function.
 *
 * @param msg Message on failure.
 * @param s Image to compare against.
 * @return 0 on success.
 *
 * \sa
 * http://wiki.libsdl.org/moin.cgi/SDL_RenderReadPixels
 * http://wiki.libsdl.org/moin.cgi/SDL_CreateRGBSurfaceFrom
 * http://wiki.libsdl.org/moin.cgi/SDL_FreeSurface
 */
static void
_compare(const char *msg, SDL_Surface *s, int allowable_error)
{
   int ret;
   SDL_Rect rect;
   Uint8 pix[4*80*60];
   SDL_Surface *testsur;
   char imageFilename[128];
   char referenceFilename[128];

   /* Read pixels. */
   /* Explicitly specify the rect in case the window isn't expected size... */
   rect.x = 0;
   rect.y = 0;
   rect.w = 80;
   rect.h = 60;
   ret = SDL_RenderReadPixels(renderer, &rect, RENDER_COMPARE_FORMAT, pix, 80*4 );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_RenderReadPixels, expected: 0, got: %i", ret);

   /* Create surface. */
   testsur = SDL_CreateRGBSurfaceFrom( pix, TESTRENDER_SCREEN_W, TESTRENDER_SCREEN_H, 32, TESTRENDER_SCREEN_W*4,
                                       RENDER_COMPARE_RMASK, RENDER_COMPARE_GMASK, RENDER_COMPARE_BMASK, RENDER_COMPARE_AMASK);
   SDLTest_AssertCheck(testsur != NULL, "Verify result from SDL_CreateRGBSurfaceFrom");

   /* Compare surface. */
   ret = SDLTest_CompareSurfaces( testsur, s, allowable_error );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDLTest_CompareSurfaces, expected: 0, got: %i", ret);

   _renderCompareCount++;
   if (ret != 0) {
      SDL_snprintf(imageFilename, 127, "image%i.bmp", _renderCompareCount);
      SDL_SaveBMP(testsur, imageFilename);
      SDL_snprintf(referenceFilename, 127, "reference%i.bmp", _renderCompareCount);
      SDL_SaveBMP(s, referenceFilename);
      SDLTest_LogError("Surfaces from failed comparison saved as %s and %s", imageFilename, referenceFilename);
   }

   /* Clean up. */
   SDL_FreeSurface( testsur );
}

/**
 * @brief Clears the screen. Helper function.
 *
 * \sa
 * http://wiki.libsdl.org/moin.cgi/SDL_SetRenderDrawColor
 * http://wiki.libsdl.org/moin.cgi/SDL_RenderFillRect
 * http://wiki.libsdl.org/moin.cgi/SDL_SetRenderDrawBlendMode
 */
static int
_clearScreen(void)
{
   int ret;

   /* Set colour. */
   ret = SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_SetRenderDrawColor, expected: 0, got: %i", ret);

   /* Clear screen. */
   ret = SDL_RenderFillRect(renderer, NULL );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_RenderFillRect, expected: 0, got: %i", ret);

   /* Set defaults. */
   ret = SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_SetRenderDrawBlendMode, expected: 0, got: %i", ret);

   ret = SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE );
   SDLTest_AssertCheck(ret == 0, "Validate result from SDL_SetRenderDrawColor, expected: 0, got: %i", ret);

   return 0;
}

/* ================= Test References ================== */

/* Render test cases */
static const SDLTest_TestCaseReference renderTest1 =
		{ (SDLTest_TestCaseFp)render_testGetNumRenderDrivers, "render_testGetNumRenderDrivers", "Tests call to SDL_GetNumRenderDrivers", TEST_ENABLED };

static const SDLTest_TestCaseReference renderTest2 =
		{ (SDLTest_TestCaseFp)render_testPrimitives, "render_testPrimitives", "Tests rendering primitives", TEST_ENABLED };

static const SDLTest_TestCaseReference renderTest3 =
		{ (SDLTest_TestCaseFp)render_testPrimitivesBlend, "render_testPrimitivesBlend", "Tests rendering primitives with blending", TEST_ENABLED };

static const SDLTest_TestCaseReference renderTest4 =
		{ (SDLTest_TestCaseFp)render_testBlit, "render_testBlit", "Tests blitting", TEST_ENABLED };

static const SDLTest_TestCaseReference renderTest5 =
		{ (SDLTest_TestCaseFp)render_testBlitColor, "render_testBlitColor", "Tests blitting with color", TEST_ENABLED };

static const SDLTest_TestCaseReference renderTest6 =
		{ (SDLTest_TestCaseFp)render_testBlitAlpha, "render_testBlitAlpha", "Tests blitting with alpha", TEST_ENABLED };

static const SDLTest_TestCaseReference renderTest7 =
		{  (SDLTest_TestCaseFp)render_testBlitBlend, "render_testBlitBlend", "Tests blitting with blending", TEST_ENABLED };

/* Sequence of Render test cases */
static const SDLTest_TestCaseReference *renderTests[] =  {
	&renderTest1, &renderTest2, &renderTest3, &renderTest4, &renderTest5, &renderTest6, &renderTest7, NULL
};

/* Render test suite (global) */
SDLTest_TestSuiteReference renderTestSuite = {
	"Render",
	InitCreateRenderer,
	renderTests,
	CleanupDestroyRenderer
};
