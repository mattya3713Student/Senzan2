#pragma once
/***************************************************
*	ボスのステートIDクラス.
*	担当 : 西村 日向.
*	名前がかぶらないようにnamespaceをしようする.
**/

namespace BossState
{
	//列挙型でリストを作成する.
	enum class enID
	{
		None = 0,	//何もしないID.

		//GamePlay: movement.
		Idol,		//待機状態.
		Move,		//プレイヤーの方に進む・攻撃する状態.
		Dead,		//死亡.

		//GamePlay: Attack
		Slash,		//通常攻撃.
		Charge,		//ため時間.
		ChargeSlash,//ため斬り.
		Shout,		//叫び.
		Stomp,		//ジャンプ.
		Throwing,	//投擲.
		Laser,		//レーザー.
		Special,	//特殊攻撃[必殺技].

		Max
	};
}