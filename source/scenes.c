#include <string.h>
#include <stdio.h>
#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include <sftd.h>
#include "api.h"
#include "vec.h"

const u32 WHITE = RGBA8(255,255,255,255);
const u32 GRAY = RGBA8(43,43,43,255);
const u32 GRAY1 = RGBA8(178,187,187,255);
const u32 GREEN = RGBA8(137,198,35,255);
const u32 GREEN2 = RGBA8(103,149,26,255);
const u32 RED = RGBA8(238,68,68,255);

scroll scenes[3] = {
	{0, 2, 2},
	{0, 0, 0},
	{0, 0, 0}
};

short cur_scene = 0;
imgr gallery;
post imgr_post;
u8 *current_img;
unsigned long buffer_size;
float scale_factor = 1.0;

void draw_start(sftd_font* font) {
	sf2d_texture *start = sfil_load_PNG_file("sdmc:/3ds/imgr3ds/start.png", SF2D_PLACE_RAM);
	short width = 0;

	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(start, 0, 0);
	sf2d_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_rectangle(0, 0, 320, 240, GRAY);

		// selected button
		if (scenes[cur_scene].index == 0) {
			sf2d_draw_rectangle(20, 50, 280, 52, GREEN2);
		} else {
			sf2d_draw_rectangle(20, 152, 280, 52, GREEN2);
		}

		sf2d_draw_rectangle(23, 53, 275, 46, GREEN);
		sf2d_draw_rectangle(23, 155, 275, 46, GREEN);

		width = sftd_get_text_width(font, 24, "browse");
		sftd_draw_text(font, (320 - width) / 2, 59, (scenes[cur_scene].index == 0) ? GREEN2 : WHITE, 24, "browse");
		width = sftd_get_text_width(font, 24, "help");
		sftd_draw_text(font, (320 - width) / 2, 161, (scenes[cur_scene].index == 1) ? GREEN2 : WHITE, 24, "help");
	sf2d_end_frame();

	sf2d_free_texture(start);
}

void draw_image(sftd_font* font) {
	short *width;
	short *height;
	short total_height;

	sf2d_texture *top;

	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		if ((strcmp(imgr_post.album_imgs.data[imgr_post.index].ext, ".jpg") == 0) || (strcmp(imgr_post.album_imgs.data[imgr_post.index].ext, ".jpeg") == 0)) {
			if (current_img) {
				top = sfil_load_JPEG_buffer(current_img, buffer_size, SF2D_PLACE_RAM);
				sf2d_draw_texture_scale(top, 0, 0, scale_factor, scale_factor);
			}
		} else if (strcmp(imgr_post.album_imgs.data[imgr_post.index].ext, ".png") == 0) {
			top = sfil_load_PNG_buffer(current_img, SF2D_PLACE_RAM);
			sf2d_draw_texture_scale(top, 0, 0, scale_factor, scale_factor);
		}  else {
			sf2d_draw_texture(NO_GIF, 0, 0);
		}
	sf2d_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sftd_calc_bounding_box(&width, &height, font, 14, 300, imgr_post.title);
		total_height = (int) height;

		sf2d_draw_rectangle(0, 0, 320, 240, GRAY);
		sftd_draw_text_wrap(font, 5, 10, WHITE, 14, 310, imgr_post.title);
		sftd_calc_bounding_box(&width, &height, font, 12, 300, imgr_post.account);
		total_height = ((int) height  + 7) + total_height;
		sftd_draw_text_wrap(font, 5, total_height, GRAY1, 12, 310, imgr_post.account);
		sftd_draw_text(font, 5, total_height + 15, WHITE, 14, imgr_post.score);
		sf2d_draw_rectangle(5, total_height + 40, imgr_post.ups_width, 5, GREEN);
		sf2d_draw_rectangle(5 + imgr_post.ups_width, total_height + 40, 310 - imgr_post.ups_width, 5, RED);
		if (imgr_post.album_imgs.length > 1) {
			sftd_draw_textf(font, 5, total_height + 50, GRAY1, 12, "%d of %d", imgr_post.index + 1, imgr_post.album_imgs.length);
			sftd_draw_text_wrap(font, 5, total_height + 65, GRAY1, 12, 310, imgr_post.album_imgs.data[imgr_post.index].description);
		} else {
			sftd_draw_text_wrap(font, 5, total_height + 50, GRAY1, 12, 310, imgr_post.album_imgs.data[imgr_post.index].description);
		}
	sf2d_end_frame();

	sf2d_free_texture(top);
}

void draw_help() {
	sf2d_texture *start = sfil_load_PNG_file("sdmc:/3ds/imgr3ds/start.png", SF2D_PLACE_RAM);
	sf2d_texture *help = sfil_load_PNG_file("sdmc:/3ds/imgr3ds/help.png", SF2D_PLACE_RAM);

	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(start, 0, 0);
	sf2d_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(help, 0, 0);
	sf2d_end_frame();

	sf2d_free_texture(start);
	sf2d_free_texture(help);
}

void increment(){
	if (scenes[cur_scene].index + 1 >= scenes[cur_scene].num_items) {
		scenes[cur_scene].index = 0;
	} else {
		scenes[cur_scene].index++;
	}
}

void decrement(){
	if (scenes[cur_scene].index - 1 < 0) {
		scenes[cur_scene].index = scenes[cur_scene].num_items - 1;
	} else {
		scenes[cur_scene].index--;
	}
}

void prev(){
	if (scenes[cur_scene].index - 1 >= 0) {
		scenes[cur_scene].index--;
	}
	get_image(gallery.posts.data[scenes[cur_scene].index]);
	dl_image();
}

void next(){
	if (scenes[cur_scene].index + 1 >= scenes[cur_scene].num_items) {
		get_images();
		scenes[cur_scene].index = 0;
	} else {
		scenes[cur_scene].index++;
	}
	get_image(gallery.posts.data[scenes[cur_scene].index]);
	dl_image();
}

void prev_album_img(){
	if (imgr_post.index - 1 >= 0) {
		imgr_post.index--;
		dl_image();
	}
}

void next_album_img(){
	if (imgr_post.album_imgs.length > 1 && imgr_post.index + 1 < imgr_post.album_imgs.length) {
		imgr_post.index++;
		dl_image();
	}
}

void forward(){
	switch (cur_scene) {
		case 0:
			switch (scenes[cur_scene].index) {
				case 0:
					get_images();
					cur_scene++;
					scenes[cur_scene].index = 0;
					scenes[cur_scene].num_items = gallery.post_count;
					get_image(gallery.posts.data[scenes[cur_scene].index]);

					dl_image();
					break;
				case 1:
					cur_scene = 2;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}

void back(){
	switch (cur_scene) {
		case 1:
			cur_scene--;
			break;
		case 2:
			cur_scene = 0;
			break;
		default:
			break;
	}
}

void zoom_out(){
	if (scale_factor > .25) {
		scale_factor = scale_factor - .25;
	}
}

void zoom_in(){
	if (scale_factor < 2) {
		scale_factor = scale_factor + .25;
	}
}

void draw_screen(sftd_font* font){
	switch (cur_scene) {
		case 0:
			draw_start(font);
			break;
		case 1:
			draw_image(font);
			break;
		case 2:
			draw_help();
			break;
		default:
			break;
	}
}

void dl_image() {
	char *url;
	asprintf(&url, "https://i.imgur.com/%s%s", imgr_post.album_imgs.data[imgr_post.index].id, imgr_post.album_imgs.data[imgr_post.index].ext);

	if ((strcmp(imgr_post.album_imgs.data[imgr_post.index].ext, ".jpg") == 0) || (strcmp(imgr_post.album_imgs.data[imgr_post.index].ext, ".jpeg") == 0) || (strcmp(imgr_post.album_imgs.data[imgr_post.index].ext, ".png") == 0)) {
		current_img = http_dl(url, &buffer_size);
	}
}