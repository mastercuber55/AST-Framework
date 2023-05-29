#include "Scenes.h" 
 
Scene_Name::Scene_Name() : Background(0, 0, -1, -1, "SomeKeyword") { 
    SpriteManager::load("SomeKeyword", SCENE("test/AnyImageInsideTheAssignedFolder.png")); 
    SpriteManager::load("SomeKeyword2", SCENE("test/AnySpriteSheetInsideTheAssignedFolder.png"), {64, 64, 32, 32}); 
} 
 
void Scene_Name::loop() { 
 	SpriteManager::draw(Background);
} 

void Scene_Name::event(SDL_Event &ev) { 
    AST::HandleEv(ev); 
    if(AST::keys[SDL_QUIT]) AST::loop = false; 
} 

Scene_Name::~Scene_Name() { 
    SpriteManager::free(); 
