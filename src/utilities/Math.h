
#pragma once

#define M_HPI			DirectX::XM_PIDIV2
#define M_QPI			DirectX::XM_PIDIV4
#define M_PI			DirectX::XM_PI
#define M_2PI			DirectX::XM_2PI
#define M_GPI			1.6180339887498f
#define M_RADPI			57.295779513082f

#define M_METRE2INCH( x )	( ( x ) / 0.0254f )
#define M_INCH2METRE( x )	( ( x ) * 0.0254f )
#define M_METRE2FOOT( x )	( ( x ) * 3.28f )
#define M_FOOT2METRE( x )	( ( x ) / 3.28f )
#define M_RAD2DEG( x )		DirectX::XMConvertToDegrees( x )
#define M_DEG2RAD( x )		DirectX::XMConvertToRadians( x )

class QAngle;
class Vector;
class CHitBox;
class Vector2D;
struct Quaternion;
struct Matrix3x4_t;

namespace Math
{
	bool Setup( );

	Vector ExtrapolateTick( const Vector& p0, const Vector& v0, const int iTicks = 1);

	void NormalizeAngles( QAngle& angAngles );
	QAngle NormalizeAnglesReturn(QAngle angle);
	Vector NormalizeAnglesReturn(Vector angle);

	Vector CrossProduct(const Vector& a, const Vector& b);

	void VectorAngles( const Vector& vecForward, QAngle& angView );
	void VectorAngles(const Vector& vecForward, Vector& vecUp, QAngle& angAngles);

	void AngleVector(const QAngle& angAngles, Vector& vecForward);
	void AngleVectors(const QAngle& angAngles, Vector& vecForward, Vector& vecRight, Vector& vecUp);

	float VectorDistance( Vector vecSource, Vector vecDestination );

	Vector2D AnglePixels( const float flSensitivity, const float flPitch, const float flYaw, const QAngle& angBegin, const QAngle& angEnd );

	void SmoothAngle( QAngle angSource, QAngle& angDestination, float flFactor );
	float GetFOV( const QAngle& angViewAngle, const QAngle& angAimAngle );
	QAngle CalcAngle( const Vector& vecStart, const Vector& vecEnd );
	Vector CalculateCameraPosition(Vector anchorPos, float distance, QAngle viewAngles);

	Vector VecAngle(Vector vec);
	float FloatNormalize(float flValue);

	Vector VectorTransform(const Vector& vecTransform, const Matrix3x4_t& matrix);

	Quaternion AngleQuaternion(const QAngle& angAngles);

	void SinCos(float radians, float* sine, float* cosine);
	void AxisAngleQuaternion(const Vector& axis, float angle, Quaternion& q);
	void CrossProduct(const Vector& a, const Vector& b, Vector& result);
	Vector& QuaternionImaginaryPart(const Quaternion& q);
	float QuaternionRealPart(const Quaternion& q);
	void VectorMA(const Vector& start, float scale, const Vector& direction, Vector& dest);
	void QuaternionMultiply(const Quaternion& q, const Vector& v, Vector& result);

	QAngle VectorToAngle(const Vector& vecForward);

	void MatrixCopy(const Matrix3x4_t& in, Matrix3x4_t& out);

	void ConcatTransforms(const Matrix3x4_t& in1, const Matrix3x4_t& in2, Matrix3x4_t& out);

	float DotProduct(const float* v1, const float* v2);

	void VectorRotate(const float* __restrict in1, const Matrix3x4_t& in2, float* __restrict out);

	void VectorRotate(const Vector& in1, const Matrix3x4_t& in2, Vector& out);

	void MatrixSetColumn2(const Vector& in, int column, Matrix3x4_t& out);

	Vector CrossProduct(const Vector& a, const Vector& b);

	void CrossProduct(const Vector& a, const Vector& b, Vector& result);

	float VectorNormalize(Vector& vec);

	void VectorVectors(const Vector& forward, Vector& right, Vector& up);

	void VectorMatrix(const Vector& forward, Matrix3x4_t& matrix);

	void VectorTransform(const float* __restrict in1, const Matrix3x4_t& in2, float* __restrict out);

	void VectorTransform(const Vector& in1, const Matrix3x4_t& in2, Vector& out);

	void MatrixMultiply(const Matrix3x4_t& in1, const Matrix3x4_t& in2, Matrix3x4_t& out);

	void AngleMatrix(const QAngle& angles, Matrix3x4_t& matrix);

	void MatrixAngles(const Matrix3x4_t& matrix, QAngle& angles);

	void MatrixPosition(const Matrix3x4_t& matrix, Vector& position);

	void MatrixAngles(const Matrix3x4_t& matrix, QAngle& angles, Vector& position);

	Matrix3x4_t CreateRotationMatrixFromVector(const Vector& vecDirection);
	Vector RotateVectorByMatrix(const Vector& vecIn, const Matrix3x4_t& matRot);

	bool IntersectSegmentToSegment( Vector vecStart, Vector vecEnd, Vector vecMin, Vector vecMax, float flRadius );

	bool IntersectRayWithAABB( Vector& origin, Vector& dir, Vector& min, Vector& max );
	bool IntersectLineWithBB( Vector& vecStart, Vector& vecEnd, Vector& vecMin, Vector& vecMax );

	bool IntersectionBoundingBox( Vector vecRayStart, CHitBox* pHitBox, Vector vecDirection, Matrix3x4_t& matrix );

	bool IntersectSegmentToCapsule(Vector vStart, Vector vEnd, Vector vMin, Vector vMax, float flRadius, std::optional<std::reference_wrapper<float>> flDistance = std::nullopt);
	bool IntersectRayWithAABBNew(Vector& vOrigin, Vector& vDirection, Vector& vMin, Vector& vMax, std::optional<std::reference_wrapper<float>> flDistance = std::nullopt);

	template < typename t >
	t Interpolate(t from, t to, const float percent) {
		return to * percent + from * (1.f - percent);
	}

	template < typename t >
	t Lerp(const t& t1, const t& t2, float progress)
	{
		return t1 + (t2 - t1) * progress;
	}

	template < typename t >
	t Maximum(const t& a, const t& b) {
		return (((a) > (b)) ? (a) : (b));
	}

	template < typename t >
	t Minimum(const t& a, const t& b) {
		return (((a) < (b)) ? (a) : (b));
	}

	template < typename t >
	void Clamp(t& n, const t& lower, const t& upper) {
		n = Maximum(lower, Minimum(n, upper));
	}

	inline void sqrt_fast_new(float* __restrict pOut, float* __restrict pIn) {
		_mm_store_ss(pOut, _mm_sqrt_ss(_mm_load_ss(pIn)));
	}

	inline int( __cdecl* fnRandomSeed )( int iSeed ) = nullptr;
	inline float( __cdecl* fnRandomFloat )( float flMinValue, float flMaxValue ) = nullptr;
	inline float( __cdecl* fnRandomFloatExp )( float flMinValue, float flMaxValue, float flExponent ) = nullptr;
	inline int( __cdecl* fnRandomInt )( int iMinValue, int iMaxValue ) = nullptr;
	inline float( __cdecl* fnRandomGaussianFloat )( float flMean, float flStdDev ) = nullptr;

}