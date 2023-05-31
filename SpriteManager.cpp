#include "SpriteManager.h"
#include <iostream>

Sprite::Sprite(SDL_Texture * t, string k) {
	texture = t;
	keyword = k;
}

namespace SpriteManager {
	vector<Sprite*> sprites;
	SDL_Renderer * screen;
	
	void init(SDL_Renderer * renderer) {
		screen = renderer;
	}
	
	void close() {
		for(auto& sprite : sprites) {
			SDL_DestroyTexture(sprite->texture);
			sprite->keyword = "";
		}
	}
	
	bool load(string keyword, string file) {
		for(auto& sprite : sprites) if(sprite->keyword == keyword && sprite->texture != nullptr) return true;
		Sprite * sprite;
		SDL_Surface * temp = IMG_Load(file.c_str());
		
		if(temp != nullptr) {
			Sprite * sprite = new Sprite(SDL_CreateTextureFromSurface(screen, temp), keyword);
			sprites.push_back(sprite);
			SDL_FreeSurface(temp);
			
			if(sprite->texture == nullptr) {
				delete sprite;
				return false;
			} else return true;
		}
		return false;
	}
	
	bool draw(string keyword, SDL_Rect Rect, double angle, bool hflip) {
		
		Sprite * that;
		bool found = false;
		for(auto& sprite : sprites) if(sprite->keyword == keyword) {
			that = sprite;
			found = true;
			break;
		}
		if(!found) {
			//std::cout << "Sprite " << keyword << " was not found!" << std::endl;
			return false;
		}
		
		angle == 0.0 ? SDL_RenderCopy(screen, that->texture, nullptr, &Rect) : SDL_RenderCopyEx(screen, that->texture, NULL, &Rect, angle, nullptr, hflip ? (SDL_RendererFlip)(SDL_FLIP_VERTICAL) : (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL));
		return true;
	}
}
