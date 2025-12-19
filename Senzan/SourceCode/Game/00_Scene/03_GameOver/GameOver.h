#pragma once
#include "../00_Base/SceneBase.h"

/*********************************************
*	ゲームオーバー画像クラス.
**/

class UIGameOver;

class GameOver
	: public SceneBase
{
public:
	GameOver();
	~GameOver() override;

	void Initialize() override;
	void Create() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;

private:
	std::shared_ptr<UIGameOver> m_pUI;
};