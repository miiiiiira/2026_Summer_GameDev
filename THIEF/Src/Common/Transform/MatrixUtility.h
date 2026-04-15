#pragma once
#include <DxLib.h>

namespace Matrix
{
	// オイラー角から回転行列XYZ順を取得
	MATRIX GetMatrixRotateXYZ(const VECTOR& euler);

	// 親子の回転行列を合成する
	MATRIX Multiplication(const MATRIX& child, const MATRIX& parent);

	// 親子のオイラー角を合成する
	MATRIX Multiplication(const VECTOR& childEuler, const VECTOR& parentEuler);
}
