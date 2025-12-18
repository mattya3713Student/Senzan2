#pragma once
#include "02_UIObject/UIObject.h"

/*********************************************
*	ゲームオーバーUIクラス.
**/

class UIGameOver
{
public:
	enum GameOverItems {
		Start,
		End,

		Max
	};

public:
	UIGameOver();
	~UIGameOver();

	void Update();
	void LateUpdate();
	void Draw();

	GameOverItems GetSelected();

private:
	void SelectUpdate();
	void SelectLateUpdate(std::shared_ptr<UIObject> ui);
	void InitAnim(GameOverItems  item);
	void AnimUpdate();

private:
	std::vector<std::shared_ptr<UIObject>> m_pUIs;
	GameOverItems 	m_Select;

	float	m_AnimeSpeed;
	bool	m_AnimReturn;
	float	m_InitAlpha;
	float	m_SelectAlpha;
};