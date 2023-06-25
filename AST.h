#pragma once

#include <SDL.h>

#ifdef AST_TEXT
	#include <SDL_ttf.h>
#ifdef AST_TEXTURE
	#include <SDL_image.h>
#endif
#endif
#ifdef AST_AUDIO
	#include <SDL_mixer.h>
#endif
#ifdef AST_NET
	#include <SDL_net.h>
#endif

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <algorithm>

using std::max;
using std::min;

#ifdef AST_PHYSICS
namespace Physics {
	namespace Rects {
		struct Static;
		struct Kinematic;
		struct Dynamic;
	}
}
#endif
// ############### Namespace AST Begin ###############
namespace AST {
// ############### Variables Begin ###############
	extern SDL_Window* win;
	extern SDL_Renderer* ren;
	extern SDL_Point Mouse;
	extern int FPS;
	extern bool loop;
	extern bool isFullscreen;
	extern std::unordered_map<int, bool> events;
	extern std::unordered_map<int, bool> keys;
	extern int code;
	extern std::function<void(SDL_Event&)> EventHandler;
// ############### Variables End ###############
// ############### Utility Functions Begin ###############
	SDL_Color RandomColor();
	bool InRange(int num, int min, int max);
	bool Hovering(SDL_FRect);
	bool Clicked(SDL_FRect, int key = SDL_BUTTON_LEFT);
	void Fullscreen(bool = !AST::isFullscreen);
	void SetTimeout(std::function<void()> function, int ms);
// ############### Utility Functions End ###############
// ############### Class Scene Begin ###############
	class Scene {
	public:
		Scene();
		virtual void loop();
		virtual void event(SDL_Event& event);
		~Scene();
	};
// ############### Class Scene End ###############
// ############### Class Text Begin ###############
#ifdef AST_TEXT
	class Text : public SDL_Rect {
		SDL_Texture * texture;
		public:
			double angle;

			Text();
			~Text();

			void set(TTF_Font*, std::string text, SDL_Color = AST::RandomColor());
			void draw();
			void free();
	};
#endif
// ############### Class Text End ###############
// ############### Class Rect Begin ###############
	class Rect : public SDL_FRect {
		void Init(SDL_FRect winrect);
	public:
		AST::Rect * layer = nullptr;
#ifdef AST_TEXT
		AST::Text * text = nullptr;
#endif
		std::string texture;
		bool filled;
		SDL_Color color;
		double angle;

#ifdef AST_PHYSICS
		Physics::Rects::Static * StaticRect;
		Physics::Rects::Kinematic * KinematicRect;
		Physics::Rects::Dynamic * DynamicRect;
#endif

		void initlayer(SDL_Color = AST::RandomColor());
		Rect(SDL_FRect rect, std::string texture);
		Rect(SDL_FRect rect, SDL_Color color = AST::RandomColor());
		~Rect();
	};
// ############### Class Rect End ###############
// ############### Basic Functions Begin ###############
	void Init(std::string title, SDL_Rect rect = {
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, -1, -1
	});
	void Render(Scene & scene);
	void Quit();
// ############### Basic Functions End ###############
} // namespace AST
// ############### Namespace AST End ###############
// ############### Namespace SpriteManager Begin ###############
namespace SpriteManager {
#ifdef AST_TEXTURE
	extern std::unordered_map<std::string, SDL_Texture*> Sprites;
	
	bool Load(std::string keyword, std::string file);
	bool Load(std::string keyword, std::string sheetFile, SDL_Rect spriteRect);

	void Free();
#endif
	bool Render(AST::Rect rect);
} // namespace SpriteManager
// ############### Namespace Physics Begin ###############
#ifdef AST_PHYSICS
namespace Physics {
	// ############### Variables Begin ###############
	extern float* xmax, * xmin, * ymax, * ymin;

	extern float Xmax, Ymax;

	extern float xmax2, ymax2, xmin2;

	extern int loop, i;

	extern SDL_FPoint a, b, c, d;

	// ############### Variables End ###############
	enum GAMETYPE {
		SIDESCROLLER, ISOMETRIC
	};
	// ############### Namespace Rects Begin ###############
	namespace Rects {

		struct Static;
		struct Kinematic;
		struct Dynamic;

		// ############### Structure World Begin ###############
		struct World {
			float gravity;
			float airfriction;
			GAMETYPE gametype;
			Rects::Static* StaticRectList;
			Rects::Kinematic* KinematicRectList;
			Rects::Dynamic* DynamicRectList;

			World(float AirFrictionCoefficient = 0.0, float Gravity = 0.0);
			~World();

			int GetStaticRectNumber();
			int GetKinematicRectNumber();
			int GetDynamicRectNumber();

			void Simulate(float ms);
		};
		// ############### Structure World End ###############
		// ############### Structure Static Begin ###############
		struct Static {
			SDL_FRect* position;
			Rects::Static* sonraki;
			World * world;

			Static(World * world, SDL_FRect * Position); //static rects aren't affected physically by other rectangles and gravity, they dont have velocity, you can use them to make stationary platforms
			~Static(); //frees the memory which is retained by the static rect and deletes it from its world
		};
		// ############### Sturcture Static End ###############
		// ############### Structure Kinematic Begin ###############
		struct Kinematic {
			SDL_FRect* position;
			SDL_FPoint velocity;
			Rects::Kinematic* sonraki;
			int boolean;
			World * world;

			Kinematic(World * world, SDL_FRect * Position); //kinematic rects aren't affected physically by other rectangles and gravity, they do have velocity, you can use them to make moving platforms
			~Kinematic(); //frees the memory which is retained by the kinematic rect and deletes it from its world

			void StartAnimating(float X1, float Y1, float X2, float Y2, float Velocity);//you can use this function to make a moving platform easily, also you must put this function in your game loop like SKR_SimulateWorld function
			void StopAnimating();//you should use this function to stop the kinematic rect after it is animated by SKR_AnimateKinematicRect function, also if you want to apply another animation on same kinematic rect, you must use this function between SKR_AnimateKinematicRect functions 
		};
		// ############### Structure Kinematic Begin ###############
		// ############### Structure Kinematic End ###############
		struct Dynamic {
			SDL_FRect* position;
			float mass;
			SDL_FPoint velocity;
			float xk;
			float yk;
			SDL_FPoint force;
			Rects::Dynamic* sonraki;
			int isonground;
			float friction;
			int alt, ust, sol, sag;
			float gravitymultiplier;
			World * world;

			Dynamic(World * world, SDL_FRect* Position, float Mass, float FrictionCoefficient, float GravityMultiplier);//dynamic rects are affected physically by other rectangles and gravity, you can set their velocities or apply forces
			~Dynamic();//frees the memory which is retained by the dynamic rect and deletes it from its world
		};
		// ############### Structure Dynamic End ###############
	}
	// ############### Namespace Rects End ###############

	int IntersectRectLine(SDL_FRect* Rect, float* x1, float* y1, float* x2, float* y2); //returns true if the rect and the line are intersected, returns false otherwise
	// ############### Functions ThatYouDontNeed Begin ###############
	int onSegment(SDL_FPoint p, SDL_FPoint q, SDL_FPoint r);
	int orientation(SDL_FPoint p, SDL_FPoint q, SDL_FPoint r);
	int doIntersect(SDL_FPoint p1, SDL_FPoint q1, SDL_FPoint p2, SDL_FPoint q2);
	void CollideDynamicStatic(Rects::Dynamic* DynamicRect, Rects::Static* StaticRect);
	void CollideDynamicKinematic(Rects::Dynamic* DynamicRect, Rects::Kinematic* KinematicRect, float gravity, GAMETYPE gametype);
	void CollideDynamicDynamic(Rects::Dynamic* DynamicRect, Rects::Dynamic* DynamicRect2);
	// ############### Functions ThatYouDontNeed End ###############
	extern Physics::Rects::Dynamic* tmpD;
	extern Physics::Rects::Dynamic* tmpD2;
	extern Physics::Rects::Static* tmpS;
	extern Physics::Rects::Kinematic* tmpK;
}

#endif
// ############### Namespace Physics End ############### 
// ############### Namespace AudioManager Begin ###############
#ifdef AST_AUDIO
namespace AudioManager {
	extern std::unordered_map<std::string, Mix_Chunk*> Chunks;
	extern std::unordered_map<std::string, Mix_Music*> Musics;
	void Play(std::string file, bool chunk = true);
	void Free();
}
#endif
// ############### Namespace AudioManager End ###############