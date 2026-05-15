// Fill out your copyright notice in the Description page of Project Settings.


#include "ForestVillageGameModeBase.h"

void AForestVillageGameModeBase::AddResource(EResourceType Type, int32 Amount)
{
	// 타입에 따라 해당하는 카운터 증가
	switch (Type)
	{
	case EResourceType::Apple:
		AppleCount += Amount;
		break;
	case EResourceType::Wood:
		WoodCount += Amount;
		break;
	case EResourceType::Orange:
		OrangeCount += Amount;
		break;
	}

	// 데이터가 잘 들어왔는지 로그로 확인 (Phase 3 요구사항)
	UE_LOG(LogTemp, Warning, TEXT("자원 획득! [Type: %d] 현재 수량 - Apple: %d, Wood: %d, Orange: %d"), 
		static_cast<int32>(Type), AppleCount, WoodCount, OrangeCount);

	// 데이터가 변했으니 UI를 새로고침합니다.
	RefreshHUD();
}

int32 AForestVillageGameModeBase::GetResourceCount(EResourceType Type) const
{
	switch (Type)
	{
	case EResourceType::Apple:  return AppleCount;
	case EResourceType::Wood:   return WoodCount;
	case EResourceType::Orange: return OrangeCount;
	default:                    return 0;
	}
}

void AForestVillageGameModeBase::RefreshHUD()
{
	// TODO: 위젯 클래스 구현 후 여기에 UI 갱신 코드 추가 예정
}

