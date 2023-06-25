# AST Framework
## Documentation
### namespace SpriteManager
#### Variables
- ##### `std::unordered_map<std::string, SDL_Texture*> SpriteManager::Sprites;`

**Description:** Holds all textures loaded by the `SpriteManager`.

**Example:** None

#### Functions
- ##### `bool SpriteManager::Load(std::string, std::string file);`

**Description:** Loads a file present in `Resources/GFX/` and stores it into `SpriteManager::Sprites` with the given keyword.

**Example:** `SpriteManager::Load("Cat", "BlackCat.png");`

- ##### `bool SpriteManager::Load(std::string keyword, std::string sheetFile, SDL_Rect spriteRect);`

**Description:** Loads a spritesheet file present in `Resources/GFX/` and rips apart a part of it according to the given `spriteRect` and stores it into `SpriteManager::Sprites` with the given keyword.

**Example:** `SpriteManager::Load("Cat", "Cats.png", {32, 32, 32, 32});`

- ##### `void SpriteManager::Free();`

**Description:** Frees memory taken by all `SDL_Texture*`(s) in the `SpriteManager::Sprites`

**Example:** `SpriteManager::Free();`

###### THE ABOVE MENTIONED FUNCTIONS & VARIABLES ARE ONLY AVAIABLE IF `-DAST_TEXTURE` WAS PASSED AS A CXX FLAG

- #### `bool SpriteManager::Render(AST::Rect)`

**Description:** Renders the given `AST::Rect` and returns true on success and false on failure.

**Example:** `SpriteManager::Render(Box);`

