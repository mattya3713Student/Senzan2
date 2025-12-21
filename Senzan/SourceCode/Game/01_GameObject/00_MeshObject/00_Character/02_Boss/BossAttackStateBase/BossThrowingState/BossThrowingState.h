#pragma once
#include "..//BossAttackStateBase.h"
#include <memory>
#include <DirectXMath.h>

class Boss;
class SnowBall;

class BossThrowingState final
	: public BossAttackStateBase
{
public:
	enum class enThrowing : byte
	{
		None,		// 初期状態
		Anim,		// 溜めアニメーション
		Attack,		// 発射・移動中
		CoolDown,	// 投げ終わりの余韻
		Trans		// 次のステートへ
	};

	enum class enParry : byte
	{
		none,
		Flinch,
		FlinchTimer,
		FlinchToIdol,
	};

public:
	BossThrowingState(Boss* owner);
	~BossThrowingState() override;

	void Enter() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void Exit() override;

	void ParryTime() override ;

private:
	void BossAttack() override;

private:
	float m_Timer;
	float m_TransitionTimer;

	enThrowing m_List;
	enParry m_Parry;

	std::unique_ptr<SnowBall> m_pBall;
	bool m_IsLaunched = false;
};