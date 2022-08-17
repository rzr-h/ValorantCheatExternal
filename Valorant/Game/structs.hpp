#pragma once
#include <Windows.h>
#include <map>
#include <d3d9types.h>
#include <vector>
#include "../Driver/driver.hpp"

#define M_PI 3.14159265358979323846264338327950288419716939937510

namespace UE4Structs
{
	typedef struct _ValEntity {
		uint64_t Actor;
	}ValEntity;
	std::vector<ValEntity> ValList;

	class FVector
	{
	public:
		FVector() : x(0.f), y(0.f), z(0.f)
		{

		}

		FVector(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
		{

		}
		~FVector()
		{

		}

		float x;
		float y;
		float z;

		inline float Dot(FVector v)
		{
			return x * v.x + y * v.y + z * v.z;
		}

		inline float Distance(FVector v)
		{
			float x = this->x - v.x;
			float y = this->y - v.y;
			float z = this->z - v.z;

			return sqrtf((x * x) + (y * y) + (z * z)) * 0.03048f;
		}

		FVector operator+(FVector v)
		{
			return FVector(x + v.x, y + v.y, z + v.z);
		}

		FVector operator-(FVector v)
		{
			return FVector(x - v.x, y - v.y, z - v.z);
		}

		FVector operator*(float number) const {
			return FVector(x * number, y * number, z * number);
		}

		__forceinline float Magnitude() const {
			return sqrtf(x * x + y * y + z * z);
		}

		inline float Length()
		{
			return sqrtf((x * x) + (y * y) + (z * z));
		}

		__forceinline FVector Normalize() {
			FVector vector;
			float length = this->Magnitude();

			if (length != 0) {
				vector.x = x / length;
				vector.y = y / length;
				vector.z = z / length;
			}
			else {
				vector.x = vector.y = 0.0f;
				vector.z = 1.0f;
			}
			return vector;
		}

		__forceinline FVector& operator+=(const FVector& v) {
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}
	};

	D3DXMATRIX Matrix(FVector rot, FVector origin)
	{
		float radPitch = (rot.x * float(M_PI) / 180.f);
		float radYaw = (rot.y * float(M_PI) / 180.f);
		float radRoll = (rot.z * float(M_PI) / 180.f);

		float SP = sinf(radPitch);
		float CP = cosf(radPitch);
		float SY = sinf(radYaw);
		float CY = cosf(radYaw);
		float SR = sinf(radRoll);
		float CR = cosf(radRoll);

		D3DMATRIX matrix;
		matrix.m[0][0] = CP * CY;
		matrix.m[0][1] = CP * SY;
		matrix.m[0][2] = SP;
		matrix.m[0][3] = 0.f;

		matrix.m[1][0] = SR * SP * CY - CR * SY;
		matrix.m[1][1] = SR * SP * SY + CR * CY;
		matrix.m[1][2] = -SR * CP;
		matrix.m[1][3] = 0.f;

		matrix.m[2][0] = -(CR * SP * CY + SR * SY);
		matrix.m[2][1] = CY * SR - CR * SP * SY;
		matrix.m[2][2] = CR * CP;
		matrix.m[2][3] = 0.f;

		matrix.m[3][0] = origin.x;
		matrix.m[3][1] = origin.y;
		matrix.m[3][2] = origin.z;
		matrix.m[3][3] = 1.f;

		return matrix;
	}

	struct FQuat
	{
		float x;
		float y;
		float z;
		float w;
	};

	struct FTransform
	{
		FQuat rot;
		FVector translation;
		char pad[4];
		FVector scale;
		char pad1[4];

		D3DMATRIX ToMatrixWithScale()
		{
			D3DMATRIX m;
			m._41 = translation.x;
			m._42 = translation.y;
			m._43 = translation.z;

			float x2 = rot.x + rot.x;
			float y2 = rot.y + rot.y;
			float z2 = rot.z + rot.z;

			float xx2 = rot.x * x2;
			float yy2 = rot.y * y2;
			float zz2 = rot.z * z2;
			m._11 = (1.0f - (yy2 + zz2)) * scale.x;
			m._22 = (1.0f - (xx2 + zz2)) * scale.y;
			m._33 = (1.0f - (xx2 + yy2)) * scale.z;

			float yz2 = rot.y * z2;
			float wx2 = rot.w * x2;
			m._32 = (yz2 - wx2) * scale.z;
			m._23 = (yz2 + wx2) * scale.y;

			float xy2 = rot.x * y2;
			float wz2 = rot.w * z2;
			m._21 = (xy2 - wz2) * scale.y;
			m._12 = (xy2 + wz2) * scale.x;

			float xz2 = rot.x * z2;
			float wy2 = rot.w * y2;
			m._31 = (xz2 + wy2) * scale.z;
			m._13 = (xz2 - wy2) * scale.x;

			m._14 = 0.0f;
			m._24 = 0.0f;
			m._34 = 0.0f;
			m._44 = 1.0f;

			return m;
		}
	};

	D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
	{
		D3DMATRIX pOut;
		pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
		pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
		pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
		pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
		pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
		pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
		pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
		pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
		pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
		pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
		pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
		pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
		pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
		pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
		pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
		pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

		return pOut;
	}

	struct FMinimalViewInfo {
		FVector Location; // 0x00(0x0c)
		FVector Rotation; // 0x0c(0x0c)
		float FOV; // 0x18(0x04)
	};

	template<class T>
	class TArrayDrink {

	public:
		TArrayDrink() {
			Data = NULL;
			Count = 0;
			Max = 0;
		};

		T operator[](uint64_t i) const {
			return driver.read<uintptr_t>(((uintptr_t)Data) + i * sizeof(T));
		};

		T* Data;
		unsigned int Count;
		unsigned int Max;
	};

	template<class T>
	class TArray
	{
	public:
		int Length() const
		{
			return m_nCount;
		}

		bool IsValid() const
		{
			if (m_nCount > m_nMax)
				return false;
			if (!m_Data)
				return false;
			return true;
		}

		uint64_t GetAddress() const
		{
			return m_Data;
		}

		T GetById(int i)
		{
			return driver.readv<T>(m_Data + i * 8);
		}

		[[nodiscard]]
		std::vector<T> GetIter(int maxCount = 1000) {
			if (m_nCount > maxCount)
				return {};

			std::vector<T> buffer(m_nCount);
			driver.readarray<T>(m_Data, buffer.data(), m_nCount);
			return buffer;
		}

	protected:
		uint64_t m_Data;
		uint32_t m_nCount;
		uint32_t m_nMax;
	};
}
namespace ColorStructs
{

	typedef struct
	{
		DWORD R;
		DWORD G;
		DWORD B;
		DWORD A;
	}RGBA;

	class Color
	{
	public:
		RGBA red = { 255,0,0,255 };
		RGBA Magenta = { 255,0,255,255 };
		RGBA yellow = { 255,255,0,255 };
		RGBA grayblue = { 128,128,255,255 };
		RGBA green = { 128,224,0,255 };
		RGBA darkgreen = { 0,224,128,255 };
		RGBA brown = { 192,96,0,255 };
		RGBA pink = { 255,168,255,255 };
		RGBA DarkYellow = { 216,216,0,255 };
		RGBA SilverWhite = { 236,236,236,255 };
		RGBA purple = { 144,0,255,255 };
		RGBA Navy = { 88,48,224,255 };
		RGBA skyblue = { 0,136,255,255 };
		RGBA graygreen = { 128,160,128,255 };
		RGBA blue = { 0,96,192,255 };
		RGBA orange = { 255,128,0,255 };
		RGBA peachred = { 255,80,128,255 };
		RGBA reds = { 255,128,192,255 };
		RGBA darkgray = { 96,96,96,255 };
		RGBA Navys = { 0,0,128,255 };
		RGBA darkgreens = { 0,128,0,255 };
		RGBA darkblue = { 0,128,128,255 };
		RGBA redbrown = { 128,0,0,255 };
		RGBA purplered = { 128,0,128,255 };
		RGBA greens = { 25,255,25,140 };
		RGBA envy = { 0,255,255,255 };
		RGBA black = { 0,0,0,255 };
		RGBA neger = { 215, 240, 180, 255 };
		RGBA negernot = { 222, 180, 200, 255 };
		RGBA gray = { 128,128,128,255 };
		RGBA white = { 255,255,255,255 };
		RGBA blues = { 30,144,255,255 };
		RGBA lightblue = { 135,206,250,255 };
		RGBA Scarlet = { 220, 20, 60, 160 };
		RGBA white_ = { 255,255,255,200 };
		RGBA gray_ = { 128,128,128,200 };
		RGBA black_ = { 0,0,0,200 };
		RGBA red_ = { 255,0,0,200 };
		RGBA Magenta_ = { 255,0,255,200 };
		RGBA yellow_ = { 255,255,0,200 };
		RGBA grayblue_ = { 128,128,255,200 };
		RGBA green_ = { 128,224,0,200 };
		RGBA darkgreen_ = { 0,224,128,200 };
		RGBA brown_ = { 192,96,0,200 };
		RGBA pink_ = { 255,168,255,200 };
		RGBA darkyellow_ = { 216,216,0,200 };
		RGBA silverwhite_ = { 236,236,236,200 };
		RGBA purple_ = { 144,0,255,200 };
		RGBA Blue_ = { 88,48,224,200 };
		RGBA skyblue_ = { 0,136,255,200 };
		RGBA graygreen_ = { 128,160,128,200 };
		RGBA blue_ = { 0,96,192,200 };
		RGBA orange_ = { 255,128,0,200 };
		RGBA pinks_ = { 255,80,128,200 };
		RGBA Fuhong_ = { 255,128,192,200 };
		RGBA darkgray_ = { 96,96,96,200 };
		RGBA Navy_ = { 0,0,128,200 };
		RGBA darkgreens_ = { 0,128,0,200 };
		RGBA darkblue_ = { 0,128,128,200 };
		RGBA redbrown_ = { 128,0,0,200 };
		RGBA purplered_ = { 128,0,128,200 };
		RGBA greens_ = { 0,255,0,200 };
		RGBA envy_ = { 0,255,255,200 };

		RGBA glassblack = { 0, 0, 0, 160 };
		RGBA GlassBlue = { 65,105,225,80 };
		RGBA glassyellow = { 255,255,0,160 };
		RGBA glass = { 200,200,200,60 };

		RGBA filled = { 0, 0, 0, 150 };

		RGBA Plum = { 221,160,221,160 };

	};
	Color Col;
}