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
	// 前フレームの Y 値を保持してスムーズに移動させる
	// float m_LastY; // 一時無効化: Y 制御オフ
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
    // timers / flags
    // 待機してからアニメ速度を遅くする設定
    float m_WaitSeconds = 0.4f;
    float m_SlowAnimSpeed = 0.5f;
    // 事前にスロー化するためのタイマー
    // float m_PreloadTimer = 0.0f;
    // bool  m_HasPreSlowed = false;
    bool  m_AnimSlowed = false;
    bool  m_IsMoving = false;
    // 連続待ち用タイマー
    float m_StateTimer = 0.0f;
    /* 上昇/下降関連を一時無効化
    // ゆったり上がるフェーズ制御
    float m_AscentTime;       // 上昇にかける時間
    float m_AscentTimer;      // 上昇タイマー
    bool  m_IsInFallPhase;    // 落下フェーズに移行したか

    // 落下フェーズの水平加速
    float m_FallAccel;        // 水平加速量
    float m_CurrentHorizSpeed;// 現在の水平速度
    float m_MaxFallHorizSpeed;// 最大水平速度
    */

	//====================================================
	// 飛びかかり（前方移動）用メンバ変数.
	//====================================================
	float m_ForwardSpeed;           // 前方移動速度.
	DirectX::XMFLOAT3 m_TargetPos;  // 目標位置（プレイヤー位置）.
	DirectX::XMFLOAT3 m_StartPos_Stomp;   // 開始位置.
	bool m_HasLanded;               // 着地済みフラグ（ダメージ1回のみ）.
};
