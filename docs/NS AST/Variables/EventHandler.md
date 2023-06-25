# AST Framework
## Documentation
### namespace AST
#### Variables
- ##### `std::function<void(SDL_Event&)> AST::EventHandler;`

**Description:** Can be used to apply a global handler to the framework, sets `AST::loop = false`if `SDL_QUIT` was emitted. 

**Example:** `AST::EventHandler = MyEventHandler;`