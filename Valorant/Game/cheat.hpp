#pragma once
#include "sdk.hpp"
#include <iostream>

using namespace Globals;
using namespace Camera;
using namespace UE4;

GWorld* UWorld;
GGameInstance* UGameInstance;
GLocalPlayer* ULocalPlayer;
GPlayerController* APlayerController;
GPawn* APawn;
GPrivatePawn* APrivatePawn;
GULevel* ULevel;
GUSkeletalMeshComponent* USkeletalMeshComponent;

bool cached = false;
uintptr_t WorldPtr;

auto CacheGame() -> void
{
	auto guardedregion = driver.guarded_region();

	while (true)
	{
		std::vector<ValEntity> CachedList;

		WorldPtr = GetWorld(guardedregion);

		auto ULevelPtr = UWorld->ULevel(WorldPtr);
		auto UGameInstancePtr = UWorld->GameInstance(WorldPtr);

		auto ULocalPlayerPtr = UGameInstance->ULocalPlayer(UGameInstancePtr);
		auto APlayerControllerPtr = ULocalPlayer->APlayerController(ULocalPlayerPtr);

		PlayerCameraManager = APlayerController->APlayerCameraManager(APlayerControllerPtr);
		auto MyHUD = APlayerController->AHUD(APlayerControllerPtr);

		auto APawnPtr = APlayerController->APawn(APlayerControllerPtr);

		if (APawnPtr != 0)
		{
			MyUniqueID = APawn->UniqueID(APawnPtr);
			MyRelativeLocation = APawn->RelativeLocation(APawnPtr);
		}

		if (MyHUD != 0)
		{
			auto PlayerArray = ULevel->AActorArray(ULevelPtr);

			for (uint32_t i = 0; i < PlayerArray.Count; ++i)
			{
				auto Pawns = PlayerArray[i];
				if (Pawns != APawnPtr)
				{
					if (MyUniqueID == APawn->UniqueID(Pawns))
					{
						ValEntity Entities{ Pawns };
						CachedList.push_back(Entities);
					}
				}
			}

			ValList.clear();
			ValList = CachedList;
			Sleep(1000);
		}
	}
}

auto CheatLoop() -> void
{
	for (ValEntity ValEntityList : ValList)
	{
		auto SkeletalMesh = APrivatePawn->USkeletalMeshComponent(ValEntityList.Actor);

		auto RelativeLocation = APawn->RelativeLocation(ValEntityList.Actor);
		auto RelativeLocationProjected = UE4::SDK::ProjectWorldToScreen(RelativeLocation);

		auto RelativePosition = RelativeLocation - CameraLocation;
		auto RelativeDistance = RelativePosition.Length() / 10000 * 2;

		auto HeadBone = UE4::SDK::GetEntityBone(SkeletalMesh, 8);
		auto HeadBoneProjected = UE4::SDK::ProjectWorldToScreen(HeadBone);

		auto RootBone = UE4::SDK::GetEntityBone(SkeletalMesh, 0);
		auto RootBoneProjected = UE4::SDK::ProjectWorldToScreen(RootBone);
		auto RootBoneProjected2 = UE4::SDK::ProjectWorldToScreen(FVector(RootBone.x, RootBone.y, RootBone.z - 15));

		auto Distance = MyRelativeLocation.Distance(RelativeLocation);

		float BoxHeight = abs(HeadBoneProjected.y - RootBoneProjected.y);
		float BoxWidth = BoxHeight * 0.40;

		auto ESPColor = ImColor(255, 255, 255);

		auto Health = APawn->Health(ValEntityList.Actor);
		if (Health <= 0) continue;

		if (APawn->bIsDormant(ValEntityList.Actor))
		{
			if (Settings::Visuals::bSnaplines)
				DrawTracers(RootBoneProjected, ESPColor);

			if (Settings::Visuals::bBox)
				Draw2DBox(RelativeLocationProjected, BoxWidth, BoxHeight, ESPColor);

			if (Settings::Visuals::bBoxOutlined)
				DrawOutlinedBox(RelativeLocationProjected, BoxWidth, BoxHeight, ESPColor);

			if (Settings::Visuals::bHealth)
				DrawHealthBar(RelativeLocationProjected, BoxWidth, BoxHeight, Health, RelativeDistance);

			if (Settings::Visuals::bDistance)
				DrawDistance(RootBoneProjected2, Distance);
		}
	}
}
