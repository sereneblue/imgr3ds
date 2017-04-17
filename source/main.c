#include <string.h>
#include <stdio.h>
#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include <sftd.h>
#include "api.h"

#include "fira_sans_semibold_ttf.h"

sf2d_texture *NO_GIF;

int main() {
	sf2d_init();
	sf2d_set_clear_color(RGBA8(178,187,187,255));
	sf2d_set_vblank_wait(0);
	httpcInit(0x1000);

	sftd_init();
	sftd_font *font = sftd_load_font_mem(fira_sans_semibold_ttf, fira_sans_semibold_ttf_size);
	// cache font to minimize blurs
	sftd_draw_textf(font, 0, 0, RGBA8(255, 0, 0, 255), 32, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890:-.'!?()");
	NO_GIF = sfil_load_PNG_file("sdmc:/3ds/imgr3ds/gif.png", SF2D_PLACE_RAM);

	while (aptMainLoop()) {
		draw_screen(font);

		hidScanInput();
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break;
		if (kDown & KEY_X) zoom_in();
		if (kDown & KEY_Y) zoom_out();

		if (kDown & KEY_UP) decrement();
		if (kDown & KEY_DOWN) increment();
		if (kDown & KEY_LEFT) prev_album_img();
		if (kDown & KEY_RIGHT) next_album_img();
		if (kDown & KEY_B) back();
		if (kDown & KEY_A) forward();
		if (kDown & KEY_L) prev();
		if (kDown & KEY_R) next();

		sf2d_swapbuffers();
	}

	sf2d_free_texture(NO_GIF);
	httpcExit();
	sftd_fini();
	sf2d_fini();
	return 0;
}
