# AST Framework
## Documentation
### namespace AST
#### Variables
- ##### `int AST::code;` 

**Description:** This is the easiest way to leave behind a message for the next scene to be used in condition or something.

**Example:**

Scene_One.cpp

`AST::code = 5890;`

Scene_Another.cpp

`if(AST::code == 5890) Something();`