#include <sys/types.h>	// This provides typedefs needed by libgte.h and libgpu.h
#include <stdio.h>	// Printf
#include <psxetc.h>	// Includes some functions that controls the display
#include <psxgte.h>	// GTE header, not really used but libgpu.h depends on it
#include <psxgpu.h>	// GPU library header
#include <psxapi.h> // API header, has InitPAD() and StartPAD() defs
#include <psxpad.h> // Controller button defines
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#define SCREEN_RES_X 320
#define SCREEN_RES_Y 240
#define SCREEN_CENTER_X SCREEN_RES_X/2
#define SCREEN_CENTER_Y SCREEN_RES_Y/2
#define SPRITE_SIZE_X 128
#define SPRITE_SIZE_Y 74
#define RCntIntr 0x1000 // Interrupt mode
#define OTLEN 8 // Ordering table length (recommended to set as a define so it can be changed easily)
#define NUMBER_OF_COLORS 7 // There are 7 different CLUTS in VRAM for 7 different colors that can be switched to for one sprite                        


DISPENV disp[2]; // Display/drawing buffer parameters
DRAWENV draw[2];
int db = 0;
// PSn00bSDK requires having all uint32_t types replaced with
// u_int, as uint32_t in modern GCC that PSn00bSDK uses defines it as a 64-bit integer.
uint32_t rc_seed;
uint32_t cc; // Corner counter
uint32_t ot[2][OTLEN];    // Ordering table length
uint32_t controller_message_counter = 0;

char pribuff[2][32768]; // Primitive buffer
char *nextpri;          // Next primitive pointer

int tim_mode;           // TIM image parameters
RECT tim_prect,tim_crect;
int tim_uoffs,tim_voffs;
uint16_t tim_texpage;
uint16_t cur_x = SCREEN_CENTER_X;
uint16_t cur_y = SCREEN_CENTER_Y;
bool rev_x = 0;
bool rev_y = 0;
bool hit_exact_corner = 0;
bool never_show_hit_count = 0;
//bool just_hit = 0;
uint8_t bgflash = 0;
uint8_t screen_flash_counter = 0; // used to keep track of screen flashes between vsyncs
// pad buffer arrays
u_char padbuff[2][34];

void display() {
    FntFlush(-1); // Clear font

    DrawSync(0);                // Wait for any graphics processing to finish
    
    VSync(0);                   // Wait for vertical retrace

    PutDispEnv(&disp[db]);      // Apply the DISPENV/DRAWENVs
    PutDrawEnv(&draw[db]);

    SetDispMask(1);             // Enable the display

    DrawOTag(ot[db]+OTLEN-1);   // Draw the ordering table
    
    db = !db;                   // Swap buffers on every pass (alternates between 1 and 0)
    nextpri = pribuff[db];      // Reset next primitive pointer
    
}

// Texture upload function
void LoadTexture(uint32_t *tim, TIM_IMAGE *tparam) {

    // Read TIM parameters (PsyQ)
    //OpenTIM(tim);
    //ReadTIM(tparam);

    // Read TIM parameters (PSn00bSDK)
    GetTimInfo(tim, tparam);

    // Upload pixel data to framebuffer
    LoadImage(tparam->prect, (uint32_t*)tparam->paddr);
    DrawSync(0);

    // Upload CLUT to framebuffer if present
    if( tparam->mode & 0x8 ) {

        LoadImage(tparam->crect, (uint32_t*)tparam->caddr);
        DrawSync(0);

    }

}

void loadstuff(void) {

    TIM_IMAGE my_image;         // TIM image parameters

    extern uint32_t dvd_logo[];

    // Load the texture
    LoadTexture(dvd_logo, &my_image); // 640, 0

    // Copy the TIM coordinates
    tim_prect   = *my_image.prect;
    tim_crect   = *my_image.crect;
    tim_mode    = my_image.mode;

    // Calculate U,V offset for TIMs that are not page aligned
    tim_uoffs = (tim_prect.x%64)<<(2-(tim_mode&0x3));
    //tim_voffs = (tim_prect.y&0xff);
    tim_voffs = 0; // Start with white
}


// To make main look tidy, init stuff has to be moved here
void init(void) {
	
	int CounterMaxValue = INT_MAX; // Counter resets on INTMAX amount of HBlanks

	SetRCnt(RCntCNT1, CounterMaxValue, RCntIntr);
	StartRCnt(RCntCNT1);
	srand(GetRCnt(RCntCNT1));

    // Reset graphics
    ResetGraph(0);

    // First buffer
    SetDefDispEnv(&disp[0], 0, 0, SCREEN_RES_X, SCREEN_RES_Y);
    SetDefDrawEnv(&draw[0], 0, SCREEN_RES_Y, SCREEN_RES_X, SCREEN_RES_Y);
    // Second buffer
    SetDefDispEnv(&disp[1], 0, SCREEN_RES_Y, SCREEN_RES_X, SCREEN_RES_Y);
    SetDefDrawEnv(&draw[1], 0, 0, SCREEN_RES_X, SCREEN_RES_Y);

    draw[0].isbg = 1;               // Enable clear
    setRGB0(&draw[0], 0, 0, 0);  // Set clear color (black)
    draw[1].isbg = 1;
    setRGB0(&draw[1], 0, 0, 0);

    nextpri = pribuff[0];           // Set initial primitive pointer address
 
    // load textures and possibly other stuff
    loadstuff();

    // set tpage
	tim_texpage = getTPage(tim_mode & 3, 0, tim_prect.x, tim_prect.y);
    // apply initial drawing environment
    PutDrawEnv(&draw[!db]);

	// Load test font
	FntLoad(960, 0);
	
	// Open up a test font text stream of 200 characters
	FntOpen(0, 8, 320, 224, 0, 200);

    // Initialize the pads
    InitPAD( padbuff[0], 34, padbuff[1], 34 );
    
    // Begin polling
    StartPAD();
    
    // To avoid VSync Timeout error, may not be defined in PsyQ
    ChangeClearPAD(1);
 
}

void screen_flash(uint8_t bgflash) {
	if(bgflash == 1) {
		setRGB0(&draw[0], 255, 255, 255);  // Set clear color (white)
		setRGB0(&draw[1], 255, 255, 255);
	} else {
		setRGB0(&draw[0], 0, 0, 0);  // Set clear color (black)
		setRGB0(&draw[1], 0, 0, 0);
	}
}

void change_logo_color () { // May or may not change it, could randomly be the same color! Uses CLUT
    rc_seed = rand() % NUMBER_OF_COLORS;
    if(rc_seed == 0) {
        tim_crect.y = 480; // CLUT slot 0 (white)
    }
    if(rc_seed == 1) {
        tim_crect.y = 481; // CLUT slot 1 (blue)
    }
    if(rc_seed == 2) {
        tim_crect.y = 482; // CLUT slot 2 (red)
    }
    if(rc_seed == 3) {
        tim_crect.y = 483; // CLUT slot 3 (violet)
    }
    if(rc_seed == 4) {
        tim_crect.y = 484; // CLUT slot 4 (yellow)
    }
    if(rc_seed == 5) {
        tim_crect.y = 485; // CLUT slot 5 (green)
    }
    if(rc_seed == 6) {
        tim_crect.y = 486; // CLUT slot 6 (orange)
    }
	printf("COLOR SELECT: %d\n", rc_seed);
	printf("LOGO X VAL: %d\n", cur_x);
	printf("LOGO Y VAL: %d\n", cur_y);
	printf("CLUT Y VAL: %d\n", tim_crect.y);
}

void change_logo_pos () {

	if(!rev_x){
		if(cur_x < (SCREEN_RES_X - SPRITE_SIZE_X)) {
			cur_x++;
		} else {
			rev_x = 1;
			change_logo_color();
		}
	}

	if(rev_x) {
		if(cur_x != 0) {
			cur_x--;
		} else {
			rev_x = 0;
			change_logo_color();
		}
	}

	if(!rev_y){
		if(cur_y < (SCREEN_RES_Y - SPRITE_SIZE_Y)) {
			cur_y++;
		} else {
			rev_y = 1;
			change_logo_color();
		}
	}

	if(rev_y) {
		if(cur_y != 0) {
			cur_y--;
		} else {
			rev_y = 0;
			change_logo_color();
		}
	}
	// 0, 0
	// 320-128, 0
	// 0, 240-74,
	// 320-128, 240-74

	if((cur_x == 0 && cur_y == 0) || (cur_x == (SCREEN_RES_X - SPRITE_SIZE_X) && cur_y == 0) || (cur_y == (SCREEN_RES_Y - SPRITE_SIZE_Y) && cur_x == 0) || (cur_x == (SCREEN_RES_X - SPRITE_SIZE_X) && cur_y == 0) || (cur_x == (SCREEN_RES_X - SPRITE_SIZE_X) && cur_y == (SCREEN_RES_Y - SPRITE_SIZE_Y)) && (!never_show_hit_count))
	{
		hit_exact_corner = true;
		controller_message_counter = 900; // 60 * 30 = 1800 VSYNCs ~15 seconds
		cc++;
		screen_flash_counter = 3; // Flash screen background black and white over 3 vsyncs
	}
}

void controller () { // Parse controller input
    PADTYPE *pad;						// Pointer for pad
    pad = (PADTYPE*)padbuff[0];

    // Only parse inputs when a controller is connected
    if( pad->stat == 0 ) {
        // Only parse when a digital pad, 
        // dual-analog and dual-shock is connected
        if((pad->type == 0x4) || (pad->type == 0x5) || (pad->type == 0x7)) {
             if( !(pad->btn&PAD_UP) )            // test UP
             {
                hit_exact_corner = 0;
                controller_message_counter = 0;
            }
            else if( !(pad->btn&PAD_DOWN) )       // test DOWN
            {
                hit_exact_corner = 0;
                never_show_hit_count = 1;
                controller_message_counter = 0;
            }
        }
    }
}

int main() {
    
    TILE *tile;                         // Pointer for TILE
    SPRT *sprt;                         // Pointer for SPRT

    // Init stuff
    init();
	
	controller_message_counter = 0; // Will display info about dismissing hit counter when non-zero for that many vsyncs

    while(1) {

        ClearOTagR(ot[db], OTLEN);      // Clear ordering table
    
        // Sort textured sprite
        
        sprt = (SPRT*)nextpri;

        setSprt(sprt);                  // Initialize the primitive (very important)
        setXY0(sprt, cur_x, cur_y);           // Position the sprite at (48,48)
        setWH(sprt, SPRITE_SIZE_X, SPRITE_SIZE_Y);            // Set sprite size
        setUV0(sprt, tim_uoffs, tim_voffs); // Set UV coordinates (can change the sprite)
        setClut(sprt,                   // Set CLUT coordinates to sprite
            tim_crect.x,
            tim_crect.y);
        setRGB0(sprt,                   // Set primitive color (neutral)
            128, 128, 128);
        addPrim(ot[db], sprt);          // Sort primitive to OT

        nextpri += sizeof(SPRT);        // Advance next primitive address

		// In your main loop, *after* (not before) linking the primitive you want to apply the texture to:
		DR_TPAGE *texpage = (DR_TPAGE *) nextpri;
		// - the second argument unlocks drawing to the currently displayed framebuffer (no reason to do that usually)
		// - the third argument enables dithering
		setDrawTPage(texpage, 0, 0, tim_texpage);

		addPrim(ot[db], texpage);
		nextpri += sizeof(texpage);

		if(hit_exact_corner && !never_show_hit_count) {
			FntPrint(-1, "EXACT CORNER HIT(S) : %d\n", cc);
			// Draw the last created text stream
			controller();
		}

		if(controller_message_counter != 0) {
			FntPrint(-1, "PRESS UP TO DISMISS UNTIL NEXT HIT\nPRESS DOWN TO DISMISS FOREVER\n");
			controller_message_counter--;
		}		

		if(screen_flash_counter != 0) {
			screen_flash(1);
			screen_flash_counter--;
		}

        // Update the display
        display();
        // Update logo
        change_logo_pos();
   		screen_flash(0);
    }
    
    return 0;
}