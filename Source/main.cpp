extern "C"
{
#include <os.h>
#include <ngc.h>
}

/*void *operator new(size_t size)
{
    return malloc(size);
}
 
void *operator new[](size_t size)
{
    return malloc(size);
}
 
void operator delete(void *p)
{
    free(p);
}
 
void operator delete[](void *p)
{
    free(p);
}*/

#include "Board.h"
using namespace stacker;

void Draw(Board&, Gc);
bool QuitKeyPressed();
bool StepKeyPressed();

int main()
{
   /* Get the gc */
   Gc gc = gui_gc_global_GC();

   /* Initialization */
   gui_gc_begin(gc);
   gui_gc_setRegion(gc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
   gui_gc_clipRect(gc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GC_CRO_SET);

   gui_gc_setColorRGB(gc, 255, 255, 255);
   gui_gc_fillRect(gc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

   Board b;

   while(!b.IsGameOver() && !QuitKeyPressed())
   {
      //Adjust the board
      b.Step();

      //Show the board
      Draw(b, gc);
      gui_gc_blit_to_screen(gc);

      if( StepKeyPressed() )
      {
         b.Stop();

	 //Wait
	 while(any_key_pressed());

	 b.Step();

	 //If won game
	 if( b.WonGame() )
         {
            Draw(b, gc);
            gui_gc_setColorRGB(gc, 255, 0, 0);
            gui_gc_clipRect(gc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GC_CRO_SET);
            int nFontHeight = gui_gc_getFontHeight(gc, gui_gc_getFont(gc));
            int nTextWidth = gui_gc_getStringWidth(gc, gui_gc_getFont(gc), "Y\0o\0u\0 \0W\0i\0n\0!\0!\0!\0", 0, 10);//10 characters
            int x = SCREEN_WIDTH/2 - nTextWidth/2, y = SCREEN_HEIGHT/2-nFontHeight/2;
            gui_gc_drawString(gc, "Y\0o\0u\0 \0W\0i\0n\0!\0!\0!\0", x, y, GC_SM_TOP);
            gui_gc_blit_to_screen(gc);
            gui_gc_clipRect(gc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GC_CRO_RESET);
	    while(!any_key_pressed())
	       sleep(100);
	    
	    break;
         }

      }

      sleep(100);//I could use a different speed depending on the height; but
      //it wasn't all that important; the game was fun at a constant speed! 
   }  

   gui_gc_finish(gc);

    // And quit
    return 0;
}

void Draw(Board& b, Gc gc)
{
   static bool boardCache[BOARD_HEIGHT][BOARD_WIDTH] = {0};
   int nWidthPerPiece = SCREEN_WIDTH / b.GetWidth();
   int nHeightPerPiece = SCREEN_HEIGHT / b.GetHeight();
   for(int nX=0; nX<b.GetWidth(); nX++)
      for(int nY=0; nY<b.GetHeight(); nY++)
      {
	 if( boardCache[nY][nX] != b.PieceAt(nX, nY) )
         {
            if( b.PieceAt(nX, nY) )
		gui_gc_setColorRGB(gc, 0, 255, 0);
            else
                gui_gc_setColorRGB(gc, 255, 255, 255);
	    gui_gc_clipRect(gc, nX * nWidthPerPiece, SCREEN_HEIGHT - ((nY+1) * nHeightPerPiece), nWidthPerPiece, nHeightPerPiece, GC_CRO_SET);
            gui_gc_fillRect(gc, nX * nWidthPerPiece, SCREEN_HEIGHT - ((nY+1) * nHeightPerPiece), nWidthPerPiece, nHeightPerPiece);
	    boardCache[nY][nX] = b.PieceAt(nX, nY);
	 }
      }
}

bool QuitKeyPressed()
{
   return isKeyPressed(KEY_NSPIRE_ESC) || isKeyPressed(KEY_NSPIRE_Q) || isKeyPressed(KEY_NSPIRE_HOME);
}

bool StepKeyPressed()
{
   return isKeyPressed(KEY_NSPIRE_ENTER) || isKeyPressed(KEY_NSPIRE_SPACE) || isKeyPressed(KEY_NSPIRE_CTRL);
}
