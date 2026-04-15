#pragma once
#include <DxLib.h>

namespace Transform
{
	MATRIX RotateXYZ(const VECTOR& euler);
	MATRIX Multiply(const MATRIX& child, const MATRIX& parent);
	MATRIX CombineEuler(const VECTOR& childEuler, const VECTOR& parentEuler);
}