#pragma once
#include "System/Singleton/SingletonTemplate.h"
/****************************
*   シーンマネージャークラス.
*   Singleton.
* 　担当:mattya3713
****/

class Buffer;
class SceneBase;
class SceneUIBase;
class SEBase;


// シーンリスト列挙型.
enum class eList
{
	Title,		// タイトル.
	GameMain,	// メイン.
	GameOver,	// ゲームオーバー.
	Ending, 	// エンディング.

	// テスト用シーン.
#if _DEBUG
	Mattya, 
	Memeu, 	
	L, 	

	UIEditor,

	AnimationTuning,
#endif // _DEBUG

	MAX,
};

class SceneManager :
	public Singleton<SceneManager>
{ 
	friend class Singleton<SceneManager>;
private:
	SceneManager();
public:
	~SceneManager();

	// データの読み込み.
	void LoadData();

	static void Update();
	static void Draw();

	// シーンの読み込み.
	static void LoadScene(eList Scene);

private:
#if _DEBUG
	const char* GetSceneName(eList scene) {
		switch (scene) {
		case eList::Title:    return "Title";
		case eList::GameMain: return "GameMain";
		case eList::GameOver: return "GameOver";
		case eList::Ending:   return "Ending";
		case eList::Mattya:  return "MattyaTest";
		case eList::Memeu:   return "MemeuTest";
		case eList::L:       return "LTest";
		case eList::UIEditor: return "UIEditor";
		default: return "Unknown";
		}
	}
#endif // _DEBUG

private:
	// シーンを作成.
	void MakeScene(eList Scene);

private:
	std::unique_ptr<SceneBase>		m_pScene;
	eList m_NextScene;
	Buffer* m_pBuffer;		// 次シーンへインスタンスを入れるバッファー.

#if _DEBUG
	eList m_CurrentSceneID;
	eList m_DebugFirstScene;	// デバッグ時に最初に起動させるシーン.
#endif // _DEBUG
};
