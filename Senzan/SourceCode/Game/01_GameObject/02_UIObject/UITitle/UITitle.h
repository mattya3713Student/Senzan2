#pragma once
#include "02_UIObject/UIObject.h"

/*********************************************
*	タイトルUIクラス.
**/

class UITitle
{
public:
	enum TitleItems {
		Start,
		End,

		Max
	};

public:
	UITitle();
	~UITitle();

	void Update();
	void LateUpdate();
	void Draw();

	TitleItems GetSelected();

private:
	void SelectUpdate();
	void SelectLateUpdate(std::shared_ptr<UIObject> ui);
	void InitAnim(TitleItems item);
	void AnimUpdate();

private:
	std::vector<std::shared_ptr<UIObject>> m_pUIs;
	TitleItems	m_Select;

	float	m_AnimeSpeed;
	bool	m_AnimReturn;
	float	m_InitAlpha;
	float	m_SelectAlpha;
};