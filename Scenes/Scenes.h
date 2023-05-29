#pragma once

#include "../AST.h"

class Scene_Name : public AST::Scene {
	AST::Rect Background;
	public:
		Scene_Name();
		void loop() override;
		void event(SDL_Event &ev) override;
		~Scene_Name();
};