#include "MatrixUtility.h"

namespace Matrix
{
	MATRIX GetMatrixRotateXYZ(const VECTOR& euler)
	{
		MATRIX ret = MGetIdent();
		ret = MMult(ret, MGetRotX(euler.x));
		ret = MMult(ret, MGetRotY(euler.y));
		ret = MMult(ret, MGetRotZ(euler.z));

		return ret;
	}

	MATRIX Multiplication(const MATRIX& child, const MATRIX& parent)
	{
		return MMult(child, parent);
	}

	MATRIX Multiplication(const VECTOR& childEuler, const VECTOR& parentEuler)
	{
		MATRIX parent = GetMatrixRotateXYZ(parentEuler);
		MATRIX child = GetMatrixRotateXYZ(childEuler);
		return MMult(child, parent);
	}
}
