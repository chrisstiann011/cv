#include "../Precompiled.h"

bool Math::Setup( )
{
	bool bSuccess = true;

	const void* hTier0Lib = Memory::GetModuleBaseHandle( TIER0_DLL );
	if (hTier0Lib == nullptr)
		return false;

	fnRandomSeed = reinterpret_cast< decltype( fnRandomSeed ) >( Memory::GetExportAddress( hTier0Lib, X( "RandomSeed" ) ) );
	bSuccess &= ( fnRandomSeed != nullptr );

	fnRandomFloat = reinterpret_cast< decltype( fnRandomFloat ) >( Memory::GetExportAddress( hTier0Lib, X( "RandomFloat" ) ) );
	bSuccess &= ( fnRandomFloat != nullptr );

	fnRandomFloatExp = reinterpret_cast< decltype( fnRandomFloatExp ) >( Memory::GetExportAddress( hTier0Lib, X( "RandomFloatExp" ) ) );
	bSuccess &= ( fnRandomFloatExp != nullptr );

	fnRandomInt = reinterpret_cast< decltype( fnRandomInt ) >( Memory::GetExportAddress( hTier0Lib, X( "RandomInt" ) ) );
	bSuccess &= ( fnRandomInt != nullptr );

	fnRandomGaussianFloat = reinterpret_cast< decltype( fnRandomGaussianFloat ) >( Memory::GetExportAddress( hTier0Lib, X( "RandomGaussianFloat" ) ) );
	bSuccess &= ( fnRandomGaussianFloat != nullptr );

	return bSuccess;
}

Vector Math::ExtrapolateTick( const Vector& p0, const Vector& v0, const int iTicks)
{

	return p0 + ( v0 * (INTERVAL_PER_TICK * iTicks));
}

void Math::NormalizeAngles( QAngle& angAngles )
{
	for (auto i = 0; i < 3; i++)
	{
		while (angAngles[ i ] < -180.0f) angAngles[ i ] += 360.0f;
		while (angAngles[ i ] > 180.0f) angAngles[ i ] -= 360.0f;
	}
}

QAngle Math::NormalizeAnglesReturn(QAngle angle)
{
	while (angle.x > 89.f)
	{
		angle.x -= 180.f;
	}
	while (angle.x < -89.f)
	{
		angle.x += 180.f;
	}
	if (angle.y > 180)
	{
		angle.y -= (round(angle.y / 360) * 360.f);
	}
	else if (angle.y < -180)
	{
		angle.y += (round(angle.y / 360) * -360.f);
	}
	if ((angle.z > 50) || (angle.z < 50))
	{
		angle.z = 0;
	}
	return angle;
}

Vector Math::NormalizeAnglesReturn(Vector angle)
{
	while (angle.x > 89.f)
	{
		angle.x -= 180.f;
	}
	while (angle.x < -89.f)
	{
		angle.x += 180.f;
	}
	if (angle.y > 180)
	{
		angle.y -= (round(angle.y / 360) * 360.f);
	}
	else if (angle.y < -180)
	{
		angle.y += (round(angle.y / 360) * -360.f);
	}
	if ((angle.z > 50) || (angle.z < 50))
	{
		angle.z = 0;
	}
	return angle;
}

void Math::VectorAngles( const Vector& vecForward, QAngle& angView )
{
	float flPitch, flYaw;

	if (vecForward.x == 0.f && vecForward.y == 0.f)
	{
		flPitch = ( vecForward.z > 0.f ) ? 270.f : 90.f;
		flYaw = 0.f;
	}
	else
	{
		flPitch = std::atan2f( -vecForward.z, vecForward.Length2D( ) ) * 180.f / M_PI;

		if (flPitch < 0.f)
			flPitch += 360.f;

		flYaw = std::atan2f( vecForward.y, vecForward.x ) * 180.f / M_PI;

		if (flYaw < 0.f)
			flYaw += 360.f;
	}

	angView.x = flPitch;
	angView.y = flYaw;
	angView.z = 0.f;
}

Vector Math::CrossProduct(const Vector& a, const Vector& b)
{
	return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

void Math::VectorAngles(const Vector& vecForward, Vector& vecUp, QAngle& angAngles)
{
	Vector vecLeft = CrossProduct(vecUp, vecForward);
	vecLeft.NormalizeInPlace();

	float flForwardDistance = vecForward.Length2D();
	if (flForwardDistance > 0.001f)
	{
		angAngles.x = atan2f(-vecForward.z, flForwardDistance) * 180 / M_PI;
		angAngles.y = atan2f(vecForward.y, vecForward.x) * 180 / M_PI;

		float upZ = (vecLeft.y * vecForward.x) - (vecLeft.x * vecForward.y);
		angAngles.z = atan2f(vecLeft.z, upZ) * 180 / M_PI;
	}
	else
	{
		angAngles.x = atan2f(-vecForward.z, flForwardDistance) * 180 / M_PI;
		angAngles.y = atan2f(-vecLeft.x, vecLeft.y) * 180 / M_PI;
		angAngles.z = 0;
	}
}

void Math::AngleVector(const QAngle& angAngles, Vector& vecForward)
{
	float sp, sy, cp, cy;

	DirectX::XMScalarSinCos(&sp, &cp, M_DEG2RAD(angAngles[0]));
	DirectX::XMScalarSinCos(&sy, &cy, M_DEG2RAD(angAngles[1]));

	vecForward.x = cp * cy;
	vecForward.y = cp * sy;
	vecForward.z = -sp;
}

void Math::AngleVectors(const QAngle& angAngles, Vector& vecForward, Vector& vecRight, Vector& vecUp)
{
	float sr, sp, sy, cr, cp, cy;

	DirectX::XMScalarSinCos(&sp, &cp, M_DEG2RAD(angAngles[0]));
	DirectX::XMScalarSinCos(&sy, &cy, M_DEG2RAD(angAngles[1]));
	DirectX::XMScalarSinCos(&sr, &cr, M_DEG2RAD(angAngles[2]));

	vecForward.x = (cp * cy);
	vecForward.y = (cp * sy);
	vecForward.z = (-sp);

	vecRight.x = (-1 * sr * sp * cy + -1 * cr * -sy);
	vecRight.y = (-1 * sr * sp * sy + -1 * cr * cy);
	vecRight.z = (-1 * sr * cp);

	vecUp.x = (cr * sp * cy + -sr * -sy);
	vecUp.y = (cr * sp * sy + -sr * cy);
	vecUp.z = (cr * cp);
}

float Math::VectorDistance( Vector vecSource, Vector vecDestination )
{
	return sqrtf( std::pow( vecSource.x - vecDestination.x, 2 ) + std::pow( vecSource.y - vecDestination.y, 2 ) + static_cast<float>(std::pow( vecSource.z - vecDestination.z, 2 ) ) );
}

Vector2D Math::AnglePixels( const float flSensitivity, const float flPitch, const float flYaw, const QAngle& angBegin, const QAngle& angEnd )
{
	QAngle angDelta = angBegin - angEnd;
	angDelta.Normalize( );

	const float flPixelMovePitch = ( angDelta.x ) / ( flYaw * flSensitivity );
	const float flPixelMoveYaw = ( angDelta.y ) / ( flPitch * flSensitivity );

	return Vector2D( flPixelMoveYaw, flPixelMovePitch );
}

void Math::SmoothAngle( QAngle angSource, QAngle& angDestination, float flFactor )
{
    float flGoodFactor = flFactor * 0.25f;

	QAngle angDelta = angDestination - angSource;

    Math::NormalizeAngles( angDelta );

	angDestination = angSource + angDelta / flGoodFactor;
}

float Math::GetFOV( const QAngle& angViewAngle, const QAngle& angAimAngle )
{
	QAngle delta = (angAimAngle - angViewAngle);
	delta.Normalize();

	return sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f));
}

QAngle Math::CalcAngle( const Vector& vecStart, const Vector& vecEnd )
{
	QAngle angView;
	const Vector vecDelta = vecEnd - vecStart;
	VectorAngles( vecDelta, angView );
	angView.Normalize( );

	return angView;
}

Vector Math::CalculateCameraPosition(Vector anchorPos, float distance, QAngle viewAngles)
{
	float yaw = DirectX::XMConvertToRadians(viewAngles.y);
	float pitch = DirectX::XMConvertToRadians(viewAngles.x);

	float x = anchorPos.x + distance * cosf(yaw) * cosf(pitch);
	float y = anchorPos.y + distance * sinf(yaw) * cosf(pitch);
	float z = anchorPos.z + distance * sinf(pitch);

	return Vector{ x, y, z };
}

Vector Math::VecAngle(Vector vec)
{
	Vector ret;
	if (vec.x == 0.0f && vec.y == 0.0f)
	{
		ret.x = (vec.z > 0.0f) ? 270.0f : 90.0f;
		ret.y = 0.0f;
	}
	else
	{
		ret.x = M_RAD2DEG(std::atan2f(-vec.z, vec.Length2D()));
		ret.y = M_RAD2DEG(std::atan2f(vec.y, vec.x));
		if (ret.y < 0.0f) ret.y += 360.0f;
		if (ret.x < 0.0f) ret.x += 360.0f;
	}
	ret.z = 0.0f;
	return ret;
}

float Math::FloatNormalize(float flValue)
{
	while (flValue > 180.f)
		flValue -= 360.f;

	while (flValue < -180.f)
		flValue += 360.f;

	return flValue;
}

Vector Math::VectorTransform(const Vector& vecTransform, const Matrix3x4_t& matrix)
{
	return Vector(vecTransform.DotProduct(matrix[0]) + matrix[0][3],
		vecTransform.DotProduct(matrix[1]) + matrix[1][3],
		vecTransform.DotProduct(matrix[2]) + matrix[2][3]);
}

Quaternion Math::AngleQuaternion(const QAngle& angAngles)
{
	float flSinYaw = std::sin(angAngles.y);
	float flCosYaw = std::cos(angAngles.y);
	float flSinRoll = std::sin(angAngles.z);
	float flCosRoll = std::cos(angAngles.z);
	float flSinPitch = std::sin(angAngles.x);
	float flCosPitch = std::cos(angAngles.x);

	float x = flSinYaw * (flCosRoll * flCosPitch) - flCosYaw * (flSinRoll * flSinPitch);
	float y = flSinYaw * (flCosPitch * flSinRoll) + flCosYaw * (flCosRoll * flSinPitch);
	float z = flCosYaw * (flCosRoll * flSinPitch) + flSinYaw * (flCosPitch * flSinRoll);
	float w = flCosYaw * (flCosRoll * flCosPitch) - flSinYaw * (flSinRoll * flSinPitch);

	return Quaternion(x, y, z, w);
}

void Math::SinCos(float radians, float* sine, float* cosine)
{
	*sine = sinf(radians);
	*cosine = cosf(radians);
}

void Math::AxisAngleQuaternion(const Vector& axis, float angle, Quaternion& q)
{
	float sa, ca;

	SinCos(M_DEG2RAD(angle) * 0.5f, &sa, &ca);

	q.x = axis.x * sa;
	q.y = axis.y * sa;
	q.z = axis.z * sa;
	q.w = ca;
}

void Math::CrossProduct(const Vector& a, const Vector& b, Vector& result)
{
	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
}

Vector& Math::QuaternionImaginaryPart(const Quaternion& q)
{
	return *(Vector*)&q;
}

float Math::QuaternionRealPart(const Quaternion& q)
{
	return q.w;
}

void Math::VectorMA(const Vector& start, float scale, const Vector& direction, Vector& dest)
{
	dest.x = start.x + scale * direction.x;
	dest.y = start.y + scale * direction.y;
	dest.z = start.z + scale * direction.z;
}

void Math::QuaternionMultiply(const Quaternion& q, const Vector& v, Vector& result)
{
	Vector t, t2;
	CrossProduct(QuaternionImaginaryPart(q), v, t);
	t *= 2.0f;
	VectorMA(v, QuaternionRealPart(q), t, result);
	CrossProduct(QuaternionImaginaryPart(q), t, t2);
	result += t2;
}

QAngle Math::VectorToAngle(const Vector& vecForward)
{
	if (vecForward.x == 0.f && vecForward.y == 0.f)
		return QAngle(0.f, vecForward.z > 0.f ? 270.f : 90.f, 0.f);

	float flYaw = std::atan2(vecForward.y, vecForward.x) * 180.f / M_PI;
	if (flYaw < 0.f)
		flYaw += 360.f;

	float flHyp = std::sqrt(vecForward.x * vecForward.x + vecForward.y * vecForward.y);

	float flPitch = std::atan2(-vecForward.z, flHyp) * 180.f / M_PI;
	if (flPitch < 0.f)
		flPitch += 360.f;

	return QAngle(flPitch, flYaw, 0.f);
}

void Math::MatrixCopy(const Matrix3x4_t& in, Matrix3x4_t& out)
{
	CRT::MemoryCopy(out.Base(), in.Base(), sizeof(Matrix3x4_t));
}

void Math::ConcatTransforms(const Matrix3x4_t& in1, const Matrix3x4_t& in2, Matrix3x4_t& out)
{
	if (&in1 == &out)
	{
		Matrix3x4_t in1b;
		Math::MatrixCopy(in1, in1b);
		ConcatTransforms(in1b, in2, out);
		return;
	}

	if (&in2 == &out)
	{
		Matrix3x4_t in2b;
		Math::MatrixCopy(in2, in2b);
		ConcatTransforms(in1, in2b, out);
		return;
	}

	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3];

	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3];

	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3];
}

FORCEINLINE float Math::DotProduct(const float* v1, const float* v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void Math::VectorRotate(const float* __restrict in1, const Matrix3x4_t& in2, float* __restrict out)
{
	float x = DotProduct(in1, in2[0]);
	float y = DotProduct(in1, in2[1]);
	float z = DotProduct(in1, in2[2]);

	out[0] = x;
	out[1] = y;
	out[2] = z;
}

void Math::VectorRotate(const Vector& in1, const Matrix3x4_t& in2, Vector& out)
{
	VectorRotate(&in1.x, in2, &out.x);
}

void Math::MatrixSetColumn2(const Vector& in, int column, Matrix3x4_t& out)
{
	out[0][column] = in.x;
	out[1][column] = in.y;
	out[2][column] = in.z;
}

float Math::VectorNormalize(Vector& vec)
{
	float radius = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);

	float iradius = 1.f / (radius + FLT_EPSILON);

	vec.x *= iradius;
	vec.y *= iradius;
	vec.z *= iradius;

	return radius;
}

void Math::VectorVectors(const Vector& forward, Vector& right, Vector& up)
{
	Vector tmp;

	if (fabs(forward[0]) < 1e-6 && fabs(forward[1]) < 1e-6)
	{

		right[0] = 0;
		right[1] = -1;
		right[2] = 0;
		up[0] = -forward[2];
		up[1] = 0;
		up[2] = 0;
	}
	else
	{
		tmp[0] = 0; tmp[1] = 0; tmp[2] = 1.0;
		CrossProduct(forward, tmp, right);
		VectorNormalize(right);
		CrossProduct(right, forward, up);
		VectorNormalize(up);
	}
}

void Math::VectorMatrix(const Vector& forward, Matrix3x4_t& matrix)
{
	Vector right, up;
	VectorVectors(forward, right, up);

	MatrixSetColumn2(forward, 0, matrix);
	MatrixSetColumn2(-right, 1, matrix);
	MatrixSetColumn2(up, 2, matrix);
}

void Math::VectorTransform(const float* __restrict in1, const Matrix3x4_t& in2, float* __restrict out)
{
	float x = DotProduct(in1, in2[0]) + in2[0][3];
	float y = DotProduct(in1, in2[1]) + in2[1][3];
	float z = DotProduct(in1, in2[2]) + in2[2][3];

	out[0] = x;
	out[1] = y;
	out[2] = z;
}

inline void Math::VectorTransform(const Vector& in1, const Matrix3x4_t& in2, Vector& out)
{
	VectorTransform(&in1.x, in2, &out.x);
}

inline void Math::MatrixMultiply(const Matrix3x4_t& in1, const Matrix3x4_t& in2, Matrix3x4_t& out)
{
	Math::ConcatTransforms(in1, in2, out);
}

void Math::AngleMatrix(const QAngle& angles, Matrix3x4_t& matrix)
{
	float sr, sp, sy, cr, cp, cy;

	SinCos(M_DEG2RAD(angles.x), &sp, &cp);
	SinCos(M_DEG2RAD(angles.y), &sy, &cy);
	SinCos(M_DEG2RAD(angles.z), &sr, &cr);

	matrix[0][0] = cp * cy;
	matrix[1][0] = cp * sy;
	matrix[2][0] = -sp;

	const auto crcy = cr * cy;
	const auto crsy = cr * sy;
	const auto srcy = sr * cy;
	const auto srsy = sr * sy;
	matrix[0][1] = sp * srcy - crsy;
	matrix[1][1] = sp * srsy + crcy;
	matrix[2][1] = sr * cp;

	matrix[0][2] = (sp * crcy + srsy);
	matrix[1][2] = (sp * crsy - srcy);
	matrix[2][2] = cr * cp;

	matrix[0][3] = 0.0f;
	matrix[1][3] = 0.0f;
	matrix[2][3] = 0.0f;
}

void Math::MatrixAngles(const Matrix3x4_t& matrix, QAngle& angles)
{
	Vector forward, left, up;

	forward = { matrix[0][0], matrix[1][0], matrix[2][0] };
	left = { matrix[0][1], matrix[1][1], matrix[2][1] };
	up = { 0.f, 0.f, matrix[2][2] };

	float len = forward.Length2D();

	if (len > 0.001f)
	{
		angles.x = M_RAD2DEG(std::atan2(-forward.z, len));
		angles.y = M_RAD2DEG(std::atan2(forward.y, forward.x));
		angles.z = M_RAD2DEG(std::atan2(left.z, up.z));
	}

	else
	{
		angles.x = M_RAD2DEG(std::atan2(-forward.z, len));
		angles.y = M_RAD2DEG(std::atan2(-left.x, left.y));
		angles.z = 0.f;
	}
}

void Math::MatrixPosition(const Matrix3x4_t& matrix, Vector& position)
{
	position[0] = matrix[0][3];
	position[1] = matrix[1][3];
	position[2] = matrix[2][3];
}

void Math::MatrixAngles(const Matrix3x4_t& matrix, QAngle& angles, Vector& position)
{
	MatrixAngles(matrix, angles);
	MatrixPosition(matrix, position);
}

Matrix3x4_t Math::CreateRotationMatrixFromVector(const Vector& vecDirection)
{
	Vector vNormalDir = vecDirection;
	vNormalDir.NormalizeInPlace();

	Vector vUp(0.f, 1.f, 0.f);
	Vector vRight = Math::CrossProduct(vUp, vNormalDir);
	vRight.NormalizeInPlace();

	Vector vNewUp = Math::CrossProduct(vNormalDir, vRight);
	vNewUp.NormalizeInPlace();

	Matrix3x4_t matResult;

	matResult[0][0] = vRight.x;
	matResult[0][1] = vRight.y;
	matResult[0][2] = vRight.z;
	matResult[0][3] = 0.0f;

	matResult[1][0] = vNewUp.x;
	matResult[1][1] = vNewUp.y;
	matResult[1][2] = vNewUp.z;
	matResult[1][3] = 0.0f;

	matResult[2][0] = vNormalDir.x;
	matResult[2][1] = vNormalDir.y;
	matResult[2][2] = vNormalDir.z;
	matResult[2][3] = 0.0f;

	return matResult;
}

Vector Math::RotateVectorByMatrix(const Vector& vecIn, const Matrix3x4_t& matRot)
{
	return Vector(
		vecIn.x * matRot[0][0] + vecIn.y * matRot[1][0] + vecIn.z * matRot[2][0],
		vecIn.x * matRot[0][1] + vecIn.y * matRot[1][1] + vecIn.z * matRot[2][1],
		vecIn.x * matRot[0][2] + vecIn.y * matRot[1][2] + vecIn.z * matRot[2][2]
	);
}

bool Math::IntersectSegmentToSegment( Vector vecStart, Vector vecEnd, Vector vecMin, Vector vecMax, float flRadius )
{
	Vector   u = vecEnd - vecStart;
	Vector   v = vecMax - vecMin;
	Vector   w = vecStart - vecMin;
	float    a = u.DotProduct( u );
	float    b = u.DotProduct( v );
	float    c = v.DotProduct( v );
	float    d = u.DotProduct( w );
	float    e = v.DotProduct( w );
	float    D = a * c - b * b;
	float    sc, sN, sD = D;
	float    tc, tN, tD = D;

	if ( D < 0.00000001f )
	{
		sN = 0.0;
		sD = 1.0;
		tN = e;
		tD = c;
	}
	else
	{
		sN = ( b * e - c * d );
		tN = ( a * e - b * d );
		if ( sN < 0.0 )
		{
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if ( sN > sD )
		{
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if ( tN < 0.0 )
	{
		tN = 0.0;

		if ( -d < 0.0 )
			sN = 0.0;
		else if ( -d > a )
			sN = sD;
		else
		{
			sN = -d;
			sD = a;
		}
	}
	else if ( tN > tD )
	{
		tN = tD;

		if ( ( -d + b ) < 0.0 )
			sN = 0;
		else if ( ( -d + b ) > a )
			sN = sD;
		else
		{
			sN = ( -d + b );
			sD = a;
		}
	}

	sc = ( abs( sN ) < 0.00000001f ? 0.0 : sN / sD );
	tc = ( abs( tN ) < 0.00000001f ? 0.0 : tN / tD );

	Vector vecDot = w + ( u * sc ) - ( v * tc );

	float flFinalRadius = std::sqrt( vecDot.DotProduct( vecDot ) );
	return flFinalRadius < flRadius;
}

bool Math::IntersectRayWithAABB( Vector& origin, Vector& dir, Vector& min, Vector& max )
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	if ( dir.x >= 0 )
	{
		tmin = ( min.x - origin.x ) / dir.x;
		tmax = ( max.x - origin.x ) / dir.x;
	}
	else
	{
		tmin = ( max.x - origin.x ) / dir.x;
		tmax = ( min.x - origin.x ) / dir.x;
	}

	if ( dir.y >= 0 )
	{
		tymin = ( min.y - origin.y ) / dir.y;
		tymax = ( max.y - origin.y ) / dir.y;
	}
	else
	{
		tymin = ( max.y - origin.y ) / dir.y;
		tymax = ( min.y - origin.y ) / dir.y;
	}

	if ( tmin > tymax || tymin > tmax )
		return false;

	if ( tymin > tmin )
		tmin = tymin;

	if ( tymax < tmax )
		tmax = tymax;

	if ( dir.z >= 0 )
	{
		tzmin = ( min.z - origin.z ) / dir.z;
		tzmax = ( max.z - origin.z ) / dir.z;
	}
	else
	{
		tzmin = ( max.z - origin.z ) / dir.z;
		tzmax = ( min.z - origin.z ) / dir.z;
	}

	if ( tmin > tzmax || tzmin > tmax )
		return false;

	if ( tmin < 0 || tmax < 0 )
		return false;

	return true;
}

bool Math::IntersectLineWithBB( Vector& vecStart, Vector& vecEnd, Vector& vecMin, Vector& vecMax )
{
	char quadrant[ 3 ];
	int i;

	Vector candidatePlane;
	for ( i = 0; i < 3; i++ )
	{
		if ( vecStart[ i ] < vecMin[ i ] )
		{
			quadrant[ i ] = 1;
			candidatePlane[ i ] = vecMin[ i ];
		}
		else if ( vecStart[ i ] > vecMax[ i ] )
		{
			quadrant[ i ] = 0;
			candidatePlane[ i ] = vecMax[ i ];
		}
		else
		{
			quadrant[ i ] = 2;
		}
	}

	Vector maxT;
	for ( i = 0; i < 3; i++ )
	{
		if ( quadrant[ i ] != 2 && vecEnd[ i ] != 0.f )
			maxT[ i ] = ( candidatePlane[ i ] - vecStart[ i ] ) / vecEnd[ i ];
		else
			maxT[ i ] = -1.f;
	}

	int whichPlane = 0;
	for ( i = 1; i < 3; i++ )
	{
		if ( maxT[ whichPlane ] < maxT[ i ] )
			whichPlane = i;
	}

	if ( maxT[ whichPlane ] < 0.0f )
		return false;

	for ( i = 0; i < 3; i++ )
	{
		if ( whichPlane != i )
		{
			float temp = vecStart[ i ] + maxT[ whichPlane ] * vecEnd[ i ];
			if ( temp < vecMin[ i ] || temp > vecMax[ i ] )
			{
				return false;
			}
		}
	}

	return true;
}

bool Math::IntersectionBoundingBox( Vector vecRayStart, CHitBox* pHitBox, Vector vecDirection, Matrix3x4_t& matrix )
{

	Vector ray_trans, dir_trans;
	Math::VectorTransform( vecRayStart, matrix, ray_trans );
	Math::VectorRotate( vecDirection, matrix, dir_trans );

	return IntersectRayWithAABB( ray_trans, dir_trans, pHitBox->m_vecMinBounds, pHitBox->m_vecMaxBounds );
}
bool Math::IntersectSegmentToCapsule(Vector vStart, Vector vEnd, Vector vMin, Vector vMax, float flRadius, std::optional<std::reference_wrapper<float>> flDistance) {
	static auto constexpr epsilon = std::numeric_limits<float>::epsilon();

	auto u = vEnd - vStart;
	auto v = vMax - vMin;
	const auto w = vStart - vMin;

	const auto a = u.DotProduct(u);
	const auto b = u.DotProduct(v);
	const auto c = v.DotProduct(v);
	const auto d = u.DotProduct(w);
	const auto e = v.DotProduct(w);
	const auto D = a * c - b * b;
	float sN, sD = D;
	float tN, tD = D;

	if (D < epsilon) {
		sN = 0.0;
		sD = 1.0;
		tN = e;
		tD = c;
	}
	else {
		sN = b * e - c * d;
		tN = a * e - b * d;

		if (sN < 0.0) {
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0) {
		tN = 0.0;

		if (-d < 0.0)
			sN = 0.0;
		else if (-d > a)
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {
		tN = tD;

		if (-d + b < 0.0)
			sN = 0;
		else if (-d + b > a)
			sN = sD;
		else {
			sN = -d + b;
			sD = a;
		}
	}

	const float sc = abs(sN) < epsilon ? 0.0 : sN / sD;
	const float tc = abs(tN) < epsilon ? 0.0 : tN / tD;

	auto dP = w + u * sc - v * tc;
	float flDot = dP.DotProduct(dP);
	float flDist{};
	sqrt_fast_new(&flDist, &flDot);

	if (flDist < flRadius) {
		if (flDistance.has_value()) {
			Vector vIntersect = vStart + u * sc;
			flDistance.value().get() = vStart.DistTo(vIntersect);
		}
		return true;
	}

	return false;
}

bool Math::IntersectRayWithAABBNew(Vector& vOrigin, Vector& vDirection, Vector& vMin, Vector& vMax, std::optional<std::reference_wrapper<float>> flDistance) {
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	if (vDirection.x >= 0) {
		tmin = (vMin.x - vOrigin.x) / vDirection.x;
		tmax = (vMax.x - vOrigin.x) / vDirection.x;
	}
	else {
		tmin = (vMax.x - vOrigin.x) / vDirection.x;
		tmax = (vMin.x - vOrigin.x) / vDirection.x;
	}

	if (vDirection.y >= 0) {
		tymin = (vMin.y - vOrigin.y) / vDirection.y;
		tymax = (vMax.y - vOrigin.y) / vDirection.y;
	}
	else {
		tymin = (vMax.y - vOrigin.y) / vDirection.y;
		tymax = (vMin.y - vOrigin.y) / vDirection.y;
	}

	if (tmin > tymax || tymin > tmax)
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	if (vDirection.z >= 0) {
		tzmin = (vMin.z - vOrigin.z) / vDirection.z;
		tzmax = (vMax.z - vOrigin.z) / vDirection.z;
	}
	else {
		tzmin = (vMax.z - vOrigin.z) / vDirection.z;
		tzmax = (vMin.z - vOrigin.z) / vDirection.z;
	}

	if (tmin > tzmax || tzmin > tmax)
		return false;

	if (tmin < 0 || tmax < 0)
		return false;

	if (flDistance.has_value()) {
		flDistance.value().get() = tmin;
	}

	return true;
}