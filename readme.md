
# AST Framework

A simple and flexible SDL2 wrapper. AST stands for Abstract Syntax Tree and my real name.

## Installation

There is no installation just download the source code or execute the following command to create a new project.

```bash
  git clone https://github.com/mastercuber55/AST-Framework.git
```

## Documentation

**`SDL_Window * AST::win;`** Can be used to do all the SDL stuff that takes `SDL_Window *` as a parameter. see `void fullscreen(bool);`

\
**`SDL_Renderer * AST::ren;** Can be used to do all the SDL stuff that takes `SDL_Renderer*`

\
**`SDL_Point AST::Mouse;`** Updated when using `AST::HandleEv(SDL_Event&);`

\
**`int AST::grid;`** makes `AST::Mouse` be updated according to grid. You have to manually set it. its `1` by default.

\
**`bool AST::isFullscreen;`** see ``void fullscreen(bool);``

**`std::unordered_map<int, bool> AST::keys;`** see [Event Handling](#event-handling)

**`std::string AST::instruction;`** Scene management is done using classes and it is recommanded to not create a Scene within a Scene so you can set this to a instruction that you may use in a condition in main.cpp for handling which Scene to load or to quit.

\
**`void AST::Init(std::string);`** creates a maximized window with the title given and a renderer with the size of 1980x1080.

**`void HandleEv(SDL_Event);`** see [Event Handling](#event-handling)

\
**`void Render(Scene&);`** takes a screen and renders it. uses `while(AST::loop)`, inside it, it creates `SDL_Event`, clears `AST::keys` using `keys.clear();`.
then uses `while (SDL_PollEvent(&event))` and calls the given Scene's event function with the event. (see [Event Handling](#event-handling) for more information.) **After `AST::loop` is set to false (most likely by Scene's event function) the Scene will be deleted.**

**Example**
```cpp
#include "AST.h"
#include "Scenes/Scenes.h"

int main(int argc, char *argv[]) {
	AST::Init("WINDOW TITLE HERE");
	
	{
		Scene_Name NameScene;
		AST::Render(NameScene);
	}

	if(AST::instruction == "Something") {
		Scene_Name NameScene;
		AST::Render(NameScene);
	}

	AST::Quit();
	return false;
}

```

\
I know this doesn't belong here but anyway.
**Scenes should be created inside scopes for auto deletion**

**`void Quit();`** does the clean up. should be at the end of `main` function.

### Scene class

Scene Class is a self handling class that is auto destroyed once work is done i.e. `AST::loop` is set to `false` somewhere inside the class (event function most likely).

```cpp
class Scene {
		public:
			Scene();
			virtual void loop();
			virtual void event(SDL_Event& event);
			~Scene();
	};
```
this is a base class , see the following example
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
This is supposed to be in `Scenes/Scenes.h`. you may remove `void event(SDL_Event&) override;` in case you only need to handle `SDL_QUIT`.


**`Scene();`** sets **`AST::loop`** to true when intialized.

**`~Scene();`** frees memory taken by `SpriteManager` using `void SpriteManager::free();`

**`void loop();`** does nothing so should be overrided in sub class. stuff like `SpriteManager::draw(AST::Rect)` is supposed to be here.

**`void event(SDL_Event&);`** sets `AST::loop` to false on `SDL_QUIT` but should be overrided in sub class for more functionality, see [Event Handling](#event-handling)

### Rect Struct
```cpp
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
```
Its unlikely that you would want to modify it but okay. you should use `struct MyRect : public AST::Rect {}` for that.

**`std::string texture`** used for texture lookup in `SpriteManager` draw function ONLY if you intialize the rect using `Rect(SDL_Rect, std::string);`

**`SDL_Color start**` used for a single colored rect or for gradient rect's starting color using `bool SpriteManager::draw()` function ONLY if you intialize the rect using `Rect(SDL_Rect, SDL_Color);` or `Rect(SDL_Rect, SDL_Color, SDL_Color);`

**`SDL_Color end`** used for a gradient rect's ending color using `bool SpriteManager::draw()` ONLY if you intialize the rect using `Rect(SDL_Rect, SDL_Color, SDL_Color);`

### Event Handling

this is the last section cuz I think its bit complex to understand than to use.

**`void HandleEv(SDL_Event&)`**

- this goes inside Scene's `void event(SDL_Event&);`
- updates `AST::keys`
- event's type is set to `true` in `AST::keys`, for example `AST::keys[SDL_QUIT]` will return `true` if that event was emmited. check [SDL2/SDL_Event](https://wiki.libsdl.org/SDL2/SDL_Event)
- clicked mouse button is set to `true` in `AST::keys`, for example `AST::keys[SDL_BUTTON_LEFT]` will return `true` if Left Mouse Button was clicked.  check [SDL2/SDL_MouseButtonEvent](https://wiki.libsdl.org/SDL2/SDL_MouseButtonEvent)

- pressed keyboard key is set to `true` in `AST::keys`, for example `AST::keys[SDLK_w]` will return `true` if `W` was pressed. check [SDL2/SDL_Keycode](https://wiki.libsdl.org/SDL2/SDL_Keycode)

#### Usage example
Scene_Name.cpp
```cpp
#include "Scenes.h" 
 
Scene_Name::Scene_Name() {/*Code*/} 
 
void Scene_Name::loop() {/*Code*/} 

void Scene_Name::event(SDL_Event &ev) { 
    AST::HandleEv(ev); 
    if(AST::keys[SDL_QUIT] || AST::keys[SDLK_ESCAPE] || AST::keys[SDL_BUTTON_MIDDLE]) AST::loop = false; 
} 

Scene_Name::~Scene_Name() {/*Code*/}
```

### Namespace SpriteManager
**`std::vector<std::pair<SDL_Texture*, std::string>> SpriteManager::sprites;`** holds `SDL_Texture*`(s) with assigned keyword.

**`bool SpriteManager::debug;`** if you set this to true it will notify you in the console if a texture was not found.

**`bool load(std::string keyword, std::string file);`** loads the file from given path and stores it into `SpriteManager::sprites` with the given `keyword`.

**`bool load(std::string keyword, std::string sheetFile, SDL_Rect spriteRect);`** loads a sprite from a spritesheet using the given `SDL_Rect` and stores it into the `SpriteManager::sprites` with the given keyword.

**`bool drawTRect(AST::Rect rect)`** draws a `AST::Rect` with the type "T". its recommanded that you use `bool draw(AST::Rect rect);` instead for flexiblity.

**`bool drawCRect(AST::Rect rect)`** draws a `AST::Rect` with the type "C". its recommanded that you use `bool draw(AST::Rect rect);` instead for flexiblity.

**`bool drawGRect(AST::Rect rect)`** draws a `AST::Rect` with the type "G". its recommanded that you use `bool draw(AST::Rect rect);` instead for flexiblity.

**`bool draw(AST::Rect rect)`** draws any type of `AST::Rect` to `AST::ren`. uses the three functions from above.

**`void free()`** called by the base class `Scene` to free the memory taken by `SpriteManager` when deconstructing. Its unlikely that you would be using it.

### Macro

**`SCENE`** takes a string and appends the Scene's resources folder's path as prefix. most likely used in `SpriteManager::load`

## License

[CC BY-NC-ND 4.0](https://creativecommons.org/licenses/by-nc-nd/4.0/)


#### Please Note that this Framework has some bugs and is updated frequently.