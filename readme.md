# imgr3ds

A basic (beta) Imgur client for the 3DS.

![imgr3ds UI](https://raw.githubusercontent.com/sereneblue/imgr3ds/master/ui.png?raw=true)

This client does not use OAuth so it is limited to browsing the hottest posts (frontpage).

### Features

	- Browse the latest Imgur posts
	- View Imgur albums (up to 10 images)
	- View JPG/JPEG/PNG images
	- View some information about a post (upvotes, author, etc)
	- Can zoom images

### Buttons:

	- (L) Go to prev post
	- (R) Go to next post
	- (A) Go forward on menu
	- (B) Go back to menu
	- (X) Zoom In 0.25x
	- (Y) Zoom out 0.25x
	- (Left) Go to previous image in album
	- (Right) Go to next image in album

### Todo:

	- Refactor
	- Add support to scroll an image, might change some button mappings in future updates
	- Add some error handling when loading the json data. Imgur's API sometimes throws an error

GIF/WEBM support won't be added. Sadly the O3DS's network speed is pretty slow and it'd be a lot of work to get something decent working. Some images have issues with sfillib so they won't display correctly and some posts don't display correctly, not sure what's causing that issue though.

### Credits

	- xerpi for sftdlib, sf2dlib, sfillib
	- smealum, et. al for the great work on libctru
	- Dave Gamble for the cJSON library
	- rxi for the vector library
	- Mozilla for the Fira Sans font