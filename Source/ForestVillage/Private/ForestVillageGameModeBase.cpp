// Fill out your copyright notice in the Description page of Project Settings.

#include "ForestVillageGameModeBase.h"
#include "MainWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h" // 🔑 시설 검색을 위해 추가
#include "VillageFacility.h"       // 🔑 시설 복구 함수 호출을 위해 추가

void AForestVillageGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (MainWidgetClass)
	{
		CurrentWidget = CreateWidget<UMainWidget>(GetWorld(), MainWidgetClass);
		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
			RefreshHUD();
		}
	}
}

void AForestVillageGameModeBase::AddResource(EResourceType Type, int32 Amount)
{
	switch (Type)
	{
	case EResourceType::Apple:   AppleCount = FMath::Max(0, AppleCount + Amount); break;
	case EResourceType::Wood:    WoodCount = FMath::Max(0, WoodCount + Amount); break;
	case EResourceType::Orange:  OrangeCount = FMath::Max(0, OrangeCount + Amount); break;
	case EResourceType::Stone:   StoneCount = FMath::Max(0, StoneCount + Amount); break;
	case EResourceType::Ruby:    RubyCount = FMath::Max(0, RubyCount + Amount); break;
	case EResourceType::Diamond: DiamondCount = FMath::Max(0, DiamondCount + Amount); break;
	case EResourceType::Lapis:   LapisCount = FMath::Max(0, LapisCount + Amount); break;
	}

	RefreshHUD();
}

bool AForestVillageGameModeBase::SpendResource(EResourceType Type, int32 Amount)
{
	if (GetResourceCount(Type) < Amount) return false;

	AddResource(Type, -Amount);
	return true;
}

int32 AForestVillageGameModeBase::GetResourceCount(EResourceType Type) const
{
	switch (Type)
	{
	case EResourceType::Apple:   return AppleCount;
	case EResourceType::Wood:    return WoodCount;
	case EResourceType::Orange:  return OrangeCount;
	case EResourceType::Stone:   return StoneCount;
	case EResourceType::Ruby:    return RubyCount;
	case EResourceType::Diamond: return DiamondCount;
	case EResourceType::Lapis:   return LapisCount;
	default:                     return 0;
	}
}

// 💡 퀘스트 완료 가능 여부 확인 (QuestWidget 등에서 버튼 활성화 판단용)
bool AForestVillageGameModeBase::CanCompleteCurrentQuest() const
{
	switch (CurrentQuestIndex)
	{
	case 0: return (WoodCount >= 5);
	case 1: return (WoodCount >= 3 && AppleCount >= 1 && RubyCount >= 1);
	case 2: return (StoneCount >= 10 && WoodCount >= 5 && LapisCount >= 1 && DiamondCount >= 1);
	default: return false;
	}
}

// 💡 UI 버튼 클릭 시 실행될 실제 채집/소모 로직
bool AForestVillageGameModeBase::TryCompleteCurrentQuest()
{
	// 1. 쿨타임 중인지 확인 (연속 완료 방지)
	if (bIsQuestOnCooldown)
	{
		UE_LOG(LogTemp, Warning, TEXT("퀘스트 완료 쿨타임 중입니다. 잠시 후 다시 시도하세요."));
		return false;
	}

	if (!CanCompleteCurrentQuest()) return false;

	// 2. 쿨타임 시작 (3초)
	bIsQuestOnCooldown = true;
	GetWorld()->GetTimerManager().SetTimer(QuestCooldownTimerHandle, this, &AForestVillageGameModeBase::ResetQuestCooldown, 3.0f, false);

	// 3. 현재 퀘스트에 맞는 자원 소모
	switch (CurrentQuestIndex)
	{
	case 0:
		SpendResource(EResourceType::Wood, 5);
		SpendResource(EResourceType::Diamond, 1);
		break;
	case 1:
		SpendResource(EResourceType::Wood, 3);
		SpendResource(EResourceType::Apple, 5);
		SpendResource(EResourceType::Ruby, 1);
		break;
	case 2:
		SpendResource(EResourceType::Stone, 10);
		SpendResource(EResourceType::Wood, 5);
		SpendResource(EResourceType::Lapis, 1);
		SpendResource(EResourceType::Diamond, 1);
		break;
	}

	// 4. 통합 완료 처리 함수 호출
	CompleteQuest();

	return true;
}

void AForestVillageGameModeBase::ResetQuestCooldown()
{
	bIsQuestOnCooldown = false;
	UE_LOG(LogTemp, Log, TEXT("퀘스트 완료 쿨타임이 해제되었습니다."));
}

void AForestVillageGameModeBase::CompleteQuest()
{
	// 방금 완료한 퀘스트 인덱스를 임시 저장해둡니다.
	int32 CompletedQuestIndex = CurrentQuestIndex;

	CurrentQuestIndex++;
	UE_LOG(LogTemp, Warning, TEXT("퀘스트 완료! 다음 인덱스: %d"), CurrentQuestIndex);

	// --- 🔑 핵심 추가: 월드의 시설물 복구 트리거 연동 ---
	TArray<AActor*> Facilities;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVillageFacility::StaticClass(), Facilities);

	for (AActor* Actor : Facilities)
	{
		AVillageFacility* Facility = Cast<AVillageFacility>(Actor);
		// 월드에 배치된 시설 중, 방금 완료한 퀘스트 인덱스와 일치하는 시설을 찾음
		if (Facility && Facility->LinkedQuestIndex == CompletedQuestIndex)
		{
			Facility->TriggerRestoration(); // 메쉬 교체 및 파티클 재생!
		}
	}
	// ----------------------------------------------------

	// 엔딩 체크 (퀘스트 3 완료 후)
	if (CurrentQuestIndex > 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("모든 퀘스트 완료! 엔딩 시퀀스를 준비합니다."));
	}

	RefreshHUD();
}

void AForestVillageGameModeBase::RefreshHUD()
{
	if (CurrentWidget)
	{
		// MainWidget의 업데이트 함수 호출
		CurrentWidget->UpdateResourceUI(
			AppleCount, WoodCount, OrangeCount, StoneCount, DiamondCount, RubyCount, LapisCount
		);
	}
}
