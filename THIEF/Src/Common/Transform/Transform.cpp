#include "Transform.h"

namespace Transform
{
	MATRIX RotateXYZ(const VECTOR& euler)
	{
		MATRIX m = MGetIdent();
		m = MMult(m, MGetRotX(euler.x));
		m = MMult(m, MGetRotY(euler.y));
		m = MMult(m, MGetRotZ(euler.z));
		return m;
	}

	MATRIX Multiply(const MATRIX& child, const MATRIX& parent)
	{
		return MMult(child, parent);
	}

	MATRIX CombineEuler(const VECTOR& childEuler, const VECTOR& parentEuler)
	{
		return Multiply(RotateXYZ(childEuler), RotateXYZ(parentEuler));
	}
}
