// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ForestVillageGameModeBase.h" // 자원 타입(EResourceType) 사용을 위해 포함
#include "ItemActor.generated.h"

UCLASS()
class FORESTVILLAGE_API AItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// 생성자
	AItemActor();

protected:
	// 게임 시작 시 호출
	virtual void BeginPlay() override;

public:
	// --- 컴포넌트 (GEMINI.md 규칙: 전방 선언 사용) ---
	UPROPERTY(EditAnywhere, Category = "Components")
	class USphereComponent* SphereComp; // 플레이어 충돌 감지용

	UPROPERTY(EditAnywhere, Category = "Components")
	class UStaticMeshComponent* MeshComp; // 아이템 외형 (사과, 나무 등)

	// --- 아이템 설정 ---
	UPROPERTY(EditAnywhere, Category = "Item Settings")
	EResourceType ItemType; // 이 아이템이 어떤 자원인지 설정 (사과/나무/오렌지)

	UPROPERTY(EditAnywhere, Category = "Item Settings")
	int32 Amount = 1; // 한 번에 추가될 수량

	// 충돌 시 호출될 함수 (델리게이트용)
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 아이템을 획득 처리하는 함수 (중복 코드 방지를 위해 분리)
	void Collect();
};
