// Fill out your copyright notice in the Description page of Project Settings.

#include "QuestWidget.h"
#include "Components/Button.h" // UButton 기능을 위해 필요
#include "GameFramework/PlayerController.h"

void UQuestWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 1. 버튼이 유효한지 확인하고 클릭 이벤트(OnClicked)에 함수 연결
	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UQuestWidget::OnCloseClicked);
	}
}

void UQuestWidget::OnCloseClicked()
{
	// 2. 화면(Viewport)에서 위젯 제거
	RemoveFromParent();

	// 3. 조작 모드를 다시 게임 전용으로 돌리고 마우스 커서를 숨깁니다.
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}
}
