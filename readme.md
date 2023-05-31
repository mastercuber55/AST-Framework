
# AST Framework

A simple and flexible SDL2 wrapper. AST stands for Abstract Syntax Tree and my real name.

## Installation

There is no installation just download the source code or execute the following command to create a new project.

```bash
  git clone https://github.com/mastercuber55/AST-Framework.git
```

## Documentation

### Variables
#### namespace AST
```cpp
// Info: Can be used with void AST::fullscreen(bool yes); and all SDL functions that take SDL_Window * as a argument.
// Example: SDL_SetWindowResizable(AST::win, SDL_FALSE); 
SDL_Window * AST::win;

// Info: Can be used with all SDL functions that take SDL_Renderer * as an argument.
// Example: SDL_RenderSetLogicalSize(AST::ren, 1920, 1080);
SDL_Renderer * AST::ren;

// Info: Makes mouse's position be updated according to grid. "1" by default.
// Example: AST::grid = 32;
int AST::grid;

// Info: SHOULD NOT BE MODIFIED.
// Example: AST::fullscreen(!AST::isFullscreen);
bool AST::isFullscreen;

// Info: Sets event.type, event.button.button, event.key.keysym.sym to true if emmited where event is a SDL_Event.
// Example: if(AST::keys[SDL_QUIT] || AST::keys[SDL_BUTTON_MIDDLE] || AST::keys[SDLK_Escape]) AST::loop = false;
std::unordered_map<int, bool> AST::keys;

// Info: Can be set to a value in one Scene and checked in if statement in another as an indicater to do something because its recommanded to not create Scene unless you are creating a temp screen to pause the current scene or do something else.
// Example: AST::code = 20;
int AST::code;
```
#### namespace SpriteManager
```cpp
// Holds SDL_Texture*(s) with the assigned keyword.
std::vector<std::pair<SDL_Texture*, std::string>> SpriteManager::sprites;
```
### Functions
#### namespace AST
```cpp
// Info: Intializates AST::win & AST::ren (Creates a maximized window and a 1920x1080 renderer);
// Example: AST::Init("Hello World");
void AST::Init(std::string WindowTitle);

// Info: calls SceneToRender.event in a while loop ( where SDL_PollEvent(&event) is something ) inside a while loop ( where AST::loop is true )
// Example:: 
// Scene_Name NameScene;
// AST::Render(NameScene)
void AST::Render(Scene & SceneToRender);

// Info: Final Clean Up
// Example: AST::Quit();
void AST::Quit(void);

// Info: Checks if a number is between two numbers.
// Example: inRange(5, 0, 10); ( true )
bool AST::inRange(int num, int min, int max);

// Info: Checks if AST::Mouse is hovering the given rect. If mouse button click is checked before calling this function that it will also work for checking if the given rect is clicked by the mouse.
// Example: if(AST::keys[SDL_BUTTON_LEFT] && AST::hovering(rect)) doSomething();
bool AST::hovering(SDL_Rect rect);

// Info: sets fullscreen according to given param.
// Example: AST::fullscreen(!AST::isFullscreen); 
void AST::fullscreen(bool yes);

// Info: from javascript. ( non blocking SDL_Delay() )
// Example: 
// AST::setTimeout([&someThingToCapture](){
//	someThingToCapture.doSomething();
// }, 1000) 
void AST::setTimeout(std::function<void()> function, int ms);
```
#### MACRO
```cpp
// Info: replaces itself with Scene's resource folder. used in Scenes/Scene_*.cpp
// Example: SpriteManager::load("Background", SCENE("Background.png"));
SCENE(std::string)
```
#### namespace SpriteManager
```cpp
// Info: Loads the file from given path and stores it into `SpriteManager::sprites` with the given `keyword`.
// Example: SpriteManager::load("Background", SCENE("Background.png")); ( It looks for "Background.png" in the resource folder of the scene )
bool SpriteManager::load(std::string keyword, std::string file);

// Info: loads a sprite from a spritesheet using the given `SDL_Rect` and stores it into the `SpriteManager::sprites` with the given keyword.
// Example: SpriteManager::load("Grass", SCENE("Tiles.png"), {64, 64, 32, 32});
bool SpriteManager::load(std::string keyword, std::string sheetFile, SDL_Rect spriteRect);

// Info: Draws a AST::Rect with the type 0. SHOULD NOT BE CALLED FOR MORE FLEXIBILITY.
// Example:: SpriteManager::draw(rect); ( where rect was constructed by providing a keyword for texture )
bool SpriteManager::drawTRect(AST::Rect rect);

// Info: Draws a AST::Rect with the type 1. SHOULD NOT BE CALLED FOR MORE FLEXIBILITY.
// Example:: SpriteManager::draw(rect); ( where rect was constructed by providing a SDL_Color )
bool SpriteManager::drawCRect(AST::Rect rect);

// Info: Draws a AST::Rect with the type 2. SHOULD NOT BE CALLED FOR MORE FLEXIBILITY.
// Example:: SpriteManager::draw(rect); ( where rect was constructed by providing two SDL_Color )
bool SpriteManager::drawGRect(AST::Rect rect);

// Info: Uses the above mentioned functions after checking rect's type.
// Example: SpriteManager::draw(rect); ( where rect was any of the possible types )
bool SpriteManager::draw(AST::Rect rect);

// Info: Frees memory taken by SpriteManager. NOTE: this function is called by the base class `Scene` and you don't need to call it but okay if you want to for some reason.
// Example: SpriteManager:free();
void SpriteManager::free(void);

IF YOU ARE CONFUSED ABOUT DRAW FUNCTION THEN SEE [Rect]()
```

### Structs And Classes
#### namespace AST

##### Scene
`Scene` is a self handling `class` that sets `bool AST::loop` to `true` on intialization and is destroyed automaticly after its work is done. ( `bool AST::loop` is set to `false` )

I was unable to get it working with pointers so I can't call delete on it at the end of `void AST::Render(Scene& scene)` and
**Scenes should be created inside scopes for self destruction**

##### How to create a scene

- Add as many functions and variables you like to add.

- `Scene_Name()` should do stuff like Intilizizing `AST::Rect`(s) and loading textues using `bool SpriteManager::load`.
- `void loop() override;` is not required but if you don't add it then the screen will be empty.
- `void event(SDL_Event &ev) override;` is not required, by default it handles `SDL_QUIT`.
- `~Scene_Name()` should be empty if you don't have anything that should managed before deconstructing.

```cpp
class Scene_Name : public AST::Scene {
	AST::Rect Background;
	public:
		Scene_Name();
		void loop() override;
		void event(SDL_Event &ev) override;
		~Scene_Name();
};
```

##### Rect
`Rect` is a extended version of `SDL_Rect` that has following variables:

- `std::string texture;` holds a keyword for its texture. (ONLY IF TYPE 0)
- `SDL_Color start;` used for single colored rect (ONLY IF TYPE 1) or start color for gradient rect (ONLY IF TYPE 2)
- `SDL_Color end;` used as end color for gradient rect (ONLY IF TYPE 2)
- `double angle;` used for rect's angle

and following constructers:

- `Rect(SDL_Rect rect, std::string texture);` used for a textured rect (TYPE 0)
- `Rect(SDL_Rect rect, SDL_Color color);` used for a single colored rect (TYPE 1)
- `Rect(SDL_Rect rect, SDL_Color start, SDL_Color end);` used for a linear gradient rect (TYPE 2)

**NOTE: Do not modify `int type;`**

### Rect Struct
```cpp
struct Rect : public SDL_Rect {
		std::string texture;
		SDL_Color start;
		SDL_Color end;
		double angle;
		int type;

		Rect(SDL_Rect rect, std::string texture);
		Rect(SDL_Rect rect, SDL_Color color);
		Rect(SDL_Rect rect, SDL_Color start, SDL_Color end);
	private: void init(SDL_Rect rect);
	};
``` 

## License

[CC BY-NC-ND 4.0](https://creativecommons.org/licenses/by-nc-nd/4.0/)