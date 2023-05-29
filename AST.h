#pragma once

#define SCENE(param) \
    (std::string(__FILE__).substr(std::string(__FILE__).find_last_of("/\\") + 1, \
    std::string(__FILE__).find_last_of(".") - std::string(__FILE__).find_last_of("/\\") - 1) \
    + "/" + (param))

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace AST {

	extern SDL_Window* win;
	extern SDL_Renderer* ren;
	extern SDL_Point Mouse;
	extern int grid;
	extern bool loop;
	extern bool isFullscreen;
	extern std::unordered_map<int, bool> keys;
	extern std::string instruction;

	void Init(std::string title); 

	void HandleEv(SDL_Event& event);

	class Scene {
		public:
			Scene();
			virtual void loop();
			virtual void event(SDL_Event& event);
			~Scene();
	};

	void Render(Scene& scene);
	void Quit();

	struct Rect : public SDL_Rect {
		std::string texture;
		SDL_Color start;
		SDL_Color end;
		double angle;
		std::string type;

		Rect(SDL_Rect rect, std::string texture);
		Rect(SDL_Rect rect, SDL_Color color);
		Rect(SDL_Rect rect, SDL_Color start, SDL_Color end);
	private:
		void init(SDL_Rect rect);
	};

	bool inRange(int num, int min, int max);
	bool hovering(SDL_Rect rect);
	void fullscreen(bool yes);
	void setTimeout(std::function<void()> function, int ms);
} // namespace AST

namespace SpriteManager {
	extern std::vector<std::pair<SDL_Texture*, std::string>> sprites;
	extern bool debug;

	bool load(std::string keyword, std::string file);
	bool load(std::string keyword, std::string sheetFile, SDL_Rect spriteRect);

	bool drawTRect(AST::Rect rect);
	void drawCRect(AST::Rect rect);
	void drawGRect(AST::Rect rect);
	bool draw(AST::Rect rect);
	
	void free();
} // namespace SpriteManager
