# AST Framework
## Documentation
### namespace AST
#### Functions
##### Utility Functions
- ###### `void AST::SetTimeout(std::function<void()> function, int ms);`

**Description:** Executes the given function after given miliseconds have passed without blocking the flow of execution.

**Example:** 
```cpp
AST::SetTimeout([](){
	DoSomethingWith(IntValue);
}, 1000);
```