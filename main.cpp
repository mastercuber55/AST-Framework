#include "AST.h"
#include "Scenes/Scenes.h"

int main(int argc, char *argv[]) {
	AST::Init("WINDOW TITLE HERE");
	
	{
		Scene_Name NameScene;
		AST::Render(NameScene);
	}

	if(AST::instruction == "Something") {
		Scene_Name NameScene;
		AST::Render(NameScene);
	}

	AST::Quit();
	return false;
}
