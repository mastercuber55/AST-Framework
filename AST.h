#pragma once

#include <SDL.h>
#ifdef AST_TEXTURE
	#include <SDL_image.h>
#endif
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <algorithm>

using std::max;
using std::min;

#ifdef AST_PHYSICS
// ############### SKR_Physics.h Start ############### 
typedef struct SKR_StaticRect SKR_StaticRect;

typedef struct SKR_KinematicRect SKR_KinematicRect;

typedef struct SKR_DynamicRect SKR_DynamicRect;

typedef struct SKR_RectWorld SKR_RectWorld;

typedef enum { SKR_SIDESCROLLER, SKR_ISOMETRIC }SKR_GAMETYPE;


int SKR_IntersectRectLine(SDL_FRect* Rect, float* x1, float* y1, float* x2, float* y2);//returns 1 if the rect and the line are intersected, returns 0 otherwise

int SKR_IntersectRectRect(SDL_FRect* Rect1, SDL_FRect* Rect2);//returns 1 if the rects are intersected, returns 0 otherwise


SKR_RectWorld* SKR_CreateRectWorld(float Gravity, float AirFrictionCoefficient, SKR_GAMETYPE GameType);//creates a SKR world to hold static kinematic and dynamic rects' data//you must define your gametype here SKR_SIDESCROLLER or SKR_ISOMETRIC, when you set this SKR_ISOMETRIC gravity will be 0 regardless of your input

void SKR_SetGravity(SKR_RectWorld* World, float Gravity);

float SKR_GetGravity(SKR_RectWorld* World);

void SKR_SetAirFriction(SKR_RectWorld* World, float AirFrictionCoefficient);

float SKR_GetAirFriction(SKR_RectWorld* World);

void SKR_DestroyRectWorld(SKR_RectWorld* World);//this frees all the memory which is retained by the world and all the rects which are created in it


SKR_StaticRect* SKR_CreateStaticRect(SKR_RectWorld* World, SDL_FRect* Position);//static rects aren't affected physically by other rectangles and gravity, they dont have velocity, you can use them to make stationary platforms

void SKR_DestroyStaticRect(SKR_RectWorld* World, SKR_StaticRect* StaticRect);//frees the memory which is retained by the static rect and deletes it from its world

SDL_FRect* SKR_GetPositionStaticRect(SKR_StaticRect* StaticRect);

int SKR_GetStaticRectNumber(SKR_RectWorld* World);//returns how many static rects there are in the world


SKR_KinematicRect* SKR_CreateKinematicRect(SKR_RectWorld* World, SDL_FRect* Position);//kinematic rects aren't affected physically by other rectangles and gravity, they do have velocity, you can use them to make moving platforms

void SKR_DestroyKinematicRect(SKR_RectWorld* World, SKR_KinematicRect* KinematicRect);//frees the memory which is retained by the kinematic rect and deletes it from its world

SDL_FRect* SKR_GetPositionKinematicRect(SKR_KinematicRect* KinematicRect);

int SKR_GetKinematicRectNumber(SKR_RectWorld* World);//returns how many kinematic rects there are in the world

void SKR_SetXVelocityKinematicRect(SKR_KinematicRect* KinematicRect, float Xspeed);

void SKR_SetYVelocityKinematicRect(SKR_KinematicRect* KinematicRect, float Yspeed);

float SKR_GetXVelocityKinematicRect(SKR_KinematicRect* KinematicRect);

float SKR_GetYVelocityKinematicRect(SKR_KinematicRect* KinematicRect);

void SKR_AnimateKinematicRect(SKR_KinematicRect* KinematicRect, float X1, float Y1, float X2, float Y2, float Velocity);//you can use this function to make a moving platform easily, also you must put this function in your game loop like SKR_SimulateWorld function

void SKR_StopAnimatingKinematicRect(SKR_KinematicRect* KinematicRect);//you should use this function to stop the kinematic rect after it is animated by SKR_AnimateKinematicRect function, also if you want to apply another animation on same kinematic rect, you must use this function between SKR_AnimateKinematicRect functions 


SKR_DynamicRect* SKR_CreateDynamicRect(SKR_RectWorld* World, SDL_FRect* Position, float Mass, float FrictionCoefficient, float GravityMultiplier);//dynamic rects are affected physically by other rectangles and gravity, you can set their velocities or apply forces

float SKR_GetFriction(SKR_DynamicRect* DynamicRect);

void SKR_SetFriction(SKR_DynamicRect* DynamicRect, float FrictionCoefficient);

float SKR_GetGravityMultiplier(SKR_DynamicRect* DynamicRect);

void SKR_SetGravityMultiplier(SKR_DynamicRect* DynamicRect, float GravityMultiplier);

void SKR_DestroyDynamicRect(SKR_RectWorld* World, SKR_DynamicRect* DynamicRect);//frees the memory which is retained by the dynamic rect and deletes it from its world

SDL_FRect* SKR_GetPositionDynamicRect(SKR_DynamicRect* DynamicRect);

int SKR_GetDynamicRectNumber(SKR_RectWorld* World);//returns how many dynamic rects there are in the world

void SKR_SetXVelocityDynamicRect(SKR_DynamicRect* DynamicRect, float Xspeed);

void SKR_SetYVelocityDynamicRect(SKR_DynamicRect* DynamicRect, float Yspeed);

float SKR_GetXVelocityDynamicRect(SKR_DynamicRect* DynamicRect);

float SKR_GetYVelocityDynamicRect(SKR_DynamicRect* DynamicRect);

float SKR_GetMassDynamicRect(SKR_DynamicRect* DynamicRect);

void SKR_SetMassDynamicRect(SKR_DynamicRect* DynamicRect, float Mass);

void SKR_ApplyForceX(SKR_DynamicRect* DynamicRect, float Force);//applying force means you are adding force to the forces which are already applying

void SKR_ApplyForceY(SKR_DynamicRect* DynamicRect, float Force);

void SKR_SetForceX(SKR_DynamicRect* DynamicRect, float Force);//setting force means, you set the force this number regardless of previously applied forces

void SKR_SetForceY(SKR_DynamicRect* DynamicRect, float Force);

float SKR_GetForceX(SKR_DynamicRect* DynamicRect);

float SKR_GetForceY(SKR_DynamicRect* DynamicRect);

int SKR_IsOnground(SKR_DynamicRect* DynamicRect);//this function returns 1 if the dynamic rect is on something, returns 0 otherwise. this function can be helpful in situations like deciding if a character can jump in your side scroller game, will return 0 always in an isometric game


void SKR_SimulateWorld(SKR_RectWorld* World, float Milliseconds);//simulates the world for some time, you must put this in your main game loop. for example your one frame/cycle takes 15 milliseconds, so you should write it like SKR_SimulateWorld(world, 15);

#endif
// ############### SKR_Physics.h End ############### 

namespace AST {

	extern SDL_Window* win;
	extern SDL_Renderer* ren;
	extern SDL_Point Mouse;
	extern int grid;
	extern int FPS;
	extern bool loop;
	extern bool isFullscreen;
	extern std::unordered_map<int, bool> keys;
	extern int code;

#ifdef AST_PHYSICS
    extern SKR_RectWorld * world;
#endif


	class Scene {
		public:
			Scene();
			virtual void loop();
			virtual void event(SDL_Event& event);
			~Scene();
	};

#ifdef AST_PHYSICS
	void Init(std::string title, SDL_Rect rect, float WorldGravity, float WorldAirFrictionCoefficient, SKR_GAMETYPE WorldGameType);
#else
	void Init(std::string title, SDL_Rect rect);
#endif
	void Render(Scene & scene);
	void Quit();

	struct Rect : public SDL_FRect {
		std::string texture;
		SDL_Color start;
		SDL_Color end;
		int type;
#ifndef AST_PHYSICS
		double angle;
#else
		SKR_StaticRect * StaticRect;
		SKR_DynamicRect * DynamicRect;
		void Physics(float mass = 0.0, float friction = 0.0, float gravity = 0.0);
#endif
#ifdef AST_TEXTURE
		Rect(SDL_FRect rect, std::string texture);
#endif
		Rect(SDL_FRect rect, SDL_Color color);
		Rect(SDL_FRect rect, SDL_Color start, SDL_Color end);
		~Rect();

		private: void Init(SDL_FRect rect);
	};

	bool inRange(int num, int min, int max);
	bool hovering(SDL_FRect rect);
	void fullscreen(bool yes);
	void setTimeout(std::function<void()> function, int ms);
} // namespace AST

namespace SpriteManager {
#ifdef AST_TEXTURE
	extern std::vector<std::pair<SDL_Texture*, std::string>> sprites;
	
	bool load(std::string keyword, std::string file);
	bool load(std::string keyword, std::string sheetFile, SDL_Rect spriteRect);

	bool drawTRect(AST::Rect rect);

	void free();
#endif
	
	void drawCRect(AST::Rect rect);
	void drawGRect(AST::Rect rect);
	bool draw(AST::Rect rect);
	
} // namespace SpriteManager