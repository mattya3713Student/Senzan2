#pragma once
/********************************************
*	ボスのステートIDクラス.
**/

namespace BossState
{
	enum class enID
	{
		None = 0,	//何もしない.

		//GamePlayer//Movement.
		Idol,
		Move,
		Dead,

		//GamePlayer//Main
		Slash,
		Charge,
		ChargeSlash,
		Shout,
		Stomp,
		Special,
		Throwing,
		Laser,

		Max

	};
}