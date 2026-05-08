#include <DxLib.h>
#include "MatrixUtility.h"

MATRIX MatrixUtility::GetMatrixRotateXYZ(const VECTOR& euler)
{

	MATRIX ret = MGetIdent();
	ret = MMult(ret, MGetRotX(euler.x));
	ret = MMult(ret, MGetRotY(euler.y));
	ret = MMult(ret, MGetRotZ(euler.z));

	return ret;

}

MATRIX MatrixUtility::Multiplication(const MATRIX& child, const MATRIX& parent)
{
	return MMult(child, parent);
}

MATRIX MatrixUtility::Multiplication(const VECTOR& childEuler, const VECTOR& parentEuler)
{
	MATRIX childMat = GetMatrixRotateXYZ(childEuler);

	MATRIX parentMat = GetMatrixRotateXYZ(parentEuler);

	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	return MMult(childMat, parentMat);
}
