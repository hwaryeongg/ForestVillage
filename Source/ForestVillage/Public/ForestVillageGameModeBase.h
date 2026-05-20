// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ForestVillageGameModeBase.generated.h"

/**
 * 
 */
// 1. 자원 종류 정의 (사과, 나무, 오렌지 + 돌, 루비, 다이아몬드, 라피스 라즐리)
UENUM(BlueprintType)
enum class EResourceType : uint8
{
	Apple   UMETA(DisplayName = "Apple"),
	Wood    UMETA(DisplayName = "Wood"),
	Orange  UMETA(DisplayName = "Orange"),
	Stone   UMETA(DisplayName = "Stone"),
	Ruby    UMETA(DisplayName = "Ruby"),
	Diamond UMETA(DisplayName = "Diamond"),
	Lapis   UMETA(DisplayName = "Lapis")
};

UCLASS()
class FORESTVILLAGE_API AForestVillageGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	// 자원을 추가하는 공개 함수 (아이템 획득 시 호출)
	void AddResource(EResourceType Type, int32 Amount);

	// 특정 자원을 소모하는 함수
	bool SpendResource(EResourceType Type, int32 Amount);

	// 특정 자원의 현재 개수를 반환하는 함수 (UI 표시용)
	UFUNCTION(BlueprintCallable, Category = "VillageData")
	int32 GetResourceCount(EResourceType Type) const;

	// 현재 퀘스트 인덱스 반환
	UFUNCTION(BlueprintCallable, Category = "VillageData")
	int32 GetCurrentQuestIndex() const { return CurrentQuestIndex; }

	// 퀘스트 완료 처리
	void CompleteQuest();

protected:
	// 게임 시작 시 호출 (UI 생성용)
	virtual void BeginPlay() override;

public:
	// --- UI 설정 ---
	// 에디터에서 할당할 메인 위젯 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UMainWidget> MainWidgetClass;

	// 생성된 위젯 인스턴스를 보관
	UPROPERTY()
	class UMainWidget* CurrentWidget;

private:
	// GEMINI.md 규칙: 자원 데이터는 private으로 관리하여 캡슐화 유지
	int32 AppleCount = 0;
	int32 WoodCount = 0;
	int32 OrangeCount = 0;
	int32 StoneCount = 0;
	int32 RubyCount = 0;
	int32 DiamondCount = 0;
	int32 LapisCount = 0;

	// 현재 진행 중인 퀘스트 인덱스 (0: 마을 입구, 1: 요리솥, 2: 우물)
	int32 CurrentQuestIndex = 0;

	// UI 갱신 로직
	void RefreshHUD();
};
