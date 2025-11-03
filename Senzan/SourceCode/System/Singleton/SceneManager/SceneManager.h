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

	void Create(HWND hWnd);

	// データの読み込み.
	void LoadData();

	static void Update();
	static void Draw();

	// シーンの読み込み.
	static void LoadScene(eList Scene);

	// HWNDを取得.
	HWND GetHWND() const;

private:
	// シーンを作成.
	void MakeScene(eList Scene);

private:
	std::unique_ptr<SceneBase>		m_pScene;
	HWND		m_hWnd;
	Buffer* m_pBuffer;		// 次シーンへインスタンスを入れるバッファー.

#if _DEBUG
	eList m_DebugFirstScene;	// デバッグ時に最初に起動させるシーン.
#endif // _DEBUG
};
