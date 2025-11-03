//#include "BossAttackManager.h"
//#include "Game/01_GameObject/00_MeshObject/00_Character/EX_Player/Player.h"
//
//BossAttackManager::BossAttackManager()
//{
//}
//
//BossAttackManager::~BossAttackManager()
//{
//	// unique_ptrは自動で解放されるため、明示的な解放処理は通常不要です。
//}
//
//void BossAttackManager::Update()
//{
//	// アクティブな攻撃があれば更新
//	if (m_pAttack)
//	{
//		m_pAttack->Update();
//		// 攻撃が終了したかどうかをチェックし、終了していればunique_ptrをリセット
//		if (!m_pAttack->IsAttackActive())
//		{
//			m_pAttack.reset(); // 攻撃が終了したので解放
//			OutputDebugStringA("BossAttackManager: Attack finished and reset.\n");
//		}
//	}
//}
//
//void BossAttackManager::Draw()
//{
//	// アクティブな攻撃があれば描画
//	if (m_pAttack)
//	{
//		m_pAttack->Draw();
//	}
//}
//
//// ボスアタックの生成と設定を行う
//void BossAttackManager::CreateBossAttack(BossAttackList attackType, const D3DXVECTOR3& bossCurrentPos)
//{
//	// 新しい攻撃を生成する前に、現在の攻撃があれば破棄する
//	m_pAttack.reset(); // 現在のunique_ptrが保持しているオブジェクトを解放
//
//	switch (attackType)
//	{
//	case BossAttackList::Slash:
//		m_pAttack = std::make_unique<Slash>();
//		// 斬撃開始メソッドを呼び出し、ボスの現在位置を渡す
//		if (Slash* slash = dynamic_cast<Slash*>(m_pAttack.get()))
//		{
//			slash->StartBossSlash(bossCurrentPos);
//		}
//		else
//		{
//			OutputDebugStringA("Error: Failed to cast to BossAttackSlash in CreateBossAttack.\n");
//		}
//		break;
//
//	case BossAttackList::Jump:
//		m_pAttack = std::make_unique<Jump>();
//		if (Jump* jump = dynamic_cast<Jump*>(m_pAttack.get()))
//		{
//			jump->StartJump(bossCurrentPos);
//		}
//		else
//		{
//			OutputDebugStringA("Error: Failed to cast to BossAttackJump in CreateBossAttack.\n");
//		}
//		break;
//
//	case BossAttackList::Charge:
//		m_pAttack = std::make_unique<SlashCharge>();
//		if (SlashCharge* charge = dynamic_cast<SlashCharge*>(m_pAttack.get()))
//		{
//			charge->StartBossSlashCharge(bossCurrentPos);
//		}
//		else
//		{
//			OutputDebugStringA("Error: Failed to cast to BossAttackSlashCharge in CreateBossAttack.\n");
//		}
//		break;
//
//	case BossAttackList::Special:
//		m_pAttack = std::make_unique<Special>();
//		if (Special* special = dynamic_cast<Special*>(m_pAttack.get()))
//		{
//			special->StartSpecialAttack(bossCurrentPos);
//		}
//		else
//		{
//			OutputDebugStringA("Error: Failed to cast to BossAttackSpecial in CreateBossAttack.\n");
//		}
//		break;
//
//	case BossAttackList::Shout:
//		m_pAttack = std::make_unique<Shout>();
//		if (Shout* shout = dynamic_cast<Shout*>(m_pAttack.get()))
//		{
//			// 修正: StartShoutAttackの引数にCPlayer*を追加
//			shout->StartShoutAttack(bossCurrentPos);
//		}
//		else
//		{
//			OutputDebugStringA("Error: Failed to cast to Shout in CreateBossAttack.\n");
//		}
//		break;
//
//	case BossAttackList::max:
//		// エラーハンドリングまたは何もしない
//		break;
//	}
//}
//
//void BossAttackManager::ResetCurrentAttack()
//{
//	m_pAttack.reset(); // 現在の攻撃を強制的に終了させ、メモリを解放
//	OutputDebugStringA("BossAttackManager: Current attack force reset.\n");
//}