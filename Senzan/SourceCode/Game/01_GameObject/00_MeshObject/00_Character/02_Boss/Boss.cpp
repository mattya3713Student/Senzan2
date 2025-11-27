#include "Boss.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossIdleState/BossIdleState.h"
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

Boss::Boss()
	: Character()
	, m_State					(std::make_unique<StateMachine<Boss>>(this))
	, m_PlayerPos				{}
	, m_TurnSpeed				(0.1f)
	, m_MoveSpeed				(0.3f)
	, m_vCurrentMoveVelocity	(0.f, 0.f, 0.f)
	, deleta_time				(0.f)
{
	AttachMesh(MeshManager::GetInstance().GetSkinMesh("extinger"));

	//bm_pChargeSlsh = std::make_unique<SlashCharge>();

	DirectX::XMFLOAT3 pos = { 0.05f, 0.05f, 20.05f };
	DirectX::XMFLOAT3 scale = { 0.05f, 0.05f, 0.05f };
	DirectX::XMFLOAT3 Rotation = { 0.0f,0.0f,0.0f };
	m_spTransform->SetPosition(pos);
	m_spTransform->SetScale(scale);
	m_spTransform->SetRotationDegrees(Rotation);

	// ステートマシンの初期ステートを、SlashChargeStateに設定
	m_State->ChangeState(std::make_shared<BossChargeSlashState>(this));
	//m_State->ChangeState(std::make_shared<BossShoutState>());


	//攻撃動作の確認用のために書いている.
	//m_State->ChangeState(std::make_shared<BossStompState>(this));
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
