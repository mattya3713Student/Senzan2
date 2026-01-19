#pragma once

#include "..//BossAttackStateBase.h"	//基底クラス.

class BossIdolState;
class Boss;

class BossStompState final
	: public BossAttackStateBase
{
public:
	enum class enAttack : byte
	{
		None,		//何もしない.
		Stomp,		//踏みつけ攻撃をする.
		CoolTime,	//クールタイム.
		Trans		//Idolに状態遷移.
	};

public:
	BossStompState(Boss* owner);
	~BossStompState() override;

	void Enter() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void Exit() override;

    // ImGui と設定の読み書き
    void DrawImGui() override;
    void LoadSettings() override;
    void SaveSettings() const override;
    std::filesystem::path GetSettingsFileName() const override { return std::filesystem::path("BossStompState.json"); }
private:
    void BossAttack();
private:
	enAttack m_List;

	std::shared_ptr<BossIdolState> m_pIdol;
	//====================================================
	// 踏みつけに必要になるメンバ変数を書く.
	//====================================================
	DirectX::XMFLOAT3 m_Velocity;

	//ジャンプの初速度.
	float m_JumpPower;
	//重力加速度.
	float m_Gravity;
	//ジャンプ中のフラグ.
	bool m_JumpFrag;
	//着地フラグ.
	//この時に時間を取得して3秒後にIdolに遷移等を書く.
	bool m_GroundedFrag;
	//===================================================
	// タイマー変数.
	//===================================================
	//タイマーの初期化変数.
	float m_Timer;
	//遷移させるタイミングの変数.
	float TransitionTimer;

	//上がる速度のスピード設定用.
	float m_UpSpeed;

	//====================================================
	// 飛びかかり（前方移動）用メンバ変数.
	//====================================================
	float m_ForwardSpeed;           // 前方移動速度.
	DirectX::XMFLOAT3 m_TargetPos;  // 目標位置（プレイヤー位置）.
	DirectX::XMFLOAT3 m_StartPos_Stomp;   // 開始位置.
	bool m_HasLanded;               // 着地済みフラグ（ダメージ1回のみ）.
};
