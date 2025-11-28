#pragma once
#include "../00_Base/SceneBase.h"
#include "02_UIObject/UIObject.h"

/*********************************************
*	タイトルクラス.
**/

class Title
	: public SceneBase
{
public:
	Title();
	~Title() override;

	void Initialize() override;
	void Create() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	
private: 	
	std::vector<std::shared_ptr<UIObject>> m_pUIs;	// UIクラス.
};