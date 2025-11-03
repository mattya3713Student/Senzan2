#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <stdexcept>
#include <unordered_set>
#include <locale>
#include <codecvt>

namespace MyString {
	// 値を文字列に変換する.
	template<typename T>
	std::string ToString(const T& value);

	// 文字列から値を戻す.
	template<typename T>
	T FromString(const std::string& str);

	// 特定の行の値を取り出す.
	std::string ExtractAmount(const std::string& str);

	// 特定の行を取り出す.
	std::string ExtractLine(const std::string& str, int Line);

	// 文字列をfloatへ変換.
	float Stof(std::string str);
	// 文字列をboolへ変換.
	bool Stob(std::string str);

	// UTF-16からUTF-8へ変換.
	std::string UTF16ToUTF8(const std::u16string& utf16); 

	// UTF-8からUTF-16へ変換.
	std::u16string UTF8ToUTF16(const std::string& utf8); 

	/**********************************************************
	* @brief 指定したコードページでエンコードされたstringをwstringに変換.
	* @param[in] str		：変換したい文字列。
	* @param[in] codePage	：変換元のコードページ（例: CP_UTF8, CP_ACP）.
	* @return 変換後のwstring.
	**********************************************************/
	std::wstring CovertToWString(const std::string& str, UINT codePage);


	/**********************************************************
	 * @brief wstringを指定したコードページのstringに変換.
	 * @param wstr		：変換したい文字列.
	 * @param codePage	：変換後の文字列のコードページ（例: CP_UTF8, CP_ACP）.
	 * @return 変換後のstring.
	**********************************************************/
	std::string ConvertFromWString(const std::wstring& wstr, UINT codePage);


	/**********************************************************
	* @brief 指定したエンコーディングのstringを、別のエンコーディングのstringに変換する.
	* @param str			：変換したい文字列.
	* @param fromCodePage	：変換元のコードページ（例: CP_UTF8, CP_ACP）.
	* @param toCodePage		：変換先のコードページ（例: CP_UTF8, CP_ACP）.
	* @return 変換後のstring.
	**********************************************************/
	std::string ConvertEncodeing(const std::string& str, UINT fromCodePage, UINT toCodePage);


	/**********************************************************
	* @brief wstringを指定したエンコーディングのwstringに変換する.
	* @param wstr		：変換したい文字列.
	* @param toCodePage	：変換後の文字列のコードページ（例: CP_UTF8, CP_ACP）.
	* @return 変換後のwstring.
	**********************************************************/
	std::wstring ConvertEncodeing(const std::wstring& wstr, UINT toCodePage);


	/**********************************************************
	* @brief Stringをwstringに変換.
	* @param[in] str：変換したい文字列.
	* @return wstringの文字列.
	**********************************************************/
	const std::wstring StringToWString(const std::string& str);


	/**********************************************************
	* @brief wstringをStringに変換.
	* @param[in] wstr：変換したいwstring.
	* @return stringの文字列.
	**********************************************************/
	const std::string WStringToString(const std::wstring& wstr);

}