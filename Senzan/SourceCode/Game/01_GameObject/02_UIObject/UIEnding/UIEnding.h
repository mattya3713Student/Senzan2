#pragma once
#include "02_UIObject/UIObject.h"

/*********************************************
*	タイトルUIクラス.
**/

class UIEnding
{
public:
	UIEnding();
	~UIEnding();

	void Update();
	void LateUpdate();
	void Draw();

private:
	void SelectUpdate();
	void SelectLateUpdate(std::shared_ptr<UIObject> ui);
	void AnimUpdate();

private:
	std::vector<std::shared_ptr<UIObject>> m_pUIs;

	float	m_AnimeSpeed;
	bool	m_AnimReturn;
	float	m_InitAlpha;
	float	m_SelectAlpha;
};