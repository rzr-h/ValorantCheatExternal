#pragma once

#include <Windows.h>
#include <map>
#include <d3d9types.h>
#include "../Overlay/render.hpp"
#include "../Driver/driver.hpp"
#include "structs.hpp"
#include <vector>

using namespace UE4Structs;

namespace Globals
{
	DWORD_PTR
		LocalPlayer,
		PlayerController,
		PlayerCameraManager;

	int MyUniqueID, MyTeamID, BoneCount;

	FVector MyRelativeLocation, closestPawn;

	namespace Camera
	{
		FVector CameraLocation, CameraRotation;
		float FovAngle;
	}
}

using namespace Globals;
using namespace Camera;


namespace UE4
{
	struct GWorld
	{
		uintptr_t GameInstance(uintptr_t GameWorld) {
			return driver.readv<uintptr_t>(GameWorld + offsets::Gameinstance);
		};

		uintptr_t ULevel(uintptr_t World) {
			return driver.read<uintptr_t>(World + offsets::Ulevel);
		};
	};

	struct GGameInstance {
		uintptr_t ULocalPlayer(uintptr_t UGameInstance) {
			auto ULocalPlayerArray = driver.readv<uintptr_t>(UGameInstance + offsets::LocalPlayers);
			return driver.readv<uintptr_t>(ULocalPlayerArray);
		};
	};

	struct GULevel {
		TArrayDrink<uintptr_t> AActorArray(uintptr_t ULevel) {
			return driver.readv<TArrayDrink<uintptr_t>>(ULevel + offsets::AActorArray);
		};
	};

	struct GPrivatePawn {
		uintptr_t USkeletalMeshComponent(uintptr_t Pawn) {
			return driver.read<uintptr_t>(Pawn + offsets::MeshComponent);
		};
	};

	struct GUSkeletalMeshComponent {
		int BoneCount(uintptr_t Mesh) {
			return driver.read<uintptr_t>(Mesh + offsets::BoneCount);
		};
	};

	struct GLocalPlayer {
		uintptr_t APlayerController(uintptr_t ULocalPlayer) {
			return driver.read<uintptr_t>(ULocalPlayer + offsets::PlayerController);
		};
	};

	struct GPlayerController {
		uintptr_t APlayerCameraManager(uintptr_t APlayerController) {
			return driver.read<uintptr_t>(APlayerController + offsets::PlayerCameraManager);
		};
		uintptr_t AHUD(uintptr_t APlayerController) {
			return driver.read<uintptr_t>(APlayerController + offsets::MyHUD);
		};
		uintptr_t APawn(uintptr_t APlayerController) {
			return driver.read<uintptr_t>(APlayerController + offsets::AcknowledgedPawn);
		};
	};

	struct GPawn {
		auto TeamID(uintptr_t APawn) -> int {
			auto PlayerState = driver.readv<uintptr_t>(APawn + offsets::PlayerState);
			auto TeamComponent = driver.readv<uintptr_t>(PlayerState + offsets::TeamComponent);
			return driver.readv<int>(TeamComponent + offsets::TeamID);
		};

		auto UniqueID(uintptr_t APawn) -> int {
			return driver.readv<int>(APawn + offsets::UniqueID);
		};

		auto FNameID(uintptr_t APawn) -> int {
			return driver.readv<int>(APawn + offsets::FNameID);
		};

		auto RelativeLocation(uintptr_t APawn) -> FVector {
			auto RootComponent = driver.readv<uintptr_t>(APawn + offsets::RootComponent);
			return driver.readv<FVector>(RootComponent + offsets::RelativeLocation);
		};

		auto bIsDormant(uintptr_t APawn) -> bool {
			return driver.readv<bool>(APawn + offsets::bIsDormant);
		};

		auto Health(uintptr_t APawn) -> float {
			auto DamageHandler = driver.read<uintptr_t>(APawn + offsets::DamageHandler);
			return driver.readv<float>(DamageHandler + offsets::Health);
		};
	};

	auto GetWorld(uintptr_t Pointer) -> uintptr_t
	{
		std::uintptr_t uworld_addr = driver.readv<uintptr_t>(Pointer + 0x50);

		unsigned long long uworld_offset;

		if (uworld_addr > 0x10000000000)
		{
			uworld_offset = uworld_addr - 0x10000000000;
		}
		else {
			uworld_offset = uworld_addr - 0x8000000000;
		}

		return Pointer + uworld_offset;
	}

	auto VectorToRotation(FVector relativeLocation) -> FVector
	{
		constexpr auto radToUnrRot = 57.2957795f;

		return FVector(
			atan2(relativeLocation.z, sqrt((relativeLocation.x * relativeLocation.x) + (relativeLocation.y * relativeLocation.y))) * radToUnrRot,
			atan2(relativeLocation.y, relativeLocation.x) * radToUnrRot,
			0.f);
	}

	auto AimAtVector(FVector targetLocation, FVector cameraLocation) -> FVector
	{
		return VectorToRotation(targetLocation - cameraLocation);
	}

	namespace SDK
	{
		auto GetEntityBone(DWORD_PTR mesh, int id) -> FVector
		{
			DWORD_PTR array = driver.readv<uintptr_t>(mesh + offsets::BoneArray);
			if (array == NULL)
				array = driver.readv<uintptr_t>(mesh + offsets::BoneArrayCache);

			FTransform bone = driver.readv<FTransform>(array + (id * 0x30));

			FTransform ComponentToWorld = driver.readv<FTransform>(mesh + offsets::ComponentToWorld);
			D3DMATRIX Matrix;

			Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

			return FVector(Matrix._41, Matrix._42, Matrix._43);
		}

		auto ProjectWorldToScreen(FVector WorldLocation) -> FVector
		{
			FVector Screenlocation = FVector(0, 0, 0);

			auto ViewInfo = driver.readv<FMinimalViewInfo>(PlayerCameraManager + 0x1fe0 + 0x10);

			CameraLocation = ViewInfo.Location;
			CameraRotation = ViewInfo.Rotation;


			D3DMATRIX tempMatrix = Matrix(CameraRotation, FVector(0, 0, 0));

			FVector vAxisX = FVector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]),
				vAxisY = FVector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]),
				vAxisZ = FVector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

			FVector vDelta = WorldLocation - CameraLocation;
			FVector vTransformed = FVector(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

			if (vTransformed.z < 1.f) vTransformed.z = 1.f;

			FovAngle = ViewInfo.FOV;

			float ScreenCenterX = Width / 2.0f;
			float ScreenCenterY = Height / 2.0f;

			Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
			Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;

			return Screenlocation;
		}

		bool IsVec3Valid(FVector vec3)
		{
			return !(vec3.x == 0 && vec3.y == 0 && vec3.z == 0);
		}
	}
}