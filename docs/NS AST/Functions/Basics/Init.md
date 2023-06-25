# AST Framework
## Documentation
### namespace AST
#### Functions
- ##### `void Init(std::string title, SDL_Rect rect = { SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, -1, -1 });` 

**Description:** 

- Intializes everything according to the give `CXXFLAGS`.
- if `rect.w` & `rect.h` are -1, then the opened window will be maximized.

**Example:** `AST::Init("AST Framework");`