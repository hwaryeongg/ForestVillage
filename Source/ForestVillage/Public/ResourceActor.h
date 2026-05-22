// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResourceActor.generated.h"

UCLASS()
class FORESTVILLAGE_API AResourceActor : public AActor
{
	GENERATED_BODY()
    
public: 
	// 생성자
	AResourceActor();

protected:
	virtual void BeginPlay() override;

public: 
	// --- 1. 컴포넌트 (GEMINI.md 규칙: 전방 선언 적용) ---
	UPROPERTY(EditAnywhere, Category = "Components")
	class UBoxComponent* BoxComp; // 플레이어 접근을 감지할 충돌체

	UPROPERTY(EditAnywhere, Category = "Components")
	class UStaticMeshComponent* MeshComp; // 나무나 바위의 외형

	// --- 2. 핵심 변수 ---
	UPROPERTY(EditAnywhere, Category = "Resource")
	float Durability = 1.0f; // 기본 내구도 (1번 맞으면 파괴)

	UPROPERTY(EditAnywhere, Category = "Resource")
	TSubclassOf<class AActor> ItemFactory; // 파괴 시 떨어뜨릴 아이템 클래스 지정 (사과/오렌지 등)

	UPROPERTY(EditAnywhere, Category = "Resource")
	TSubclassOf<class AActor> WoodFactory; // 함께 떨어뜨릴 나무(Wood) 아이템 클래스 지정

	// --- 2-1. 바위 관련 설정 ---
	UPROPERTY(EditAnywhere, Category = "Resource")
	bool bIsRockNode = false;

	UPROPERTY(EditAnywhere, Category = "Resource")
	TSubclassOf<class AActor> StoneFactory;

	UPROPERTY(EditAnywhere, Category = "Resource")
	TSubclassOf<class AActor> RubyFactory;

	UPROPERTY(EditAnywhere, Category = "Resource")
	TSubclassOf<class AActor> DiamondFactory;

	UPROPERTY(EditAnywhere, Category = "Resource")
	TSubclassOf<class AActor> LapisFactory;

	// --- 3. 함수 ---
	// 채집을 당할 때 호출되는 함수
	void Gather(float Power);

	// 자원(아이템)을 생성하는 함수
	void DropResource();
};