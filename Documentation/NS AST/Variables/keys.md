# AST Framework
## Documentation
### namespace AST
#### Variables
- ##### `std::unordered_map<int, bool> AST::keys;`
 
**Description:** Can be used to check if a key is pressed and is updated by the framework.

**Example:** `if(AST::keys[SDL_BUTTON_MIDDLE] || AST::keys[SDLK_Escape]) AST::loop = false;`
