#pragma once
#include "../00_Base/SceneBase.h"

class UIEnding;

/*********************************************
*	エンディングクラス.
**/

class Ending
	: public SceneBase
{
public:
	Ending();
	~Ending() override;

	void Initialize() override;
	void Create() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
private:
	std::shared_ptr<UIEnding> m_pUI;

};