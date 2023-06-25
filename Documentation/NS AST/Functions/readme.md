# AST Framework
## Documentation
### namespace AST
#### Functions
##### Utility Functions
- [`SDL_Color AST::RandomColor();`](./Utilities/RandomColor.md)
- [`bool AST::InRange(int num, int min, int max);`](./Utilities/InRange.md)
- [`bool AST::Hovering(SDL_FRect);`](./Utilities/Hovering.md)
- [`bool AST::Clicked(SDL_FRect, int key = SDL_BUTTON_LEFT);`](./Utilities/Clicked.md)
- [`void AST::Fullscreen(bool = !AST::isFullscreen);`](./Utilities/Fullscreen.md)
- [`void AST::SetTimeout(std::function<void()> function, int ms);`](./Utilities/SetTimeout.md)
##### Basic Functions
- [`void AST::Init(std::string title, SDL_Rect rect = { -1, -1, -1, -1 });`](./Basics/Init.md)
- [`void AST::Render(Scene & scene);`](./Basics/Render.md)
- [`void AST::Quit();`](./Basics/Quit.md)