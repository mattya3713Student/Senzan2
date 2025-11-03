#pragma once
#include "Game/01_GameObject/00_MeshObject/MeshObject.h"

/*************************************************************
*	設定画面にコントローラーを表示するためのクラス.
**/

class Key final
	: public MeshObject
{
public:
	Key();
	~Key();

	void Update() override;
	void LateUpdate() override;
	void Draw() override;

private:

};