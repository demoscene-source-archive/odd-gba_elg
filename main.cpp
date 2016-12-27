#include <gba_dma.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>
#include <gba_sprites.h>

#define packed
#include <krawall.h>
#undef packed

#include <math.h>

#include "config.h"
#include "modules.h"

#include <algorithm>

/* wait for rasterline */
static inline void waitline(unsigned long line) {
	while (REG_VCOUNT < line);
//	while (REG_VCOUNT != line);
}

unsigned moose_x = WIDTH / 2;
unsigned moose_y = 30;
unsigned moose_frame = 1;


void mod_callback(int event, int data) {
	switch (event) {
		case KRAP_CB_DONE:
		break;

		case KRAP_CB_MARK:
			// flytt elg!
		break;

		default: return;
	}
}

volatile unsigned frame_counter = 0;

void vblank() {
#ifdef MUSIC
	kramWorker();
#endif
	frame_counter++;
}

#include "solnedgang.pal.c"
#include "solnedgang.raw.c"
#include "solnedgang.map.c"

#include "moose.pal.c"
#include "moose.raw.c"
#include "moose.map.c"

int main() {
	InitInterrupt();

	SetMode(MODE_0 | BG2_ENABLE | OBJ_ENABLE | OBJ_1D_MAP);

	// setup BG2
	DMA3COPY(solnedgang_Palette, BG_COLORS, COPY32 | 128);
	DMA3COPY(solnedgang_Tiles, CHAR_BASE_ADR(0), COPY32 | (sizeof(solnedgang_Tiles) >> 2));
	DMA3COPY(solnedgang_Map, MAP_BASE_ADR(24), COPY32 | (sizeof(solnedgang_Map) >> 2));
	BGCTRL[2] = BG_256_COLOR | CHAR_BASE(0) | SCREEN_BASE(24) | BG_SIZE_0;

	// DIZABLE STUFF!
	for (unsigned i = 0; i < 128; i++) OAM[i].attr0 = OBJ_DISABLE;

	OAM[0].attr0 = OBJ_Y(80) | OBJ_16_COLOR; // | OBJ_MODE(1);
	OAM[0].attr1 = OBJ_X(120) | OBJ_SIZE(3);
	OAM[1].attr0 = OBJ_Y(80) | OBJ_16_COLOR; // | OBJ_MODE(1);
	OAM[1].attr1 = OBJ_X(120) | OBJ_SIZE(3);
	OAM[2].attr0 = OBJ_Y(80) | OBJ_16_COLOR; // | OBJ_MODE(1);
	OAM[2].attr1 = OBJ_X(120) | OBJ_SIZE(3);
	OAM[3].attr0 = OBJ_Y(80) | OBJ_16_COLOR; // | OBJ_MODE(1);
	OAM[3].attr1 = OBJ_X(120) | OBJ_SIZE(3);

	OAM[0].attr2 = 8 * 8 * 0;
	OAM[1].attr2 = 8 * 8 * 1;
	OAM[2].attr2 = 8 * 8 * 4;
	OAM[3].attr2 = 8 * 8 * 5;

	DMA3COPY(moose_Palette, OBJ_COLORS, COPY32 | 128);
	DMA3COPY(moose_Tiles, OBJ_BASE_ADR, COPY32 | (sizeof(moose_Tiles) >> 2));

	SetInterrupt(Int_Vblank, vblank);
	EnableInterrupt(Int_Vblank);

#ifdef MUSIC
	SetInterrupt(Int_Timer1, kradInterrupt);
	EnableInterrupt(Int_Timer1);
	kragInit(KRAG_INIT_MONO);
	krapCallback(mod_callback);
	krapPlay(&MUSIC, KRAP_MODE_LOOP, 0);
#endif

	frame_counter = 0;
	while (1) {
		unsigned frame = frame_counter;
		unsigned timer = frame;

		moose_x = WIDTH / 2 + ((((timer >> 5) & 3) - 2) << 4);
		moose_frame = (timer >> 5) % 3;

		waitline(160); // vblank

		OAM[2].attr2 = 8 * 8 * (2 + moose_frame * 2);
		OAM[3].attr2 = 8 * 8 * (3 + moose_frame * 2);


		OAM[0].attr0 = OAM[0].attr0 & ~0x00ff | OBJ_Y(moose_y);
		OAM[0].attr1 = OAM[0].attr1 & ~0x01ff | OBJ_X(moose_x - 64);
		OAM[1].attr0 = OAM[1].attr0 & ~0x00ff | OBJ_Y(moose_y);
		OAM[1].attr1 = OAM[1].attr1 & ~0x01ff | OBJ_X(moose_x);

		OAM[2].attr0 = OAM[2].attr0 & ~0x00ff | OBJ_Y(moose_y + 64);
		OAM[2].attr1 = OAM[2].attr1 & ~0x01ff | OBJ_X(moose_x - 64);
		OAM[3].attr0 = OAM[3].attr0 & ~0x00ff | OBJ_Y(moose_y + 64);
		OAM[3].attr1 = OAM[3].attr1 & ~0x01ff | OBJ_X(moose_x);
	}

	return 0;
}
