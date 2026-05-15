// Fill out your copyright notice in the Description page of Project Settings.

#include "NPCCharacter.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "QuestWidget.h"
#include "GameFramework/PlayerController.h"

// 생성자: 컴포넌트 초기화
ANPCCharacter::ANPCCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// 1. 플레이어 감지 영역(BoxComponent) 생성 및 부착
	InteractionArea = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionArea"));
	InteractionArea->SetupAttachment(RootComponent);
	
	// 감지 영역 크기 설정 (반지름 개념이므로 200, 200, 100 크기)
	InteractionArea->SetBoxExtent(FVector(200.f, 200.f, 100.f));
}

void ANPCCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// 상호작용 실행 함수
void ANPCCharacter::OnInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("NPC: OnInteract 호출됨!"));

	// 1. 위젯이 이미 화면에 떠 있는지 확인
	if (CurrentQuestUI && CurrentQuestUI->IsInViewport())
	{
		UE_LOG(LogTemp, Warning, TEXT("NPC: 이미 위젯이 뷰포트에 있습니다."));
		return;
	}

	// 2. 퀘스트 위젯 클래스 설정 여부 확인
	if (QuestWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("NPC: QuestWidgetClass 발견. 위젯 생성을 시도합니다."));
		CurrentQuestUI = CreateWidget<UQuestWidget>(GetWorld(), QuestWidgetClass);
		
		if (CurrentQuestUI)
		{
			// 3. 화면에 추가
			CurrentQuestUI->AddToViewport();
			UE_LOG(LogTemp, Warning, TEXT("NPC: 위젯이 성공적으로 뷰포트에 추가되었습니다."));
			
			// 4. 마우스 커서 및 조작 모드 설정
			APlayerController* PC = GetWorld()->GetFirstPlayerController();
			if (PC)
			{
				FInputModeGameAndUI InputMode;
				InputMode.SetWidgetToFocus(CurrentQuestUI->TakeWidget());
				PC->SetInputMode(InputMode);
				PC->bShowMouseCursor = true;
				UE_LOG(LogTemp, Warning, TEXT("NPC: 마우스 커서 및 입력 모드가 전환되었습니다."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("NPC: 위젯 인스턴스 생성 실패!"));
		}
	}
	else
	{
		// 이 로그가 뜬다면 에디터에서 NPC 블루프린트의 QuestWidgetClass를 설정하지 않은 것입니다.
		UE_LOG(LogTemp, Error, TEXT("NPC: QuestWidgetClass가 설정되지 않았습니다! (에디터 확인 필요)"));
	}
}
