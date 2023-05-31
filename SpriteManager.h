#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>
using std::string;
using std::vector;

struct Sprite {
	SDL_Texture * texture;
	string keyword;
	Sprite(SDL_Texture * t, string k);
};

struct tRect : public SDL_Rect {
	string tag;
	tRect(int x, int y, int w, int h, string tag) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->tag = tag;
	}
};

namespace SpriteManager {
	extern vector<Sprite*> sprites;
	extern SDL_Renderer * screen;
	
	void init(SDL_Renderer * renderer);
	void close();
	bool load(string keyword, string file);
	bool draw(string keyword, SDL_Rect Rect, double angle = 0.0, bool hflip = false);
}
#endif
