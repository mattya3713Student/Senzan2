//#pragma once
//
//#include "Game/01_GameObject/00_MeshObject/00_Character/EX_Boss/BossAttack/Jump/Jump.h"
//#include "Game/01_GameObject/00_MeshObject/00_Character/EX_Boss/BossAttack/Shout/Shout.h"
//#include "Game/01_GameObject/00_MeshObject/00_Character/EX_Boss/BossAttack/Slash/Slash.h"
//#include "Game/01_GameObject/00_MeshObject/00_Character/EX_Boss/BossAttack/SlashCharge/SlashCharge.h"
//#include "Game/01_GameObject/00_MeshObject/00_Character/EX_Boss/BossAttack/Special/Special.h"
//
//
//class Player; // 前方宣言
//
///***********************************************************************
//*	ボスマネージャークラス.
//**/
//
//class BossAttackManager
//{
//public:
//	//列挙型を作成(BossList用)
//	enum class BossAttackList
//	{
//		Slash,
//		Jump, // ★追加: ジャンプ攻撃
//		Charge, // ★追加: チャージ攻撃
//		Special,
//		Shout, // ★追加: 叫び攻撃
//
//		max,
//	};
//
//public:
//	BossAttackManager();
//	~BossAttackManager();
//
//	void Update();
//	void Draw();
//
//	// ボスアタックの生成と設定を行う関数
//	// Parameters:
//	//    attackType: 攻撃の種類
//	//    bossCurrentPos: 攻撃開始時のボスの現在のワールド座標
//	// ★修正: playerTargetPosを引数に追加し、CPlayer*も追加
//	void CreateBossAttack(BossAttackList attackType, const D3DXVECTOR3& bossCurrentPos);
//
//	// 現在アクティブなボスアタックが存在するかどうかをチェックする
//	bool HasActiveAttack() const { return m_pAttack != nullptr && m_pAttack->IsAttackActive(); }
//
//	// 現在アクティブなボスアタックオブジェクトへのポインタを取得（読み取り専用）
//	// CBossクラスが攻撃の状態や位置を取得するために使用
//	BossAttack* GetActiveAttack() const { return m_pAttack.get(); }
//
//	// 現在のボスアタックを強制的に終了させる（必要であれば）
//	void ResetCurrentAttack();
//
//private:
//	std::unique_ptr<BossAttack> m_pAttack; // 現在アクティブなボスアタックを保持
//};