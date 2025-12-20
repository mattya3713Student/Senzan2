#pragma once
#include "02_UIObject/UIObject.h"
#include "Game//04_Time//Time.h"

/*********************************************
*	ゲームメインUIクラス.
**/

class UIGameMain
{
	struct Gauge
	{
		float Max = 0.0f;
		float Now = 0.0f;
		float Before = 0.0f;

		float InitRate = 1.0f;
		float Rate = 1.0f;

		// 状態.
		bool IsChanged = false;
		bool IsDelay = false;
		bool IsEasing = false;

		// easing.
		float EaseFrame = 30.0f;
		float DelayFrame = 60.0f;
		float EaseTime = 0.0f;
		float EaseMax = 0.3f;
		float EaseStart = 0.0f;
		float EaseEnd = 0.0f;

		// delay（赤用）.
		float DelayTime = 0.0f;
		float DelayMax = 0.4f;

		void Set(float max, float now);
		void Update(float dt);
		void StartFollow(float targetRate);
		bool ConsumeChanged();
	};

public:
	UIGameMain();
	~UIGameMain();

	void Update();
	void LateUpdate();
	void Draw();

	void Create();

	/******************************************************
	* @brief コンボ数を設定.
	* @param num	コンボ数.
	******************************************************/
	void SetCombo(int num);


	/******************************************************
	* @brief 時間を設定.
	* @param max	最大時間.
	* @param now	現時間.
	******************************************************/
	void SetTime(float max, float now);


	/******************************************************
	* @brief プレイヤーHPを設定.
	* @param max	最大ステータス.
	* @param now	現ステータス.
	******************************************************/
	void SetPlayerHP(float max,float now);


	/******************************************************
	* @brief プレイヤーUltを設定.
	* @param max	最大ステータス.
	* @param now	現ステータス.
	******************************************************/
	void SetPlayerUlt(float max, float now);


	/******************************************************
	* @brief ボスHPを設定.
	* @param max	最大ステータス.
	* @param now	現ステータス.
	******************************************************/
	void SetBossHP(float max, float now);


private:
	std::vector<std::shared_ptr<UIObject>> m_pUIs;

	float m_GuageDelSpeed;
	float m_ClockSecInitRot;
	float m_ClockSecNow;

	int m_Combo;
	int m_ComboMax;

	Gauge m_PlayerHP;
	Gauge m_PlayerDamage;
	Gauge m_PlayerUlt;
	Gauge m_BossHP;
	Gauge m_BossDamage;

#if _DEBUG
	// Debug用.
	float m_DebugPlayerHP = 1.0f;
	float m_DebugPlayerUlt = 1.0f;
	float m_DebugBossHP = 1.0f;

	float m_DebugPlayerHPMax = 100.0f;
	float m_DebugPlayerUltMax = 100.0f;
	float m_DebugBossHPMax = 500.0f;
#endif
};