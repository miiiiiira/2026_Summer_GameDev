#pragma once
#include <vector>
#include <string>
#include <DxLib.h>

#include "Vector2.h"

namespace Math
{
	// ---- 定数 ----
	// 
	// ラジアン(rad)・度(deg)変換用
	constexpr float RAD2DEG = (180.0f / DX_PI_F);
	constexpr float DEG2RAD = (DX_PI_F / 180.0f);
	constexpr float kEpsilonNormalSqrt = 1e-15F;

	constexpr VECTOR VECTOR_ZERO = { 0.0f, 0.0f, 0.0f };
	constexpr VECTOR VECTOR_ONE = { 1.0f, 1.0f, 1.0f };

	// 回転軸
	constexpr VECTOR AXIS_X = { 1.0f, 0.0f, 0.0f };
	constexpr VECTOR AXIS_Y = { 0.0f, 1.0f, 0.0f };
	constexpr VECTOR AXIS_Z = { 0.0f, 0.0f, 1.0f };

	// 方向
	constexpr VECTOR DIR_F = { 0.0f, 0.0f, 1.0f };
	constexpr VECTOR DIR_B = { 0.0f, 0.0f, -1.0f };
	constexpr VECTOR DIR_R = { 1.0f, 0.0f, 0.0f };
	constexpr VECTOR DIR_L = { -1.0f, 0.0f, 0.0f };
	constexpr VECTOR DIR_U = { 0.0f, 1.0f, 0.0f };
	constexpr VECTOR DIR_D = { 0.0f, -1.0f, 0.0f };

	// ---- 基本関数 ----
	// 
	// 四捨五入
	int Round(float v);

	// 文字列の分割
	std::vector<std::string> Split(const std::string& line, char delimiter);

	// ---- 角度変換 ----
	// 
	// ラジアン(rad)から度(deg)
	double Rad2Deg(double rad);
	float Rad2Deg(float rad);

	// 度(deg)からラジアン(rad)
	double Deg2Rad(double deg);
	float Deg2Rad(float deg);

	// 0～360度の範囲に収める
	double DegIn360(double deg);

	// 0(0)～2π(360度)の範囲に収める
	double RadIn2PI(double rad);

	// 角度を -π ～ +π に正規化
	float NormalizeAngle(float rad);

	// 回転が少ない方の回転向きを取得する(時計回り:1、反時計回り:-1)
	int DirNearAroundRad(float from, float to);

	// 回転が少ない方の回転向きを取得する(時計回り:1、反時計回り:-1)
	int DirNearAroundDeg(float from, float to);

	// ---- 補間 ----
	//
	// 線形補間
	int Lerp(int start, int end, float t);
	float Lerp(float start, float end, float t);
	double Lerp(double start, double end, double t);
	Vector2 Lerp(const Vector2& start, const Vector2& end, float t);
	VECTOR Lerp(const VECTOR& start, const VECTOR& end, float t);

	// 角度の線形補間
	double LerpDeg(double start, double end, double t);

	// 角度の線形補間(常に最短経路)
	float LerpAngle(float from, float to, float t);

	// 色の線形補間
	COLOR_F Lerp(const COLOR_F& start, const COLOR_F& end, float t);

	// ---- 曲線 ----
	//
	// ベジェ曲線
	Vector2 Bezier(const Vector2& p1, const Vector2& p2, const Vector2& p3, float t);
	VECTOR Bezier(const VECTOR& p1, const VECTOR& p2, const VECTOR& p3, float t);

	// ---- ベクトル ----
	// 
	// ベクトルの長さ
	double Magnitude(const Vector2& v);
	double Magnitude(const VECTOR& v);
	float MagnitudeF(const VECTOR& v);

	float SqrMagnitude(const Vector2& v);
	float SqrMagnitudeF(const VECTOR& v);
	double SqrMagnitude(const VECTOR& v);
	double SqrMagnitude(const VECTOR& v1, const VECTOR& v2);

	double Distance(const Vector2& v1, const Vector2& v2);
	double Distance(const VECTOR& v1, const VECTOR& v2);

	// 比較
	bool Equals(const VECTOR& v1, const VECTOR& v2);
	bool EqualsVZero(const VECTOR& v1);

	// 正規化
	VECTOR Normalize(const Vector2& v);
	VECTOR Normalize(const VECTOR& v);

	// 2つのベクトルの間の角度
	double AngleDeg(const VECTOR& from, const VECTOR& to);
}