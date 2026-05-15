// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ForestVillageGameModeBase.generated.h"

/**
 * 
 */
// 1. 자원 종류 정의 (사과, 나무, 오렌지)
UENUM(BlueprintType)
enum class EResourceType : uint8
{
	Apple   UMETA(DisplayName = "Apple"),
	Wood    UMETA(DisplayName = "Wood"),
	Orange  UMETA(DisplayName = "Orange")
};

UCLASS()
class FORESTVILLAGE_API AForestVillageGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	// 자원을 추가하는 공개 함수 (아이템 획득 시 호출)
	void AddResource(EResourceType Type, int32 Amount);

	// 특정 자원의 현재 개수를 반환하는 함수 (UI 표시용)
	UFUNCTION(BlueprintCallable, Category = "VillageData")
	int32 GetResourceCount(EResourceType Type) const;

protected:
	// 게임 시작 시 호출 (UI 생성용)
	virtual void BeginPlay() override;

public:
	// --- UI 설정 ---
	// 에디터에서 할당할 메인 위젯 클래스 (BP_MainWidget 등)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UMainWidget> MainWidgetClass;

	// 생성된 위젯 인스턴스를 보관 (가비지 컬렉션 방지를 위해 UPROPERTY 사용)
	UPROPERTY()
	class UMainWidget* CurrentWidget;

private:
	// GEMINI.md 규칙: 자원 데이터는 private으로 관리하여 캡슐화 유지
	int32 AppleCount = 0;
	int32 WoodCount = 0;
	int32 OrangeCount = 0;

	// UI 갱신 로직 (위젯의 함수를 호출하여 화면 업데이트)
	void RefreshHUD();
};
