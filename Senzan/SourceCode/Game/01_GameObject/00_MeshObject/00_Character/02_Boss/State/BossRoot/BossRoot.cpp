#include "BossRoot.h"

#include "..//..//Boss.h"
#include "00_Action/00_BossMovement/00_BossIdol/BossIdol.h"

namespace BossState
{
	BossState::BossRoot::BossRoot(Boss* pOwner)
		: BossStateBase(pOwner)

		, m_pIdol(nullptr)
		, m_CurrentState(std::ref(*m_pIdol.get()))
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
		m_pIdol = std::make_unique<BossIdol>(m_pOwner);
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

	void BossRoot::ChangeState(BossState::enID id)
	{

	}

	std::reference_wrapper<BossIdol> BossState::BossRoot::GetIdolState()
	{
		return std::ref(*m_pIdol.get());
	}


} //BossState.