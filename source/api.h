#include "vec.h"
#include <sf2d.h>

#ifndef api__h
#define api__h

void get_images();

struct album {
	char *id;
	char *description;
	char *ext;
	short animated;
	short width;
	short height;
};

typedef vec_t(struct album) vec_album_t;

typedef struct post {
	char *title;
	char *account;
	char *score;
    short ups_width;
    int index;
    vec_album_t album_imgs;
} post;

typedef struct imgr {
	vec_str_t posts;
	short post_count;
} imgr;

typedef struct scroll {
	short index;
	short num_items;
	short num_display;
} scroll;

extern imgr gallery;
extern post imgr_post;
extern sf2d_texture *NO_GIF;

#endif