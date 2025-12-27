#include "BossRoot.h"

#include "..//..//Boss.h"
#include "00_Action/00_BossMovement/00_BossIdol/BossIdol.h"
#include "00_Action/00_BossMovement/01_BossMove/BossMove.h"

namespace BossState
{
	BossState::BossRoot::BossRoot(Boss* pOwner)
		: BossStateBase(pOwner)

		, m_pIdol(nullptr)

		, m_pMove(nullptr)

		, m_CurrentState(std::ref(*this))
	{
	}

	BossState::BossRoot::~BossRoot()
	{
	}

	constexpr BossState::enID BossRoot::GetStateID() const
	{
		return m_CurrentState.get().GetStateID();
	}

	void BossState::BossRoot::Enter()
	{
		// 実際のインスタンス生成
		m_pIdol = std::make_unique<BossIdol>(m_pOwner);
		m_pMove = std::make_unique<BossMove>(m_pOwner);

		// 初期ステートを Idol に設定
		m_CurrentState = std::ref(*static_cast<BossStateBase*>(m_pIdol.get()));
		m_CurrentState.get().Enter();
	}

	void BossState::BossRoot::Update()
	{
		m_CurrentState.get().Update();
	}

	void BossState::BossRoot::LateUpdate()
	{
		m_CurrentState.get().LateUpdate();
	}

	void BossState::BossRoot::Draw()
	{
		m_CurrentState.get().Draw();
	}

	void BossState::BossRoot::Exit()
	{
		m_CurrentState.get().Exit();
	}

	void BossState::BossRoot::ChangeState(BossState::enID id)
	{
		if (m_CurrentState.get().GetStateID() == id) return;

		m_CurrentState.get().Exit();

		switch (id)
		{
		case BossState::enID::Idol:
			m_CurrentState = std::ref(*static_cast<BossStateBase*>(m_pIdol.get()));
			break;
		case BossState::enID::Move:
			m_CurrentState = std::ref(*static_cast<BossStateBase*>(m_pMove.get()));
			break;
		}

		m_CurrentState.get().Enter();
	}

#pragma region
	std::reference_wrapper<BossIdol> BossState::BossRoot::GetIdolState()
	{
		return std::ref(*m_pIdol.get());
	}
	std::reference_wrapper<BossMove> BossRoot::GetMoveState()
	{
		return std::ref(*m_pMove.get());
	}
#pragma endregion

} //BossState.