#include "Boss.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossMoveState/BossMoveState.h"
#include "BossAttackStateBase/BossAttackStateBase.h"
#include "BossAttackStateBase/BossStompState/BossStompState.h"
#include "BossAttackStateBase/BossSlashState/BossSlashState.h"
#include "BossAttackStateBase/BossChargeSlashState/BossChargeSlashState.h"
#include "BossAttackStateBase/BossShoutState/BossShoutState.h"

#include "System/Utility/StateMachine/StateMachine.h"

#include "BossAttackStateBase/BossSpecialState/BossSpecialState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossLaserState/BossLaserState.h"

#include "Resource/Mesh/02_Skin/SkinMesh.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossDeadState/BossDeadState.h"

#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossChargeState/BossChargeState.h"

#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossThrowingState/BossThrowingState.h"

constexpr float HP_Max = 100.0f;

Boss::Boss()
	: Character()
	, m_State(std::make_unique<StateMachine<Boss>>(this))
	, m_PlayerPos{}
	, m_TurnSpeed(0.1f)
	, m_MoveSpeed(0.3f)
	, m_vCurrentMoveVelocity(0.f, 0.f, 0.f)
	, deleta_time(0.f)
	, m_HitPoint(0.0f)
{
	AttachMesh(MeshManager::GetInstance().GetSkinMesh("boss"));

	//DirectX::XMFLOAT3 pos = { 0.05f, 10.0f, 20.05f };
	DirectX::XMFLOAT3 pos = { 0.05f, 0.05f, 20.05f };
	DirectX::XMFLOAT3 scale = { 5.0f, 5.0f, 5.0f };
	DirectX::XMFLOAT3 Rotation = { 0.0f,0.0f,0.0f };
	m_spTransform->SetPosition(pos);
	m_spTransform->SetScale(scale);
	m_spTransform->SetRotationDegrees(Rotation);

	// ステートマシンの初期ステートを、SlashChargeStateに設定
	//Idolに遷移させるんだけど
	//アニメーションの再生系統を今日するのでここを変更していく.
	//m_State->ChangeState(std::make_shared<BossStompState>(this));
	m_State->ChangeState(std::make_shared<BossIdolState>(this));
	//m_State->ChangeState(std::make_shared<BossShoutState>());


	//攻撃動作の確認用のために書いている.
	//m_State->ChangeState(std::make_shared<BossStompState>(this));

	//ボスの最大体力.
	m_HitPoint = HP_Max;
}

Boss::~Boss()
{
}

void Boss::Update()
{
	//距離の計算後にステートを更新する.
	m_State->Update();

}

void Boss::Draw()
{
	MeshObject::Draw();
	m_State->Draw();

}
void Boss::Init()
{
}

StateMachine<Boss>* Boss::GetStateMachine()
{
	return m_State.get();
}

LPD3DXANIMATIONCONTROLLER Boss::GetAnimCtrl() const
{
	return m_pAnimCtrl;
}

void Boss::Hit()
{
	//ボスの体力の最小値.
	constexpr float zero = 0.0f;
	//ボスがPlayerからの攻撃を受けるダメージ変数.
	//このダメージは今は仮でおいているだけです
	//通常攻撃.
	constexpr float ten = 10.0f;
	//必殺技.
	constexpr float twenty = 20.0f;
	//ジャスト回避時の攻撃.
	constexpr float Five = 5.0f;
	//パリィの時の与えるダメージ.
	constexpr float Fifteen = 15.0f;

	//Bossの体力でのステートにいれる.
	constexpr float Dead_HP = zero;

	//被弾時のアニメーションの再生.
	SetAnimSpeed(0.03);
	ChangeAnim(enBossAnim::Hit);

	//いったんこの10ダメだけにしておく.
	//最後はTenをBaseにして+や-を使用する感じになると思っている.
	m_HitPoint -= ten;
	if (m_HitPoint <= ten)
	{
		m_HitPoint = ten;
		//死んだときにDeadStateclassに入る.
		m_State->ChangeState(std::make_shared<BossDeadState>(this));
	}
}

void Boss::SetTargetPos(const DirectX::XMFLOAT3 Player_Pos)
{
	m_PlayerPos = Player_Pos;
}

void Boss::HandleDamageDetection()
{
}

void Boss::HandleAttackDetection()
{
}

void Boss::HandleDodgeDetection()
{
}
