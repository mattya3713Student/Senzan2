#pragma once
#include "../Action.h"
#include <vector>

class Player;
class SingleTrigger;

/**************************************************
*	プレイヤーの攻撃の共通処理ステート(基底).
*   このステート自体がインスタンス化されることはない.
*	担当:淵脇 未来.
**/

namespace PlayerState 
{
    class Combat : public Action
    {
    public:
        Combat(Player* owner);
        ~Combat();

        virtual void Enter() override;
        virtual void Update() override;
        virtual void LateUpdate() override;
        virtual void Draw() override;
        virtual void Exit() override;

        // Collider window helpers (for attack hit windows)
        void ClearColliderWindows();
        void AddColliderWindow(float start, float duration);
        void ProcessColliderWindows(float currentTime);
        void RenderColliderWindowsUI(const char* title = "コライダー ウィンドウ設定");

    protected:
        float               m_Distance; // 各移動距離.
        float               m_MaxTime;  // 各攻撃時間.
        float               m_currentTime;  // 各経過時間.

        struct ColliderWindow {
            float start = 0.0f;    // 開始時刻(ステート秒)
            float duration = 0.1f; // 継続時間(秒)
            bool activated = false; // 既に有効化処理を行ったか
            bool deactivated = false; // 既に無効化処理を行ったか
        };

        std::vector<ColliderWindow> m_ColliderWindows; // 可変長のウィンドウリスト
        bool  m_isAttackColliderEnabled = false; // 現在コライダーが有効になっているかの目安
        int   m_ActiveWindowCount = 0; // 現在アクティブなウィンドウ数
    };
}

