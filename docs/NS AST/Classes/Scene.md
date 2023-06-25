# AST Framework
## Documentation
### namespace AST
#### Classes
- ##### `Scene`
###### ITS MOST LIKELY THAT YOU WILL CREATE A SUBCLASS OF IT AND PASS IT TO `AST::Render()` FUNCTION ELSE THIS IS USELESS
###### Functions 
- ###### `Scene()`

**Description:** Sets `AST::loop` to true to keep itself alive.

**Example:** None

- ###### `virtual void loop();`

**Description:** Does nothing and only exists to be `override;`

**Example:** None

- ###### `virtual void event();`

**Description:** Does nothing and only exists to be `override;`

**Example:** None

- ###### `~Scene()`

**Description:** Calls `SpriteManager::Free();` if `-DAST_TEXTURE` was passed as a CXXFLAG

**Example:** None

