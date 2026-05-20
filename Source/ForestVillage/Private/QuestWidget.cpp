#include "QuestWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ForestVillageGameModeBase.h" 
#include "GameFramework/PlayerController.h"

void UQuestWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_TabAvailable) 
		Btn_TabAvailable->OnClicked.AddDynamic(this, &UQuestWidget::OnAvailableTabClicked);
	
	if (Btn_TabCompleted) 
		Btn_TabCompleted->OnClicked.AddDynamic(this, &UQuestWidget::OnCompletedTabClicked);
	
	if (Btn_CompleteQuest) 
		Btn_CompleteQuest->OnClicked.AddDynamic(this, &UQuestWidget::OnCompleteButtonClicked);

	UpdateQuestView(true);
}

void UQuestWidget::UpdateQuestView(bool bShowAvailable)
{
	if (AForestVillageGameModeBase* GM = Cast<AForestVillageGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		int32 QuestIdx = GM->GetCurrentQuestIndex();

		// 1. 진행 가능 탭 (bShowAvailable == true)
		if (bShowAvailable)
		{
			bool bCanComplete = false;
			FString Title = TEXT("");
			FString Desc = TEXT("");

			switch (QuestIdx)
			{
			case 0: 
				Title = TEXT("Quest 1: 마을 입구 보수");
				{
					int32 Wood = GM->GetResourceCount(EResourceType::Wood);
					bCanComplete = (Wood >= 5);
					Desc = FString::Printf(TEXT("마을 입구의 가시덤불을 제거해야 합니다.\n(목재: %d / 5)"), Wood);
				}
				break;

			case 1: 
				Title = TEXT("Quest 2: 따뜻한 주방 복원");
				{
					int32 Stone = GM->GetResourceCount(EResourceType::Stone);
					int32 Wood = GM->GetResourceCount(EResourceType::Wood);
					int32 Ruby = GM->GetResourceCount(EResourceType::Ruby);
					bCanComplete = (Stone >= 1 && Wood >= 3 && Ruby >= 1);
					Desc = FString::Printf(TEXT("무너진 야외 부엌 가마솥을 복구합시다.\n(돌: %d/1, 목재: %d/3, 루비: %d/1)"), Stone, Wood, Ruby);
				}
				break;

			case 2: 
				Title = TEXT("Quest 3: 광장의 생명수 (최종)");
				{
					int32 Stone = GM->GetResourceCount(EResourceType::Stone);
					int32 Wood = GM->GetResourceCount(EResourceType::Wood);
					int32 Lapis = GM->GetResourceCount(EResourceType::Lapis);
					int32 Diamond = GM->GetResourceCount(EResourceType::Diamond);
					bCanComplete = (Stone >= 10 && Wood >= 5 && Lapis >= 1 && Diamond >= 1);
					Desc = FString::Printf(TEXT("마을의 심장인 우물을 정화합시다.\n(돌:%d/10, 목재:%d/5, 라피스:%d/1, 다이아:%d/1)"), Stone, Wood, Lapis, Diamond);
				}
				break;

			default:
				Title = TEXT("모든 복원 완료!");
				Desc = TEXT("로렐 밸리에 평화가 찾아왔습니다. 감사합니다!");
				bCanComplete = false;
				break;
			}

			if (Text_QuestTitle) Text_QuestTitle->SetText(FText::FromString(Title));
			if (Text_QuestDescription) Text_QuestDescription->SetText(FText::FromString(Desc));

			if (Btn_CompleteQuest)
			{
				Btn_CompleteQuest->SetVisibility(ESlateVisibility::Visible);
				Btn_CompleteQuest->SetIsEnabled(bCanComplete);
				Btn_CompleteQuest->SetRenderOpacity(bCanComplete ? 1.0f : 0.5f);
			}
		}
		// 2. 완료됨 탭 (bShowAvailable == false)
		else
		{
			if (Text_QuestTitle) Text_QuestTitle->SetText(FText::FromString(TEXT("완료된 기록")));

			FString CompletedList = TEXT("");
			if (QuestIdx > 0) CompletedList += TEXT("v Quest 1: 마을 입구 보수 (완료)\n");
			if (QuestIdx > 1) CompletedList += TEXT("v Quest 2: 따뜻한 주방 복원 (완료)\n");
			if (QuestIdx > 2) CompletedList += TEXT("v Quest 3: 광장의 생명수 (완료)\n");

			if (QuestIdx == 0) CompletedList = TEXT("아직 완료된 퀘스트가 없습니다.");

			if (Text_QuestDescription) Text_QuestDescription->SetText(FText::FromString(CompletedList));

			// 완료 탭에서는 완료 버튼을 숨깁니다.
			if (Btn_CompleteQuest) 
			{
				Btn_CompleteQuest->SetVisibility(ESlateVisibility::Collapsed);
			}
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
	if (AForestVillageGameModeBase* GM = Cast<AForestVillageGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		int32 QuestIdx = GM->GetCurrentQuestIndex();
		bool bSuccess = false;

		if (QuestIdx == 0)
		{
			bSuccess = GM->SpendResource(EResourceType::Wood, 5);
		}
		else if (QuestIdx == 1)
		{
			if (GM->GetResourceCount(EResourceType::Stone) >= 1 && GM->GetResourceCount(EResourceType::Wood) >= 3 && GM->GetResourceCount(EResourceType::Ruby) >= 1)
			{
				GM->SpendResource(EResourceType::Stone, 1);
				GM->SpendResource(EResourceType::Wood, 3);
				GM->SpendResource(EResourceType::Ruby, 1);
				bSuccess = true;
			}
		}
		else if (QuestIdx == 2)
		{
			if (GM->GetResourceCount(EResourceType::Stone) >= 10 && GM->GetResourceCount(EResourceType::Wood) >= 5 && GM->GetResourceCount(EResourceType::Lapis) >= 1 && GM->GetResourceCount(EResourceType::Diamond) >= 1)
			{
				GM->SpendResource(EResourceType::Stone, 10);
				GM->SpendResource(EResourceType::Wood, 5);
				GM->SpendResource(EResourceType::Lapis, 1);
				GM->SpendResource(EResourceType::Diamond, 1);
				bSuccess = true;
			}
		}

		if (bSuccess)
		{
			GM->CompleteQuest();
			// 탭 상태를 유지하며 갱신 (보통 완료 후엔 다음 퀘스트가 보이게 하는 것이 자연스러움)
			UpdateQuestView(true);
		}
	}
}

void UQuestWidget::CloseUI()
{
	RemoveFromParent();
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}
}