#include "Action.h"

#include "00_MeshObject/00_Character/01_Player/Player.h"

#include "Game/05_InputDevice/VirtualPad.h"

#include "System/Utility/SingleTrigger/SingleTrigger.h"

namespace PlayerState {
Action::Action(Player* owner)
	: PlayerStateBase(owner)
	, m_AnimSpeedChangedTrigger(std::make_unique<SingleTrigger>())
{
}

Action::~Action()
{
}

void Action::Enter()
{
}

void Action::Update()
{
	// フラグ競合防止：ノックバック中や死亡中は入力を受け付けない
	if (m_pOwner->IsKnockBack() || m_pOwner->IsDead())
	{
		return;
	}

	// 必殺技ボタンが押されたら、かつゲージがMAXの場合
	// TODO: VirtualPadに必殺技用のアクションを追加する必要がある場合はここを調整
	// 仮にeGameAction::SpecialAttackがあると仮定、またはパリィボタン長押しなど
	if (VirtualPad::GetInstance().IsActionDown(VirtualPad::eGameAction::Parry))
	{
		// ゲージがMAXかチェック（パリィ中でないことも確認）
		if (m_pOwner->GetUltValue() >= m_pOwner->GetMaxUltValue() && !m_pOwner->IsParry())
		{
			m_pOwner->ChangeState(PlayerState::eID::SpecialAttack);
			return;
		}
	}

	// 回避ボタンが押されたら.
	if (VirtualPad::GetInstance().IsActionDown(VirtualPad::eGameAction::Dodge))
	{
		// ジャスト回避に派生.
		if (m_pOwner->m_IsJustDodgeTiming)
		{
			m_pOwner->ChangeState(PlayerState::eID::JustDodge);
		}
		// 回避に派生.	
		else
		{
			m_pOwner->ChangeState(PlayerState::eID::DodgeExecute);
		}
		return;
	}
}

void Action::LateUpdate()
{
    RotetToFront();
}

void Action::Draw()
{
}

void Action::Exit()
{
}
} // PlayerState.
