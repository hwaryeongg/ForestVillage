// Fill out your copyright notice in the Description page of Project Settings.

#include "QuestWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "ForestVillageGameModeBase.h" // 실제 게임모드 헤더 포함
#include "GameFramework/PlayerController.h"

void UQuestWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 1. 버튼 이벤트 바인딩 (유효성 검사 후 연결)
	if (Btn_TabAvailable) 
		Btn_TabAvailable->OnClicked.AddDynamic(this, &UQuestWidget::OnAvailableTabClicked);
	
	if (Btn_TabCompleted) 
		Btn_TabCompleted->OnClicked.AddDynamic(this, &UQuestWidget::OnCompletedTabClicked);
	
	if (Btn_CompleteQuest) 
		Btn_CompleteQuest->OnClicked.AddDynamic(this, &UQuestWidget::OnCompleteButtonClicked);

	// 2. 기본값으로 '진행 가능' 탭을 먼저 보여줍니다.
	UpdateQuestView(true);
}

void UQuestWidget::UpdateQuestView(bool bShowAvailable)
{
	// TODO: QuestListBox 내부의 이전 목록들을 삭제하고 새로운 데이터를 채우는 로직 필요
	
	// 3. 게임모드에서 자원 정보를 가져와 UI 텍스트와 버튼 상태 업데이트
	if (AForestVillageGameModeBase* GM = Cast<AForestVillageGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		// 예시 데이터: 나무 울타리 복원 (나무 5개 필요)
		int32 CurrentWood = GM->GetResourceCount(EResourceType::Wood);
		bool bCanComplete = CurrentWood >= 5;
		
		if (Btn_CompleteQuest)
		{
			// 자원이 충분할 때만 버튼 활성화
			Btn_CompleteQuest->SetIsEnabled(bCanComplete);
			// 비활성화 시 시각적 피드백 (투명도 조절)
			Btn_CompleteQuest->SetRenderOpacity(bCanComplete ? 1.0f : 0.5f);
		}

		// 텍스트 정보 업데이트
		if (Text_QuestTitle) 
			Text_QuestTitle->SetText(FText::FromString(TEXT("나무 울타리 복원")));
		if (Text_QuestDescription)
		{
			FString Desc = FString::Printf(TEXT("마을 입구의 부서진 울타리를 고쳐야 합니다.\n(필요한 나무: 5 / 현재 보유: %d)"), CurrentWood);
			Text_QuestDescription->SetText(FText::FromString(Desc));
		}
	}
}

void UQuestWidget::OnAvailableTabClicked()
{
	UpdateQuestView(true);
}

void UQuestWidget::OnCompletedTabClicked()
{
	UpdateQuestView(false);
}

void UQuestWidget::OnCompleteButtonClicked()
{
	// 4. 실제 복원 로직 실행 (로그 출력 후 위젯 닫기)
	UE_LOG(LogTemp, Warning, TEXT("퀘스트 완료: 마을 복원 시퀀스 시작!"));
	
	// 화면에서 위젯 제거
	RemoveFromParent();
	
	// 게임 조작 모드로 복구 및 마우스 커서 숨김
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}
}
