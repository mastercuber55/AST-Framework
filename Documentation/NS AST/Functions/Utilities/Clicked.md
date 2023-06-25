# AST Framework
## Documentation
### namespace AST
#### Functions
##### Utility Functions
- ###### `bool AST::Clicked(SDL_FRect, int key = SDL_BUTTON_LEFT);`

**Description:** Checks if the given `SDL_FRect` is being hovered by `AST::Mouse` and given `key` is being pressed.

**Example:** `if(AST::Clicked(Button)) Button.ClickEffect();`