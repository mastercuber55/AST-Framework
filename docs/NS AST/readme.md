# AST Framework
## Documentation
### namespace AST
#### Variables
#### Classes
- [`Scene`](./Classes/Scene.md)
#### Functions
##### Utility Functions
- [`SDL_Color AST::RandomColor();`](./Functions/Utilities/RandomColor.md)
- [`bool AST::InRange(int num, int min, int max);`](./Functions/Utilities/InRange.md)
- [`bool AST::Hovering(SDL_FRect);`](./Functions/Utilities/Hovering.md)
- [`bool AST::Clicked(SDL_FRect, int key = SDL_BUTTON_LEFT);`](./Functions/Utilities/Clicked.md)
- [`void AST::Fullscreen(bool = !AST::isFullscreen);`](./Functions/Utilities/Fullscreen.md)
- [`void AST::SetTimeout(std::function<void()> function, int ms);`](./Functions/Utilities/SetTimeout.md)
##### Basic Functions
- [`void AST::Init(std::string title, SDL_Rect rect = { -1, -1, -1, -1 });`](./Functions/Basics/Init.md)
- [`void AST::Render(Scene & scene);`](./Functions/Basics/Render.md)
- [`void AST::Quit();`](./Functions/Basics/Quit.md)