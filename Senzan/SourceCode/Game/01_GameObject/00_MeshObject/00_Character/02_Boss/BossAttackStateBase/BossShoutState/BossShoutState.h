#pragma once
#include "..//BossAttackStateBase.h"
#include "Game\03_Collision\00_Core\01_Capsule\CapsuleCollider.h"

/************************************************************************************
*	叫び攻撃：叫びの攻撃は当たり判定でノックバックさせるのでここでは時間でのIdolの遷移を書く.
**/


class BossIdolState;

class BossShoutState
	: public BossAttackStateBase
{
public:
	enum class enShout : byte
	{
		none,
		Shout,
		ShoutTime,
		ShoutToIdol,
	};
public:
	BossShoutState(Boss* owner);
	~BossShoutState() override;

	void Enter() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void Exit() override;

private:
    void DrawImGui() override;
    void LoadSettings() override;
    void SaveSettings() const override;
    std::filesystem::path GetSettingsFileName() const override { return std::filesystem::path("BossShoutState.json"); }
private:
	std::shared_ptr<BossIdolState> m_pBossIdol;

	// 変換用タイマー
	float m_TransitionTimer = 60.0f;

	enShout m_List;

	// 叫び攻撃の設定.
	float m_ShoutDamage = 10.0f;     // ダメージ量.
	float m_ShoutRadius = 30.0f;     // 範囲半径.
	float m_KnockBackPower = 15.0f;  // ノックバック力.
};
