#pragma once
#include "..//BossAttackStateBase.h"

/*******************************************************************************
*	投擲攻撃：雪だるまを投げる攻撃.
**/

class Boss;
class BossIdolState;
class Time;
class SnowBall;

class BossThrowingState final
	: public BossAttackStateBase
{
public:
	enum class enThrowing : byte
	{
		None,		//何もしない
		Anim,		//投擲アニメションを再生.
		Attack,		//投擲攻撃.
		CoolDown,	//クールダウン.
		Trans		//次のステートに遷移.
	};

	//投擲のパリィ時の列挙.
	enum class enParry : byte
	{
		none,			//何もしない.
		Flinch,			//パリィ時.
		FlinchTimer,	//怯み状態中.
		FlinchToIdol,	//怯みから待機への遷移.
	};
public:
	BossThrowingState(Boss* owner);
	~BossThrowingState() override;

	void Enter() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void Exit() override;

	void ParryTime();

private:
	void BossAttack() override;
private:
	//================================================
	// 投擲のメンバ変数を作成.
	//================================================
	//遷移のタイミング取得用のタイム変数.
	float m_Timer;
	//遷移の時間のタイム変数.
	float m_TransitionTimer;

	enThrowing m_List;

	enParry m_Parry;

	std::unique_ptr<SnowBall> m_pBall;

	bool m_IsLaunched = false;
};