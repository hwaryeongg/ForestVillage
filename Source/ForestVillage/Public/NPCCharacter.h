// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPCCharacter.generated.h"

/**
 * 마을 NPC를 담당하는 캐릭터 클래스
 */
UCLASS()
class FORESTVILLAGE_API ANPCCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// 생성자
	ANPCCharacter();

protected:
	// 게임 시작 시 호출
	virtual void BeginPlay() override;

public:
	// --- 컴포넌트 ---
	UPROPERTY(EditAnywhere, Category = "Components")
	class UBoxComponent* InteractionArea; // 플레이어 접근 감지용 박스

	// --- UI 설정 ---
	// 에디터에서 할당할 대화 위젯 클래스 (WBP_Dialogue)
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> DialogueWidgetClass;

	/** 플레이어와 상호작용 시 호출될 함수 */
	void OnInteract();

private:
	// 생성된 대화 위젯 인스턴스
	UPROPERTY()
	class UUserWidget* CurrentDialogueUI;
};
