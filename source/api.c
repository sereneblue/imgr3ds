#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <3ds.h>
#include "api.h"
#include "cJSON.h"

imgr gallery;
post imgr_post;

Result http_get(const char *url, char **response_data) {
	Result ret=0;
	httpcContext context;
	char *newurl=NULL;
	u32 statuscode=0;
	u32 contentsize=0, readsize=0, size=0;
	u8 *buf, *lastbuf;

	do {
		ret = httpcOpenContext(&context, HTTPC_METHOD_GET, url, 1);
		// This disables SSL cert verification, so https:// will be usable
		ret = httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);

		// Set a User-Agent header so websites can identify your application
		ret = httpcAddRequestHeaderField(&context, "User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.102 Safari/537.36");

		// Tell the server we can support Keep-Alive connections.
		// This will delay connection teardown momentarily (typically 5s)
		// in case there is another request made to the same server.
		ret = httpcSetKeepAlive(&context, HTTPC_KEEPALIVE_ENABLED);
		ret = httpcAddRequestHeaderField(&context, "Connection", "Keep-Alive");

		ret = httpcBeginRequest(&context);
		if(ret!=0){
			httpcCloseContext(&context);
			if(newurl!=NULL) free(newurl);
			return ret;
		}

		ret = httpcGetResponseStatusCode(&context, &statuscode);
		if(ret!=0){
			httpcCloseContext(&context);
			if(newurl!=NULL) free(newurl);
			return ret;
		}

		if ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308)) {
			if(newurl==NULL) newurl = malloc(0x1000); // One 4K page for new URL
			if (newurl==NULL){
				httpcCloseContext(&context);
				return -1;
			}
			ret = httpcGetResponseHeader(&context, "Location", newurl, 0x1000);
			url = newurl; // Change pointer to the url that we just learned
			httpcCloseContext(&context); // Close this context before we try the next
		}
	} while ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308));

	if(statuscode!=200){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		return -2;
	}

	// This relies on an optional Content-Length header and may be 0
	ret=httpcGetDownloadSizeState(&context, NULL, &contentsize);
	if(ret!=0){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		return ret;
	}

	// Start with a single page buffer
	buf = (u8*)malloc(0x1000);
	if(buf==NULL){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		return -1;
	}

	do {
		// This download loop resizes the buffer as data is read.
		ret = httpcDownloadData(&context, buf+size, 0x1000, &readsize);
		size += readsize;
		if (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING){
				lastbuf = buf; // Save the old pointer, in case realloc() fails.
				buf = realloc(buf, size + 0x1000);
				if(buf==NULL){
					httpcCloseContext(&context);
					free(lastbuf);
					if(newurl!=NULL) free(newurl);
					return -1;
				}
			}
	} while (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING);

	if(ret!=0){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		free(buf);
		return -1;
	}

	// Resize the buffer back down to our actual final size
	lastbuf = buf;
	buf = realloc(buf, size);
	if(buf==NULL){ // realloc() failed.
		httpcCloseContext(&context);
		free(lastbuf);
		if(newurl!=NULL) free(newurl);
		return -1;
	}
	httpcCloseContext(&context);
	if (newurl!=NULL) free(newurl);

	*response_data = malloc((strlen(buf) + 1) * sizeof(char));
	strcpy(*response_data, buf);
	free(buf);
	return 0;
}

u8* http_dl(const char *url, unsigned long *len) {
	Result ret=0;
	httpcContext context;
	char *newurl=NULL;
	u32 statuscode=0;
	u32 contentsize=0, readsize=0, size=0;
	u8 *buf, *lastbuf;

	do {
		ret = httpcOpenContext(&context, HTTPC_METHOD_GET, url, 1);
		// This disables SSL cert verification, so https:// will be usable
		ret = httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);

		// Set a User-Agent header so websites can identify your application
		ret = httpcAddRequestHeaderField(&context, "User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.102 Safari/537.36");

		// Tell the server we can support Keep-Alive connections.
		// This will delay connection teardown momentarily (typically 5s)
		// in case there is another request made to the same server.
		ret = httpcSetKeepAlive(&context, HTTPC_KEEPALIVE_ENABLED);
		ret = httpcAddRequestHeaderField(&context, "Connection", "Keep-Alive");

		ret = httpcBeginRequest(&context);
		if(ret!=0){
			httpcCloseContext(&context);
			if(newurl!=NULL) free(newurl);
			return ret;
		}

		ret = httpcGetResponseStatusCode(&context, &statuscode);
		if(ret!=0){
			httpcCloseContext(&context);
			if(newurl!=NULL) free(newurl);
			return ret;
		}

		if ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308)) {
			if(newurl==NULL) newurl = malloc(0x1000); // One 4K page for new URL
			if (newurl==NULL){
				httpcCloseContext(&context);
				return -1;
			}
			ret = httpcGetResponseHeader(&context, "Location", newurl, 0x1000);
			url = newurl; // Change pointer to the url that we just learned
			httpcCloseContext(&context); // Close this context before we try the next
		}
	} while ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308));

	if(statuscode!=200){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		return -2;
	}

	// This relies on an optional Content-Length header and may be 0
	ret=httpcGetDownloadSizeState(&context, NULL, &contentsize);
	if(ret!=0){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		return ret;
	}

	// Start with a single page buffer
	buf = (u8*)malloc(0x1000);
	if(buf==NULL){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		return -1;
	}

	do {
		// This download loop resizes the buffer as data is read.
		ret = httpcDownloadData(&context, buf+size, 0x1000, &readsize);
		size += readsize;
		if (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING){
				lastbuf = buf; // Save the old pointer, in case realloc() fails.
				buf = realloc(buf, size + 0x1000);
				if(buf==NULL){
					httpcCloseContext(&context);
					free(lastbuf);
					if(newurl!=NULL) free(newurl);
					return -1;
				}
			}
	} while (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING);

	if(ret!=0){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		free(buf);
		return -1;
	}

	// Resize the buffer back down to our actual final size
	lastbuf = buf;
	buf = realloc(buf, size);
	if(buf==NULL){ // realloc() failed.
		httpcCloseContext(&context);
		free(lastbuf);
		if(newurl!=NULL) free(newurl);
		return -1;
	}
	httpcCloseContext(&context);
	if (newurl!=NULL) free(newurl);

	*len = size;
	return buf;
}

char* replace_char(char* str, char find, char replace){
    char *current_pos = strchr(str,find);
    while (current_pos){
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return str;
}

void get_images() {
	char *response;
	Result ret;

	ret = http_get("https://api.imgur.com/3/gallery/hot/viral/0.json", &response);

 	cJSON *root = cJSON_Parse(response);
 	cJSON *posts = cJSON_GetObjectItem(root, "data");

 	gallery.post_count = cJSON_GetArraySize(posts);
 	vec_init(&gallery.posts);

    for (short i = 0; i < gallery.post_count; i++) {
		cJSON *subitem = cJSON_GetArrayItem(posts, i);
		char *hash = cJSON_GetObjectItem(subitem, "id")->valuestring; // supposed to be hash, but for some reason it's parsed as id?
    	vec_push(&gallery.posts, hash);
	}
}

void get_image(char *id) {
	char *response;
	char *url;
	Result ret = 0;

	asprintf(&url, "https://imgur.com/gallery/%s.json", id);

	ret = http_get(url, &response);

 	cJSON *root = cJSON_Parse(response);
 	cJSON *data = cJSON_GetObjectItem(root, "data");
 	cJSON *image = cJSON_GetObjectItem(data, "image");

 	vec_init(&imgr_post.album_imgs);
 	imgr_post.index = 0;
	imgr_post.account = cJSON_GetObjectItem(image, "account_url")->valuestring;
	imgr_post.title = cJSON_GetObjectItem(image, "title")->valuestring;
	asprintf(&imgr_post.score, "%d points", cJSON_GetObjectItem(image, "score")->valueint);
	int ups = cJSON_GetObjectItem(image, "ups")->valueint;
	int downs = cJSON_GetObjectItem(image, "downs")->valueint;
	imgr_post.ups_width = (310 * ((ups * 100) /(ups + downs))) / 100;

 	if (cJSON_GetObjectItem(image, "is_album")->valueint) {
 		cJSON *album_images = cJSON_GetObjectItem(image, "album_images");
		cJSON *images = cJSON_GetObjectItem(album_images, "images");
		short count = cJSON_GetArraySize(images);

 		for (short i = 0; i < count; i++) {
			cJSON *subitem = cJSON_GetArrayItem(images, i);

	 		struct album imgr_album;
	 		char *description = cJSON_GetObjectItem(subitem, "description")->valuestring;
	 		imgr_album.id = cJSON_GetObjectItem(subitem, "hash")->valuestring;
	 		if ((description != NULL) && (description[0] == '\0')) {
	 			imgr_album.description = replace_char(description, '\n', ' ');
	 		} else {
				imgr_album.description = "No description";
	 		}
	 		imgr_album.ext = cJSON_GetObjectItem(subitem, "ext")->valuestring;
	 		imgr_album.width = cJSON_GetObjectItem(subitem, "width")->valueint;
	 		imgr_album.height = cJSON_GetObjectItem(subitem, "height")->valueint;
	 		imgr_album.animated = cJSON_GetObjectItem(subitem, "animated")->valueint;
	 		vec_push(&imgr_post.album_imgs, imgr_album);
		}
 	} else {
 		struct album imgr_album;
 		char *description = cJSON_GetObjectItem(image, "description")->valuestring;
		if ((description != NULL) && (description[0] == '\0')) {
 			imgr_album.description = replace_char(description, '\n', ' ');
 		} else {
			imgr_album.description = "No description";
 		}
 		imgr_album.id = cJSON_GetObjectItem(image, "hash")->valuestring;
 		imgr_album.ext = cJSON_GetObjectItem(image, "ext")->valuestring;
 		imgr_album.width = cJSON_GetObjectItem(image, "width")->valueint;
 		imgr_album.height = cJSON_GetObjectItem(image, "height")->valueint;
 		imgr_album.animated = cJSON_GetObjectItem(image, "animated")->valueint;

 		vec_push(&imgr_post.album_imgs, imgr_album);
 	}
}