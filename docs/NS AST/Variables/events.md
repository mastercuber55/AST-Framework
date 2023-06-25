# AST Framework
## Documentation
### namespace AST
#### Variables
- ##### `std::unordered_map<int, bool> AST::events;`

**Description:** Can be used to check if a event was emitted and is auto updated by the framework.

**Example:** `if(AST::events[SDL_QUIT]) AST::loop = false;`