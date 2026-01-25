#pragma once
#include "System/Singleton/SingletonTemplate.h"

class Player;
class Boss;

/**********************************************************************************
* @author    : 淵脇 未来.
* @date      : 2026/01/23.
* @brief     : パリィ管理クラス.
*              PlayerとBoss間のパリィ成功処理を仲介する.
**********************************************************************************/
class ParryManager final
	: public Singleton<ParryManager>
{
private:
	friend class Singleton<ParryManager>;
	ParryManager();

public:
	~ParryManager();

	// PlayerとBossの参照を設定（シーン初期化時に呼び出す）.
	void Initialize(Player* player, Boss* boss);

	// 参照をクリア（シーン終了時に呼び出す）.
	void Clear();

    // パリィ成功を通知（Playerから呼び出される）.
    // withDelay = true の場合、ボスのパリィはアニメ再生後に delay 秒待機する挙動になる。
    void OnParrySuccess(bool withDelay = false);

	// パリィ成功時のカメラ演出が完了したか.
	bool IsParryCameraFinished() const;

    // ジャスト回避開始.
    void StartJustDodge(float TimeScale);
    void EndJustDodge();
    // ボスの攻撃判定を全てオフにする（プレイヤー側からのユーティリティ）
    void DisableBossAttackColliders();


private:
	Player* m_pPlayer = nullptr;
	Boss* m_pBoss = nullptr;
};
