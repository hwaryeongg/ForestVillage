// Fill out your copyright notice in the Description page of Project Settings.


#include "ForestVillageGameModeBase.h"
#include "MainWidget.h" // 위젯 함수 호출을 위해 필요
#include "Blueprint/UserWidget.h" // CreateWidget 기능을 위해 필요

void AForestVillageGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// 1. 위젯 클래스가 에디터(블루프린트)에서 설정되어 있는지 확인
	if (MainWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode: 위젯 클래스 발견! 생성을 시작합니다."));

		// 2. 위젯 생성 및 인스턴스 저장
		CurrentWidget = CreateWidget<UMainWidget>(GetWorld(), MainWidgetClass);
		
		if (CurrentWidget)
		{
			// 3. 화면(Viewport)에 추가
			CurrentWidget->AddToViewport();
			UE_LOG(LogTemp, Warning, TEXT("GameMode: 위젯이 뷰포트에 추가되었습니다."));
			
			// 4. 초기 자원 수치(0)를 UI에 반영
			RefreshHUD();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GameMode: 위젯 인스턴스 생성 실패!"));
		}
	}
	else
	{
		// 이 로그가 뜬다면 에디터의 GameMode 블루프린트에서 MainWidgetClass 칸이 None인 상태입니다.
		UE_LOG(LogTemp, Error, TEXT("GameMode: MainWidgetClass가 설정되지 않았습니다! (에디터 확인 필요)"));
	}
}
void AForestVillageGameModeBase::AddResource(EResourceType Type, int32 Amount)
{
	// 타입에 따라 해당하는 카운터 증가
	switch (Type)
	{
	case EResourceType::Apple:   AppleCount += Amount;   break;
	case EResourceType::Wood:    WoodCount += Amount;    break;
	case EResourceType::Orange:  OrangeCount += Amount;  break;
	case EResourceType::Stone:   StoneCount += Amount;   break;
	case EResourceType::Ruby:    RubyCount += Amount;    break;
	case EResourceType::Diamond: DiamondCount += Amount; break;
	case EResourceType::Lapis:   LapisCount += Amount;   break;
	}

	// 데이터 무결성 확인을 위한 로그
	UE_LOG(LogTemp, Warning, TEXT("자원 획득! [현재 수량] Apple:%d, Wood:%d, Orange:%d, Stone:%d, Ruby:%d, Diamond:%d, Lapis:%d"), 
		AppleCount, WoodCount, OrangeCount, StoneCount, RubyCount, DiamondCount, LapisCount);

	// 데이터가 변했으니 UI를 새로고침합니다.
	RefreshHUD();
}

bool AForestVillageGameModeBase::SpendResource(EResourceType Type, int32 Amount)
{
	// 소지량 확인
	if (GetResourceCount(Type) < Amount) return false;

	// 타입에 따라 차감
	switch (Type)
	{
	case EResourceType::Apple:   AppleCount -= Amount;   break;
	case EResourceType::Wood:    WoodCount -= Amount;    break;
	case EResourceType::Orange:  OrangeCount -= Amount;  break;
	case EResourceType::Stone:   StoneCount -= Amount;   break;
	case EResourceType::Ruby:    RubyCount -= Amount;    break;
	case EResourceType::Diamond: DiamondCount -= Amount; break;
	case EResourceType::Lapis:   LapisCount -= Amount;   break;
	}

	RefreshHUD();
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

void AForestVillageGameModeBase::CompleteQuest()
{
	CurrentQuestIndex++;
	UE_LOG(LogTemp, Warning, TEXT("퀘스트 완료! 다음 인덱스: %d"), CurrentQuestIndex);

	// 엔딩 체크 (퀘스트 3 완료 후)
	if (CurrentQuestIndex > 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("모든 퀘스트 완료! 엔딩 시퀀스를 준비합니다."));
		// 추후 엔딩 처리 로직 추가 가능
	}

	RefreshHUD();
}

void AForestVillageGameModeBase::RefreshHUD()
{
	// 위젯 인스턴스가 유효하다면 데이터를 전달하여 UI 갱신
	// (참고: MainWidget은 상시 HUD에서 인벤토리 정보가 빠지므로, QuestWidget 등에서 이를 참조하게 될 것입니다.)
	if (CurrentWidget)
	{
		// 기존 UI가 있다면 업데이트 (파라미터가 3개뿐이므로 일단 유지)
		CurrentWidget->UpdateResourceUI(AppleCount, WoodCount, OrangeCount);
	}
}

