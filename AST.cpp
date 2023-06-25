#include "AST.h"
#include <thread>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <ctime>

// ############### Namespace AST Begin ###############
namespace AST {
// ############### Variables Begin ###############
	SDL_Window* win;
	SDL_Renderer* ren;
	SDL_Point Mouse;
	int FPS = 60;
	bool loop;
	bool isFullscreen = false;
	std::unordered_map<int, bool> events;
	std::unordered_map<int, bool> keys;
	int code;
	std::function<void(SDL_Event&)> EventHandler = [](SDL_Event&){
		if(AST::events[SDL_QUIT]) AST::loop = false;
	};
// ############### Variables End ###############
// ############### Utility Functions Begin ###############
	SDL_Color RandomColor() {
		SDL_Color color;
		color.r = rand() % 256;
		color.g = rand() % 256;
		color.b = rand() % 256;
		color.a = rand() % 256;
		return color;
	}
	bool InRange(int num, int min, int max) {
		return (num >= min && num <= max);
	}
	bool Hovering(SDL_FRect rect) {
		return InRange(Mouse.x, rect.x, rect.x + rect.w) && InRange(Mouse.y, rect.y, rect.y + rect.h);
	}
	bool Clicked(SDL_FRect rect, int key) {
		return keys[key] && Hovering(rect);
	}
	void Fullscreen(bool yes) {
		isFullscreen = yes;
		SDL_SetWindowFullscreen(win, yes ? SDL_WINDOW_FULLSCREEN : 0);
	}
	void SetTimeout(std::function<void()> function, int ms) {
		std::thread([function, ms]() {
			SDL_Delay(ms);
			function();
		}).detach();
	}
// ############### Utility Functions End ###############
// ############### Class Scene Begin ###############
	Scene::Scene() { AST::loop = true; }
	void Scene::loop() {}
	void Scene::event(SDL_Event&) {}
	Scene::~Scene() {
#ifdef AST_TEXTURE
		SpriteManager::Free();
#endif
	}
// ############### Class Scene End ###############
// ############### Class Rect Begin ###############
	Rect::Rect(SDL_FRect rect, std::string texture) {
		Init(rect);
		this->texture = texture;
	}
	Rect::Rect(SDL_FRect rect, SDL_Color color) {
		Init(rect);
		this->color = color;
	}

	void Rect::Init(SDL_FRect rect) {
		this->x = rect.x == -1 ? rand() % 1920 : rect.x;
		this->y = rect.y == -1 ? rand() % 1080 : rect.y;

		this->w = rect.w == -1 ? 1920 : rect.w;
		this->h = rect.h == -1 ? 1080 : rect.h;

		#ifndef AST_PHYSICS
			angle = 0.0;
		#endif
		filled = true;
	}
	void Rect::initlayer(SDL_Color color) {
		this->layer = new AST::Rect(*this, color);
		this->layer->filled = false;
	}
	Rect::~Rect() {
#ifdef AST_PHYSICS
		// if(this->StaticRect != nullptr) delete this->StaticRect;
		// if(this->KinematicRect != nullptr) delete this->KinematicRect;
		// if(this->DynamicRect != nullptr) delete this->DynamicRect;
#endif
	}
	// ############### Class Rect End ###############
	// ############### Class Text Begin ###############
#ifdef AST_TEXT
	Text::Text() {}
	Text::~Text() {
		this->free();
	}
	void Text::set(TTF_Font * font, std::string text, SDL_Color color) {
		if(texture != nullptr) SDL_DestroyTexture(texture);
		SDL_Surface * surface = TTF_RenderText_Solid(font , text.c_str(), color);
		texture = SDL_CreateTextureFromSurface(ren, surface);
		this->w = surface->w;
		this->h = surface->h;
		SDL_FreeSurface(surface);
	}
	void Text::draw() { SDL_RenderCopyEx(AST::ren, texture, nullptr, this, angle, nullptr, SDL_FLIP_NONE); }
	void Text::free() { SDL_DestroyTexture(texture); }
#endif
	// ############### Class Text End ###############
	// ############### Basic Functions Begin ###############
void Init(std::string title, SDL_Rect rect) {
	srand(time(nullptr));

    SDL_Init(SDL_INIT_EVERYTHING);
#ifdef AST_TEXT
    TTF_Init();
#endif
#ifdef AST_TEXTURE
    IMG_Init(IMG_INIT_JPG
		IMG_INIT_PNG
		IMG_INIT_TIF
		IMG_INIT_WEBP
		IMG_INIT_JXL
		IMG_INIT_AVIF
	);
#endif
#ifdef AST_AUDIO
    Mix_Init(
    	MIX_INIT_FLAC | 
    	MIX_INIT_MOD | 
    	MIX_INIT_MP3 | 
    	MIX_INIT_OGG | 
    	MIX_INIT_MID | 
    	MIX_INIT_OPUS
    );
    Mix_OpenAudio(
    	MIX_DEFAULT_FREQUENCY, 
    	MIX_DEFAULT_FORMAT, 
    	MIX_DEFAULT_CHANNELS, 
    	4096
    );
#endif
#ifdef AST_NET
    SDLNet_Init();
#endif

    win = SDL_CreateWindow(title.c_str(), rect.x == -1 ? SDL_WINDOWPOS_CENTERED : rect.x, rect.y == -1 ? SDL_WINDOWPOS_CENTERED : rect.y, rect.w, rect.h, rect.w == -1 && rect.h == -1 ? (SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED) : 0);
    SDL_SetWindowResizable(win, SDL_FALSE);
    
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(ren, 1920, 1080);

    SDL_Rect windowRect;
    SDL_GetWindowSize(win, &windowRect.w, &windowRect.h);
    float scaleX = static_cast<float>(windowRect.w) / 1920;
    float scaleY = static_cast<float>(windowRect.h) / 1080;
    SDL_RenderSetScale(ren, scaleX, scaleY);
}
	
	void Render(Scene & scene) {
		while (AST::loop) {
			Uint32 frameStart = SDL_GetTicks();

			//Event Handling
			SDL_Event event;
			events.clear();
			while (SDL_PollEvent(&event)) {
				SDL_GetMouseState(&Mouse.x, &Mouse.y);

				int windowWidth, windowHeight;
				SDL_GetRendererOutputSize(ren, &windowWidth, &windowHeight);

				float scaleX = static_cast<float>(windowWidth) / 1920;
				float scaleY = static_cast<float>(windowHeight) / 1080;

				Mouse.x = static_cast<int>(Mouse.x / scaleX);
				Mouse.y = static_cast<int>(Mouse.y / scaleY);

				events[event.type] = true;

				if (event.type == SDL_MOUSEBUTTONDOWN) keys[event.button.button] = true;
				if(event.type == SDL_MOUSEBUTTONUP) keys[event.button.button] = false;

				if (event.type == SDL_KEYDOWN) keys[event.key.keysym.sym] = true;
				if (event.type == SDL_KEYUP) keys[event.key.keysym.sym] = false;

				EventHandler(event);
				scene.event(event);
			}
			//Rendering
			SDL_RenderClear(ren);
			scene.loop();
			SDL_RenderPresent(ren);

			Uint32 frameTime = SDL_GetTicks() - frameStart;
			if (frameTime < 1000/FPS) SDL_Delay(1000/FPS - frameTime);
		}
	}

	void Quit() {
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(ren);
		SDL_Quit();
#ifdef AST_TEXT
		TTF_Quit();
#endif
#ifdef AST_AUDIO
		Mix_Quit();
#endif
	}
	// ############### Basic Functions End ###############
} // namespace AST
// ############### Namespace AST End ###############
// ############### Namespace SpriteManager Begin ###############
namespace SpriteManager {
#ifdef AST_TEXTURE
	std::unordered_map<std::string, SDL_Texture*> Sprites;

	bool Load(std::string keyword, std::string file) {
		if(Sprites[keyword]) return true;

	    SDL_Texture* texture = IMG_LoadTexture(AST::ren, ("Resources/GFX/" + file).c_str());
	    if (!texture) return false;

	    Sprites[keyword = texture];
	    return true;
	}

	bool Load(std::string keyword, std::string sheetFile, SDL_Rect spriteRect) {
	    if(Sprites[keyword]) return true;

	    SDL_Texture* sheetTexture = IMG_LoadTexture(AST::ren, ("Resources/GFX/" + sheetFile).c_str());
	    if (sheetTexture == nullptr) return false;

	    SDL_Texture* spriteTexture = SDL_CreateTexture(AST::ren, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_TARGET, spriteRect.w, spriteRect.h);
	    if (spriteTexture == nullptr) {
	        SDL_DestroyTexture(sheetTexture);
	        return false;
	    }

	    SDL_SetRenderTarget(AST::ren, spriteTexture);
	    SDL_RenderCopy(AST::ren, sheetTexture, &spriteRect, nullptr);
	    SDL_SetRenderTarget(AST::ren, nullptr);
	    Sprites[keyword] = spriteTexture;

	    SDL_DestroyTexture(sheetTexture);

	    return true;
	}

	void Free() {
		for (auto& Sprite : Sprites) SDL_DestroyTexture(Sprite.second);
		Sprites.clear();
	}

#endif

	bool Render(AST::Rect rect) {
		if(rect.texture.empty()) {
			SDL_SetRenderDrawColor(AST::ren, rect.color.r, rect.color.g, rect.color.b, rect.color.a);
			if(rect.filled) SDL_RenderFillRectF(AST::ren, &rect);
			else SDL_RenderDrawRectF(AST::ren, &rect);
			SDL_SetRenderDrawColor(AST::ren, 0, 0, 0, 255);
		}
		#ifdef AST_TEXTURE 
		else {
			if(!Sprites[rect.texture]) return false;
			SDL_RenderCopyExF(AST::ren, sprite.first, nullptr, &rect, rect.angle, nullptr, SDL_FLIP_NONE);
			return true;
		}
		#else
		#endif
		if(rect.layer != nullptr) {
			rect.layer->x = rect.x;	
			rect.layer->y = rect.y;
			rect.layer->w = rect.w;
			rect.layer->h = rect.h;		
			SpriteManager::Render(*rect.layer);
		}
	#ifdef AST_TEXT
		if(rect.text != nullptr) {
			rect.text->x = (rect.x + (rect.w / 2)) - (rect.text->w / 2);
			rect.text->y = (rect.y + (rect.h / 2)) - (rect.text->h / 2);
			rect.text->draw();
		}
	#endif
		return true;
	}

} // namespace SpriteManager
// ############### Namespace SpriteManager End ###############
// ############### Namespace Physics Begin ############### 
#ifdef AST_PHYSICS
namespace Physics {
	// ############### Variables Begin ###############
	float* xmax = nullptr, * xmin = nullptr, * ymax = nullptr, * ymin = nullptr;

	float Xmax, Ymax;

	float xmax2, ymax2, xmin2;

	int loop, i;

	SDL_FPoint a, b, c, d;

	Rects::Dynamic* tmpD;
	Rects::Dynamic* tmpD2;
	Rects::Static* tmpS;
	Rects::Kinematic* tmpK;
	// ############### Variables End ###############
	// ############### Namespace Rects Begin ###############
	namespace Rects {
		// ############### Structure World Begin ###############
		World::World(float AirFrictionCoefficient, float Gravity)  {
			if (xmax == nullptr) xmax = new float;
			if (xmin == nullptr) xmin = new float;
			if (ymax == nullptr) ymax = new float;
			if (ymin == nullptr) ymin = new float;

			this->gravity = Gravity;
			this->airfriction = AirFrictionCoefficient;
			this->StaticRectList = nullptr;
			this->KinematicRectList = nullptr;
			this->DynamicRectList = nullptr;
			this->gametype = Gravity == 0 ? ISOMETRIC : SIDESCROLLER;
		}
		World::~World() {
			Rects::Static* tmp = this->StaticRectList;
			while (this->StaticRectList != nullptr) {
				tmp = this->StaticRectList;
				this->StaticRectList = this->StaticRectList->sonraki;
				free(tmp);
				tmp = nullptr;
			}
			Rects::Kinematic* tmp2 = this->KinematicRectList;
			while (this->KinematicRectList != nullptr) {
				tmp2 = this->KinematicRectList;
				this->KinematicRectList = this->KinematicRectList->sonraki;
				free(tmp2);
				tmp2 = nullptr;
			}
			Rects::Dynamic* tmp3 = this->DynamicRectList;
			while (this->DynamicRectList != nullptr) {
				tmp3 = this->DynamicRectList;
				this->DynamicRectList = this->DynamicRectList->sonraki;
				free(tmp3);
				tmp3 = nullptr;
			}
			free(this);
			return;
		}

		int World::GetStaticRectNumber() {
			if (this == nullptr) return 0;
			int size = 0;
			Rects::Static* tmp = this->StaticRectList;
			while (tmp != nullptr) {
				size++;
				tmp = tmp->sonraki;
			}
			return size;
		}
		int World::GetKinematicRectNumber() {
			if (this == nullptr) {
				return 0;
			}
			int size = 0;
			Rects::Kinematic* tmp = this->KinematicRectList;
			while (tmp != nullptr) {
				size++;
				tmp = tmp->sonraki;
			}
			return size;
		}
		int World::GetDynamicRectNumber() {
			if (this == nullptr) {
				return 0;
			}
			int size = 0;
			Rects::Dynamic* tmp = this->DynamicRectList;
			while (tmp != nullptr) {
				size++;
				tmp = tmp->sonraki;
			}
			return size;
		}

		void World::Simulate(float ms) {
			tmpD = this->DynamicRectList;
			tmpD2 = this->DynamicRectList;
			tmpS = this->StaticRectList;
			tmpK = this->KinematicRectList;
			while (tmpK != nullptr) {
				tmpK->position->x = tmpK->position->x + (tmpK->velocity.x * (ms / 10));
				tmpK->position->y = tmpK->position->y + (tmpK->velocity.y * (ms / 10));
				tmpK = tmpK->sonraki;
			}
			while (tmpD != nullptr) {
				xmax2 = tmpD->velocity.x;
				tmpD->velocity.x = tmpD->velocity.x + (tmpD->force.x / tmpD->mass);
				if (tmpD->velocity.x > 0) {
					tmpD->velocity.x = tmpD->velocity.x - fabsf(xmax2 * this->airfriction * (ms / 1000));
					if (tmpD->alt == 1 || tmpD->ust == 1) {
						tmpD->velocity.x = tmpD->velocity.x - fabsf(xmax2 * tmpD->friction * (ms / 1000));
					}
					if (tmpD->velocity.x < 0.01f) {
						tmpD->velocity.x = 0;
					}
				}
				else if (tmpD->velocity.x < 0) {
					tmpD->velocity.x = tmpD->velocity.x + fabsf(xmax2 * this->airfriction * (ms / 1000));
					if (tmpD->alt == 1 || tmpD->ust == 1) {
						tmpD->velocity.x = tmpD->velocity.x + fabsf(xmax2 * tmpD->friction * (ms / 1000));
					}
					if (tmpD->velocity.x > -0.01f) {
						tmpD->velocity.x = 0;
					}
				}
				ymax2 = tmpD->velocity.y;
				tmpD->velocity.y = tmpD->velocity.y + (tmpD->force.y / tmpD->mass) + (this->gravity * (tmpD->gravitymultiplier) * (ms / 1000));
				if (tmpD->velocity.y > 0) {
					tmpD->velocity.y = tmpD->velocity.y - fabsf(ymax2 * this->airfriction * (ms / 1000));
					if (tmpD->sol == 1 || tmpD->sag == 1) {
						tmpD->velocity.y = tmpD->velocity.y - fabsf(ymax2 * tmpD->friction * (ms / 1000));
					}
					if (tmpD->velocity.y < 0.01f) {
						tmpD->velocity.y = 0;
					}
				}
				else if (tmpD->velocity.y < 0) {
					tmpD->velocity.y = tmpD->velocity.y + fabsf(ymax2 * this->airfriction * (ms / 1000));
					if (tmpD->sol == 1 || tmpD->sag == 1) {
						tmpD->velocity.y = tmpD->velocity.y + fabsf(ymax2 * tmpD->friction * (ms / 1000));
					}
					if (tmpD->velocity.y > -0.01f) {
						tmpD->velocity.y = 0;
					}
				}
				tmpD->force.x = 0;
				tmpD->force.y = 0;
				xmax2 = ((tmpD->velocity.x + tmpD->xk) * (ms / 10));
				ymax2 = ((tmpD->velocity.y + tmpD->yk) * (ms / 10));
				xmin2 = max(fabsf(xmax2), fabsf(ymax2));
				loop = (int)xmin2;
				if (loop < 1) {
					loop = 1;
				}
				tmpD->isonground = 0;
				tmpD->xk = 0;
				tmpD->yk = 0;
				tmpD->sol = 0;
				tmpD->sag = 0;
				tmpD->alt = 0;
				tmpD->ust = 0;
				if (xmin2 == 0) {
					tmpS = this->StaticRectList;
					while (tmpS != nullptr) {
						CollideDynamicStatic(tmpD, tmpS);
						tmpS = tmpS->sonraki;
					}
					tmpK = this->KinematicRectList;
					while (tmpK != nullptr) {
						CollideDynamicKinematic(tmpD, tmpK, (this->gravity * (tmpD->gravitymultiplier)), this->gametype);
						tmpK = tmpK->sonraki;
					}
					tmpD2 = this->DynamicRectList;
					while (tmpD2 != nullptr) {
						CollideDynamicDynamic(tmpD, tmpD2);
						tmpD2 = tmpD2->sonraki;
					}
				}
				else {
					for (i = 0; i < loop; i++) {
						tmpD->position->x = tmpD->position->x + (xmax2 / loop);
						tmpD->position->y = tmpD->position->y + (ymax2 / loop);
						tmpS = this->StaticRectList;
						while (tmpS != nullptr) {
							CollideDynamicStatic(tmpD, tmpS);
							tmpS = tmpS->sonraki;
						}
						tmpK = this->KinematicRectList;
						while (tmpK != nullptr) {
							CollideDynamicKinematic(tmpD, tmpK, (this->gravity * (tmpD->gravitymultiplier)), this->gametype);
							tmpK = tmpK->sonraki;
						}
						tmpD2 = this->DynamicRectList;
						while (tmpD2 != nullptr) {
							CollideDynamicDynamic(tmpD, tmpD2);
							tmpD2 = tmpD2->sonraki;
						}
					}
				}
				if (this->gametype == ISOMETRIC) {
					tmpD->isonground = 0;
				}
				tmpD = tmpD->sonraki;
			}
			return;
		}
		// ############### Structure World End ###############
		// ############### Structure Static Begin ###############
		Static::Static(World * world, SDL_FRect * Position) {
			if (world == nullptr) return;
			this->world = world;
			this->position = Position;
			this->sonraki = world->StaticRectList;
			world->StaticRectList = this;
		}

		Static::~Static() {
			if (world->StaticRectList == this) {
				world->StaticRectList = world->StaticRectList->sonraki;
				free(this);
				return;
			}
			Rects::Static* tmp = world->StaticRectList;
			Rects::Static* tmp2 = nullptr;
			while (tmp != nullptr) {
				if (tmp == this) {
					tmp2->sonraki = tmp->sonraki;
					free(this);
					return;
				}
				tmp2 = tmp;
				tmp = tmp->sonraki;
			}
			return;
		}
		// ############### Structure Static End ###############
		// ############### Structure Kinematic Begin ###############
		Kinematic::Kinematic(World * world, SDL_FRect* Position) {
			if (world == nullptr) return;
			this->world = world;
			this->position = Position;
			this->sonraki = world->KinematicRectList;
			this->velocity.x = 0;
			this->velocity.y = 0;
			this->boolean = -1;
			world->KinematicRectList = this;
		}

		Kinematic::~Kinematic() {
			if (world->KinematicRectList == this) {
				world->KinematicRectList = world->KinematicRectList->sonraki;
				free(this);
				return;
			}
			Rects::Kinematic* tmp = world->KinematicRectList;
			Rects::Kinematic* tmp2 = nullptr;
			while (tmp != nullptr) {
				if (tmp == this) {
					tmp2->sonraki = tmp->sonraki;
					free(this);
					return;
				}
				tmp2 = tmp;
				tmp = tmp->sonraki;
			}
			return;
		}

		void Kinematic::StartAnimating(float X1, float Y1, float X2, float Y2, float Velocity) {
			Velocity = fabsf(Velocity);
			if (this == nullptr) return;
			else if (this->boolean == -2) return;
			else if (this->boolean == -1) {
				this->position->x = X1;
				this->position->y = Y1;
				this->boolean = 0;
			}
			else if (this->boolean == 0) {
				this->velocity.x = ((X2 - X1) / sqrtf((powf(X2 - X1, 2) + powf(Y2 - Y1, 2)))) * Velocity;
				this->velocity.y = ((Y2 - Y1) / sqrtf((powf(X2 - X1, 2) + powf(Y2 - Y1, 2)))) * Velocity;
				if (X1 > X2) {
					if (this->position->x <= X2) {
						this->position->x = X2;
						this->position->y = Y2;
						this->boolean = 1;
					}
				}
				else if (X1 < X2) {
					if (this->position->x >= X2) {
						this->position->x = X2;
						this->position->y = Y2;
						this->boolean = 1;
					}
				}
				else if (Y1 > Y2) {
					if (this->position->x <= Y2) {
						this->position->x = X2;
						this->position->y = Y2;
						this->boolean = 1;
					}
				}
				else if (Y1 < Y2) {
					if (this->position->x >= Y2) {
						this->position->x = X2;
						this->position->y = Y2;
						this->boolean = 1;
					}
				}
				else {
					this->velocity.x = 0;
					this->velocity.y = 0;
					this->boolean = -2;
				}
			}
			else if (this->boolean == 1) {
				this->velocity.x = ((X1 - X2) / sqrtf((powf(X2 - X1, 2) + powf(Y2 - Y1, 2)))) * Velocity;
				this->velocity.y = ((Y1 - Y2) / sqrtf((powf(X2 - X1, 2) + powf(Y2 - Y1, 2)))) * Velocity;
				if (X1 > X2) {
					if (this->position->x >= X1) {
						this->position->x = X1;
						this->position->y = Y1;
						this->boolean = 0;
					}
				}
				else if (X1 < X2) {
					if (this->position->x <= X1) {
						this->position->x = X1;
						this->position->y = Y1;
						this->boolean = 0;
					}
				}
				else if (Y1 > Y2) {
					if (this->position->x >= Y1) {
						this->position->x = X1;
						this->position->y = Y1;
						this->boolean = 0;
					}
				}
				else if (Y1 < Y2) {
					if (this->position->x <= Y1) {
						this->position->x = X1;
						this->position->y = Y1;
						this->boolean = 0;
					}
				}
				else {
					this->velocity.x = 0;
					this->velocity.y = 0;
					this->boolean = -2;
				}
			}
			return;
		}

		void Kinematic::StopAnimating() {
			if (this == nullptr) return;
			this->boolean = -1;
			this->velocity.x = 0;
			this->velocity.y = 0;
			return;
		}
		// ############### Structure Kinematic End ###############
		// ############### Structure Dynamic Begin ###############

		Dynamic::Dynamic(World * world, SDL_FRect* Position, float Mass, float FrictionCoefficient, float GravityMultiplier) {
			if (world == nullptr) return;
			this->world = world;
			this->position = Position;
			this->sonraki = world->DynamicRectList;
			this->mass = Mass;
			this->friction = FrictionCoefficient;
			this->velocity.x = 0;
			this->velocity.y = 0;
			this->force.x = 0;
			this->force.y = 0;
			this->gravitymultiplier = GravityMultiplier;
			this->xk = 0;
			this->yk = 0;
			this->alt = 0;
			this->sol = 0;
			this->sag = 0;
			this->ust = 0;
			world->DynamicRectList = this;
		}

		Dynamic::~Dynamic() {
			if (world->DynamicRectList == this) {
				world->DynamicRectList = world->DynamicRectList->sonraki;
				free(this);
				return;
			}
			Dynamic* tmp = world->DynamicRectList;
			Dynamic* tmp2 = nullptr;
			while (tmp != nullptr) {
				if (tmp == this) {
					tmp2->sonraki = tmp->sonraki;
					free(this);
					return;
				}
				tmp2 = tmp;
				tmp = tmp->sonraki;
			}
			return;
		}
		// ############### Structure Dynamic End ###############
	}
	// ############### Namespace Rects End ###############
	int IntersectRectLine(SDL_FRect* Rect, float* x1, float* y1, float* x2, float* y2) {
		a.x = *x1;
		a.y = *y1;
		b.x = *x2;
		b.y = *y2;
		c.x = Rect->x;
		c.y = Rect->y;
		d.x = Rect->x + Rect->w;
		d.y = Rect->y;
		if (doIntersect(a, b, c, d)) {
			return 1;
		}
		d.x = Rect->x;
		d.y = Rect->y + Rect->h;
		if (doIntersect(a, b, c, d)) {
			return 1;
		}
		c.x = Rect->x + Rect->w;
		c.y = Rect->y + Rect->h;
		if (doIntersect(a, b, c, d)) {
			return 1;
		}
		d.x = Rect->x + Rect->w;
		d.y = Rect->y;
		if (doIntersect(a, b, c, d)) {
			return 1;
		}
		if (Rect->x <= min(*x1, *x2) && (Rect->x + Rect->w) >= max(*x1, *x2) && Rect->y <= min(*y1, *y2) && (Rect->y + Rect->h) >= max(*y1, *y2)) {
			return 1;
		}
		return 0;
	}

	int onSegment(SDL_FPoint p, SDL_FPoint q, SDL_FPoint r) {

		if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) && q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y)) {
			return 1;
		}

		return 0;
	}

	int orientation(SDL_FPoint p, SDL_FPoint q, SDL_FPoint r) {

		if (((q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y)) == 0) {
			return 0;
		}

		return (((q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y)) > 0) ? 1 : 2;

	}

	int doIntersect(SDL_FPoint p1, SDL_FPoint q1, SDL_FPoint p2, SDL_FPoint q2) {

		if (orientation(p1, q1, p2) != orientation(p1, q1, q2) && orientation(p2, q2, p1) != orientation(p2, q2, q1)) { return 1; }

		if (orientation(p1, q1, p2) == 0 && onSegment(p1, p2, q1)) { return 1; }

		if (orientation(p1, q1, q2) == 0 && onSegment(p1, q2, q1)) { return 1; }

		if (orientation(p2, q2, p1) == 0 && onSegment(p2, p1, q2)) { return 1; }

		if (orientation(p2, q2, q1) == 0 && onSegment(p2, q1, q2)) { return 1; }

		return 0;
	}

	void CollideDynamicStatic(Rects::Dynamic* DynamicRect, Rects::Static* StaticRect) {
		if (SDL_HasIntersectionF(DynamicRect->position, StaticRect->position)) {
			*xmin = StaticRect->position->x;
			*ymin = StaticRect->position->y;
			*xmax = StaticRect->position->x + StaticRect->position->w;
			*ymax = StaticRect->position->y + StaticRect->position->h;
			if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->y = StaticRect->position->y - DynamicRect->position->h - 0.01f;
				if (DynamicRect->velocity.y > 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->isonground = 1;
				DynamicRect->alt = 1;
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->x = StaticRect->position->x + StaticRect->position->w + 0.01f;
				if (DynamicRect->velocity.x < 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sol = 1;
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->y = StaticRect->position->y + StaticRect->position->h + 0.01f;
				if (DynamicRect->velocity.y < 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->ust = 1;
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->x = StaticRect->position->x - DynamicRect->position->w - 0.01f;
				if (DynamicRect->velocity.x > 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sag = 1;
			}
			else if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				if ((StaticRect->position->x + StaticRect->position->w - DynamicRect->position->x) >= ((DynamicRect->position->y + DynamicRect->position->h - StaticRect->position->y))) {
					DynamicRect->position->y = StaticRect->position->y - DynamicRect->position->h - 0.01f;
					if (DynamicRect->velocity.y > 0) {
						DynamicRect->velocity.y = 0;
					}
					DynamicRect->isonground = 1;
					DynamicRect->alt = 1;
				}
				else {
					DynamicRect->position->x = StaticRect->position->x + StaticRect->position->w + 0.01f;
					if (DynamicRect->velocity.x < 0) {
						DynamicRect->velocity.x = 0;
					}
					DynamicRect->sol = 1;
				}
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				if ((StaticRect->position->x + StaticRect->position->w - DynamicRect->position->x) >= ((StaticRect->position->y + StaticRect->position->h - DynamicRect->position->y) + DynamicRect->velocity.y)) {
					DynamicRect->position->y = StaticRect->position->y + StaticRect->position->h + 0.01f;
					if (DynamicRect->velocity.y < 0) {
						DynamicRect->velocity.y = 0;
					}
					DynamicRect->ust = 1;
				}
				else {
					DynamicRect->position->x = StaticRect->position->x + StaticRect->position->w + 0.01f;
					if (DynamicRect->velocity.x < 0) {
						DynamicRect->velocity.x = 0;
					}
					DynamicRect->sol = 1;
				}
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				if ((DynamicRect->position->x + DynamicRect->position->w - StaticRect->position->x) >= ((StaticRect->position->y + StaticRect->position->h - DynamicRect->position->y) + DynamicRect->velocity.y)) {
					DynamicRect->position->y = StaticRect->position->y + StaticRect->position->h + 0.01f;
					if (DynamicRect->velocity.y < 0) {
						DynamicRect->velocity.y = 0;
					}
					DynamicRect->ust = 1;
				}
				else {
					DynamicRect->position->x = StaticRect->position->x - DynamicRect->position->w - 0.01f;
					if (DynamicRect->velocity.x > 0) {
						DynamicRect->velocity.x = 0;
					}
					DynamicRect->sag = 1;
				}
			}
			else if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				if ((DynamicRect->position->x + DynamicRect->position->w - StaticRect->position->x) >= ((DynamicRect->position->y + DynamicRect->position->h - StaticRect->position->y))) {
					DynamicRect->position->y = StaticRect->position->y - DynamicRect->position->h - 0.01f;
					if (DynamicRect->velocity.y > 0) {
						DynamicRect->velocity.y = 0;
					}
					DynamicRect->isonground = 1;
					DynamicRect->alt = 1;
				}
				else {
					DynamicRect->position->x = StaticRect->position->x - DynamicRect->position->w - 0.01f;
					if (DynamicRect->velocity.x > 0) {
						DynamicRect->velocity.x = 0;
					}
					DynamicRect->sag = 1;
				}
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->y = StaticRect->position->y + StaticRect->position->h + 0.01f;
				if (DynamicRect->velocity.y < 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->ust = 1;
			}
			else if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->x = StaticRect->position->x - DynamicRect->position->w - 0.01f;
				if (DynamicRect->velocity.x > 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sag = 1;
			}
			else if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->y = StaticRect->position->y - DynamicRect->position->h - 0.01f;
				if (DynamicRect->velocity.y > 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->isonground = 1;
				DynamicRect->alt = 1;
			}
			else if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->x = StaticRect->position->x + StaticRect->position->w + 0.01f;
				if (DynamicRect->velocity.x < 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sol = 1;
			}
		}
		return;
	}

	void CollideDynamicKinematic(Rects::Dynamic* DynamicRect, Rects::Kinematic* KinematicRect, float gravity, GAMETYPE gametype) {
		if (SDL_HasIntersectionF(DynamicRect->position, KinematicRect->position)) {
			*xmin = KinematicRect->position->x;
			*ymin = KinematicRect->position->y;
			*xmax = KinematicRect->position->x + KinematicRect->position->w;
			*ymax = KinematicRect->position->y + KinematicRect->position->h;
			if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->y = KinematicRect->position->y - DynamicRect->position->h - 0.01f;
				if (DynamicRect->velocity.y > 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->alt = 1;
				if (gametype == SIDESCROLLER) {
					DynamicRect->isonground = 1;
					DynamicRect->position->y += 0.02f;
					if (KinematicRect->velocity.y > 0 && KinematicRect->velocity.y <= gravity) {
						DynamicRect->yk = KinematicRect->velocity.y;
					}
					DynamicRect->xk = KinematicRect->velocity.x;
				}
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->x = KinematicRect->position->x + KinematicRect->position->w + 0.01f;
				if (DynamicRect->velocity.x < 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sol = 1;
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->y = KinematicRect->position->y + KinematicRect->position->h + 0.01f;
				if (DynamicRect->velocity.y < 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->ust = 1;
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->x = KinematicRect->position->x - DynamicRect->position->w - 0.01f;
				if (DynamicRect->velocity.x > 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sag = 1;
			}
			else if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				if ((KinematicRect->position->x + KinematicRect->position->w - DynamicRect->position->x) >= ((DynamicRect->position->y + DynamicRect->position->h - KinematicRect->position->y))) {
					DynamicRect->position->y = KinematicRect->position->y - DynamicRect->position->h - 0.01f;
					if (DynamicRect->velocity.y > 0) {
						DynamicRect->velocity.y = 0;
					}
					DynamicRect->alt = 1;
					if (gametype == SIDESCROLLER) {
						DynamicRect->isonground = 1;
						DynamicRect->position->y += 0.02f;
						if (KinematicRect->velocity.y > 0 && KinematicRect->velocity.y <= gravity) {
							DynamicRect->yk = KinematicRect->velocity.y;
						}
						DynamicRect->xk = KinematicRect->velocity.x;
					}
				}
				else {
					DynamicRect->position->x = KinematicRect->position->x + KinematicRect->position->w + 0.01f;
					if (DynamicRect->velocity.x < 0) {
						DynamicRect->velocity.x = 0;
					}
					DynamicRect->sol = 1;
				}
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				if ((KinematicRect->position->x + KinematicRect->position->w - DynamicRect->position->x) >= ((KinematicRect->position->y + KinematicRect->position->h - DynamicRect->position->y) + DynamicRect->velocity.y)) {
					DynamicRect->position->y = KinematicRect->position->y + KinematicRect->position->h + 0.01f;
					if (DynamicRect->velocity.y < 0) {
						DynamicRect->velocity.y = 0;
					}
					DynamicRect->ust = 1;
				}
				else {
					DynamicRect->position->x = KinematicRect->position->x + KinematicRect->position->w + 0.01f;
					if (DynamicRect->velocity.x < 0) {
						DynamicRect->velocity.x = 0;
					}
					DynamicRect->sol = 1;
				}
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				if ((DynamicRect->position->x + DynamicRect->position->w - KinematicRect->position->x) >= ((KinematicRect->position->y + KinematicRect->position->h - DynamicRect->position->y) + DynamicRect->velocity.y)) {
					DynamicRect->position->y = KinematicRect->position->y + KinematicRect->position->h + 0.01f;
					if (DynamicRect->velocity.y < 0) {
						DynamicRect->velocity.y = 0;
					}
					DynamicRect->ust = 1;
				}
				else {
					DynamicRect->position->x = KinematicRect->position->x - DynamicRect->position->w - 0.01f;
					if (DynamicRect->velocity.x > 0) {
						DynamicRect->velocity.x = 0;
					}
					DynamicRect->sag = 1;
				}
			}
			else if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				if ((DynamicRect->position->x + DynamicRect->position->w - KinematicRect->position->x) >= ((DynamicRect->position->y + DynamicRect->position->h - KinematicRect->position->y))) {
					DynamicRect->position->y = KinematicRect->position->y - DynamicRect->position->h - 0.01f;
					if (DynamicRect->velocity.y > 0) {
						DynamicRect->velocity.y = 0;
					}
					DynamicRect->alt = 1;
					if (gametype == SIDESCROLLER) {
						DynamicRect->isonground = 1;
						DynamicRect->position->y += 0.02f;
						if (KinematicRect->velocity.y > 0 && KinematicRect->velocity.y <= gravity) {
							DynamicRect->yk = KinematicRect->velocity.y;
						}
						DynamicRect->xk = KinematicRect->velocity.x;
					}
				}
				else {
					DynamicRect->position->x = KinematicRect->position->x - DynamicRect->position->w - 0.01f;
					if (DynamicRect->velocity.x > 0) {
						DynamicRect->velocity.x = 0;
					}
					DynamicRect->sag = 1;
				}
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->y = KinematicRect->position->y + KinematicRect->position->h + 0.01f;
				if (DynamicRect->velocity.y < 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->ust = 1;
			}
			else if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->x = KinematicRect->position->x - DynamicRect->position->w - 0.01f;
				if (DynamicRect->velocity.x > 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sag = 1;
			}
			else if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->y = KinematicRect->position->y - DynamicRect->position->h - 0.01f;
				if (DynamicRect->velocity.y > 0) {
					DynamicRect->velocity.y = 0;
				}
				DynamicRect->alt = 1;
				if (gametype == SIDESCROLLER) {
					DynamicRect->isonground = 1;
					DynamicRect->position->y += 0.02f;
					if (KinematicRect->velocity.y > 0 && KinematicRect->velocity.y <= gravity) {
						DynamicRect->yk = KinematicRect->velocity.y;
					}
					DynamicRect->xk = KinematicRect->velocity.x;
				}
			}
			else if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->x = KinematicRect->position->x + KinematicRect->position->w + 0.01f;
				if (DynamicRect->velocity.x < 0) {
					DynamicRect->velocity.x = 0;
				}
				DynamicRect->sol = 1;
			}
		}
		return;
	}

	void CollideDynamicDynamic(Rects::Dynamic* DynamicRect, Rects::Dynamic* DynamicRect2) {
		if (DynamicRect != DynamicRect2 && SDL_HasIntersectionF(DynamicRect->position, DynamicRect2->position)) {
			*xmin = DynamicRect2->position->x;
			*ymin = DynamicRect2->position->y;
			*xmax = DynamicRect2->position->x + DynamicRect2->position->w;
			*ymax = DynamicRect2->position->y + DynamicRect2->position->h;
			if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->y = DynamicRect2->position->y - DynamicRect->position->h;
				if (DynamicRect2->alt == 1) {
					if (DynamicRect->velocity.y > 0) {
						DynamicRect->velocity.y = 0;
					}
					DynamicRect->isonground = 1;
					DynamicRect->alt = 1;
				}
				else if (DynamicRect->velocity.y > 0) {
					DynamicRect->velocity.y = ((DynamicRect->velocity.y * DynamicRect->mass) + (DynamicRect2->velocity.y * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
					DynamicRect2->velocity.y = DynamicRect->velocity.y;
				}
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->x = DynamicRect2->position->x + DynamicRect2->position->w;
				if (DynamicRect2->sol == 1) {
					if (DynamicRect->velocity.x < 0) {
						DynamicRect->velocity.x = 0;
					}
					DynamicRect->sol = 1;
				}
				else if (DynamicRect->velocity.x < 0) {
					DynamicRect->velocity.x = ((DynamicRect->velocity.x * DynamicRect->mass) + (DynamicRect2->velocity.x * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
					DynamicRect2->velocity.x = DynamicRect->velocity.x;
				}
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->y = DynamicRect2->position->y + DynamicRect2->position->h;
				if (DynamicRect2->ust == 1) {
					if (DynamicRect->velocity.y < 0) {
						DynamicRect->velocity.y = 0;
					}
					DynamicRect->ust = 1;
				}
				else if (DynamicRect->velocity.y < 0) {
					DynamicRect->velocity.y = ((DynamicRect->velocity.y * DynamicRect->mass) + (DynamicRect2->velocity.y * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
					DynamicRect2->velocity.y = DynamicRect->velocity.y;
				}
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->x = DynamicRect2->position->x - DynamicRect->position->w;
				if (DynamicRect2->sag == 1) {
					if (DynamicRect->velocity.x > 0) {
						DynamicRect->velocity.x = 0;
					}
					DynamicRect->sag = 1;
				}
				else if (DynamicRect->velocity.x > 0) {
					DynamicRect->velocity.x = ((DynamicRect->velocity.x * DynamicRect->mass) + (DynamicRect2->velocity.x * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
					DynamicRect2->velocity.x = DynamicRect->velocity.x;
				}
			}
			else if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				if ((DynamicRect2->position->x + DynamicRect2->position->w - DynamicRect->position->x) >= ((DynamicRect->position->y + DynamicRect->position->h - DynamicRect2->position->y))) {
					DynamicRect->position->y = DynamicRect2->position->y - DynamicRect->position->h;
					if (DynamicRect2->alt == 1) {
						if (DynamicRect->velocity.y > 0) {
							DynamicRect->velocity.y = 0;
						}
						DynamicRect->isonground = 1;
						DynamicRect->alt = 1;
					}
					else if (DynamicRect->velocity.y > 0) {
						DynamicRect->velocity.y = ((DynamicRect->velocity.y * DynamicRect->mass) + (DynamicRect2->velocity.y * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
						DynamicRect2->velocity.y = DynamicRect->velocity.y;
					}
				}
				else {
					DynamicRect->position->x = DynamicRect2->position->x + DynamicRect2->position->w;
					if (DynamicRect2->sol == 1) {
						if (DynamicRect->velocity.x < 0) {
							DynamicRect->velocity.x = 0;
						}
						DynamicRect->sol = 1;
					}
					else if (DynamicRect->velocity.x < 0) {
						DynamicRect->velocity.x = ((DynamicRect->velocity.x * DynamicRect->mass) + (DynamicRect2->velocity.x * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
						DynamicRect2->velocity.x = DynamicRect->velocity.x;
					}
				}
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				if ((DynamicRect2->position->x + DynamicRect2->position->w - DynamicRect->position->x) >= ((DynamicRect2->position->y + DynamicRect2->position->h - DynamicRect->position->y) + DynamicRect->velocity.y)) {
					DynamicRect->position->y = DynamicRect2->position->y + DynamicRect2->position->h;
					if (DynamicRect2->ust == 1) {
						if (DynamicRect->velocity.y < 0) {
							DynamicRect->velocity.y = 0;
						}
						DynamicRect->ust = 1;
					}
					else if (DynamicRect->velocity.y < 0) {
						DynamicRect->velocity.y = ((DynamicRect->velocity.y * DynamicRect->mass) + (DynamicRect2->velocity.y * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
						DynamicRect2->velocity.y = DynamicRect->velocity.y;
					}
				}
				else {
					DynamicRect->position->x = DynamicRect2->position->x + DynamicRect2->position->w;
					if (DynamicRect2->sol == 1) {
						if (DynamicRect->velocity.x < 0) {
							DynamicRect->velocity.x = 0;
						}
						DynamicRect->sol = 1;
					}
					else if (DynamicRect->velocity.x < 0) {
						DynamicRect->velocity.x = ((DynamicRect->velocity.x * DynamicRect->mass) + (DynamicRect2->velocity.x * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
						DynamicRect2->velocity.x = DynamicRect->velocity.x;
					}
				}
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				if ((DynamicRect->position->x + DynamicRect->position->w - DynamicRect2->position->x) >= ((DynamicRect2->position->y + DynamicRect2->position->h - DynamicRect->position->y) + DynamicRect->velocity.y)) {
					DynamicRect->position->y = DynamicRect2->position->y + DynamicRect2->position->h;
					if (DynamicRect2->ust == 1) {
						if (DynamicRect->velocity.y < 0) {
							DynamicRect->velocity.y = 0;
						}
						DynamicRect->ust = 1;
					}
					else if (DynamicRect->velocity.y < 0) {
						DynamicRect->velocity.y = ((DynamicRect->velocity.y * DynamicRect->mass) + (DynamicRect2->velocity.y * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
						DynamicRect2->velocity.y = DynamicRect->velocity.y;
					}
				}
				else {
					DynamicRect->position->x = DynamicRect2->position->x - DynamicRect->position->w;
					if (DynamicRect2->sag == 1) {
						if (DynamicRect->velocity.x > 0) {
							DynamicRect->velocity.x = 0;
						}
						DynamicRect->sag = 1;
					}
					else if (DynamicRect->velocity.x > 0) {
						DynamicRect->velocity.x = ((DynamicRect->velocity.x * DynamicRect->mass) + (DynamicRect2->velocity.x * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
						DynamicRect2->velocity.x = DynamicRect->velocity.x;
					}
				}
			}
			else if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				if ((DynamicRect->position->x + DynamicRect->position->w - DynamicRect2->position->x) >= ((DynamicRect->position->y + DynamicRect->position->h - DynamicRect2->position->y))) {
					DynamicRect->position->y = DynamicRect2->position->y - DynamicRect->position->h;
					if (DynamicRect2->alt == 1) {
						if (DynamicRect->velocity.y > 0) {
							DynamicRect->velocity.y = 0;
						}
						DynamicRect->isonground = 1;
						DynamicRect->alt = 1;
					}
					else if (DynamicRect->velocity.y > 0) {
						DynamicRect->velocity.y = ((DynamicRect->velocity.y * DynamicRect->mass) + (DynamicRect2->velocity.y * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
						DynamicRect2->velocity.y = DynamicRect->velocity.y;
					}
				}
				else {
					DynamicRect->position->x = DynamicRect2->position->x - DynamicRect->position->w;
					if (DynamicRect2->sag == 1) {
						if (DynamicRect->velocity.x > 0) {
							DynamicRect->velocity.x = 0;
						}
						DynamicRect->sag = 1;
					}
					else if (DynamicRect->velocity.x > 0) {
						DynamicRect->velocity.x = ((DynamicRect->velocity.x * DynamicRect->mass) + (DynamicRect2->velocity.x * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
						DynamicRect2->velocity.x = DynamicRect->velocity.x;
					}
				}
			}
			else if (!IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->y = DynamicRect2->position->y + DynamicRect2->position->h;
				if (DynamicRect2->ust == 1) {
					if (DynamicRect->velocity.y < 0) {
						DynamicRect->velocity.y = 0;
					}
					DynamicRect->ust = 1;
				}
				else if (DynamicRect->velocity.y < 0) {
					DynamicRect->velocity.y = ((DynamicRect->velocity.y * DynamicRect->mass) + (DynamicRect2->velocity.y * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
					DynamicRect2->velocity.y = DynamicRect->velocity.y;
				}
			}
			else if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && !IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->x = DynamicRect2->position->x - DynamicRect->position->w;
				if (DynamicRect2->sag == 1) {
					if (DynamicRect->velocity.x > 0) {
						DynamicRect->velocity.x = 0;
					}
					DynamicRect->sag = 1;
				}
				else if (DynamicRect->velocity.x > 0) {
					DynamicRect->velocity.x = ((DynamicRect->velocity.x * DynamicRect->mass) + (DynamicRect2->velocity.x * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
					DynamicRect2->velocity.x = DynamicRect->velocity.x;
				}
			}
			else if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && !IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->y = DynamicRect2->position->y - DynamicRect->position->h;
				if (DynamicRect2->alt == 1) {
					if (DynamicRect->velocity.y > 0) {
						DynamicRect->velocity.y = 0;
					}
					DynamicRect->isonground = 1;
					DynamicRect->alt = 1;
				}
				else if (DynamicRect->velocity.y > 0) {
					DynamicRect->velocity.y = ((DynamicRect->velocity.y * DynamicRect->mass) + (DynamicRect2->velocity.y * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
					DynamicRect2->velocity.y = DynamicRect->velocity.y;
				}
			}
			else if (IntersectRectLine(DynamicRect->position, xmin, ymin, xmax, ymin) && IntersectRectLine(DynamicRect->position, xmax, ymin, xmax, ymax) && IntersectRectLine(DynamicRect->position, xmax, ymax, xmin, ymax) && !IntersectRectLine(DynamicRect->position, xmin, ymax, xmin, ymin)) {
				DynamicRect->position->x = DynamicRect2->position->x + DynamicRect2->position->w;
				if (DynamicRect2->sol == 1) {
					if (DynamicRect->velocity.x < 0) {
						DynamicRect->velocity.x = 0;
					}
					DynamicRect->sol = 1;
				}
				else if (DynamicRect->velocity.x < 0) {
					DynamicRect->velocity.x = ((DynamicRect->velocity.x * DynamicRect->mass) + (DynamicRect2->velocity.x * DynamicRect2->mass)) / (DynamicRect->mass + DynamicRect2->mass);
					DynamicRect2->velocity.x = DynamicRect->velocity.x;
				}
			}
		}
		return;
	}
}
#endif
// ############### Namespace Physics End ###############
// ############### Namespace AudioManager Begin ###############
#ifdef AST_AUDIO
namespace AudioManager {
	std::unordered_map<std::string, Mix_Chunk*> Chunks;
	std::unordered_map<std::string, Mix_Music*> Musics;

	void Play(std::string fileName, bool chunk) {
		bool found = false;

		if(chunk) {
			if(!Chunks[fileName]) Chunks[fileName] = Mix_LoadWAV(("Resources/SFX/" + fileName).c_str());
			Mix_PlayChannel(-1, Chunks[fileName], 0);
		} else {
			if(!Musics[fileName]) Musics[fileName] = Mix_LoadMUS(("Resources/SFX/" + fileName).c_str());
			Mix_PlayMusic(Musics[fileName], 0);
		}
	}
	void Free() {
		for(auto& Chunk : Chunks) Mix_FreeChunk(Chunk.second);
		for(auto& Music : Musics) Mix_FreeMusic(Music.second);
	}
}
#endif
// ############### Namespace AudioManager End ###############