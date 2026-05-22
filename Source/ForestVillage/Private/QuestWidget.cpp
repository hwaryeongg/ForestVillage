#include "QuestWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ForestVillageGameModeBase.h" 
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h" // 모모(NPC)를 찾기 위해 추가
#include "NPCCharacter.h"           // 모모 클래스를 알기 위해 추가

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
       bool bCanComplete = GM->CanCompleteCurrentQuest(); // GameMode의 통합 체크 함수 사용

       // 1. 진행 가능 탭 (bShowAvailable == true)
       if (bShowAvailable)
       {
          FString Title = TEXT("");
          FString Desc = TEXT("");

          switch (QuestIdx)
          {
          case 0: // Quest 1: 마을 입구 보수
             Title = TEXT("Quest 1: 마을 입구 보수");
             {
                int32 Wood = GM->GetResourceCount(EResourceType::Wood);
                Desc = FString::Printf(TEXT("마을 입구의 가시덤불을 제거해야 합니다.\n(목재: %d / 5"), Wood );
             }
             break;

          case 1: // Quest 2: 따뜻한 주방 복원
             Title = TEXT("Quest 2: 따뜻한 주방 복원");
             {
                int32 Stone = GM->GetResourceCount(EResourceType::Stone);
                int32 Wood = GM->GetResourceCount(EResourceType::Wood);
                int32 Apple = GM->GetResourceCount(EResourceType::Apple);
                int32 Ruby = GM->GetResourceCount(EResourceType::Ruby);
                Desc = FString::Printf(TEXT("무너진 야외 부엌 가마솥을 복구합시다.\n(돌: %d/1, 목재: %d/3, 사과: %d/1, 루비: %d/1)"), Stone, Wood, Apple, Ruby);
             }
             break;

          case 2: // Quest 3: 광장의 생명수 (최종)
             Title = TEXT("Quest 3: 광장의 생명수 (최종)");
             {
                int32 Stone = GM->GetResourceCount(EResourceType::Stone);
                int32 Wood = GM->GetResourceCount(EResourceType::Wood);
                int32 Lapis = GM->GetResourceCount(EResourceType::Lapis);
                int32 Diamond = GM->GetResourceCount(EResourceType::Diamond);
                Desc = FString::Printf(TEXT("마을의 심장인 우물을 정화합시다.\n(돌: %d/10, 목재: %d/5, 라피스: %d/1, 다이아: %d/1)"), Stone, Wood, Lapis, Diamond);
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

       // GameMode에 구현된 쿨타임 로직이 포함된 함수를 호출합니다.
       if (GM->TryCompleteCurrentQuest())
       {
          // 퀘스트 완료 성공 시 모모(NPC) 즉시 순간이동 처리
          AActor* FoundNPC = UGameplayStatics::GetActorOfClass(GetWorld(), ANPCCharacter::StaticClass());
          if (FoundNPC)
          {
             if (QuestIdx == 0) // 첫 번째 퀘스트 완료
             {
                FoundNPC->SetActorLocation(FVector(2570.0f, 4210.0f, 910.0f));
             }
             else if (QuestIdx == 1) // 두 번째 퀘스트 완료
             {
                FoundNPC->SetActorLocation(FVector(1450.0f, 970.0f, 910.0f));
             }
          }
          
          // 더블 클릭 버그 방지 및 연출을 위해 즉시 UI를 닫습니다.
          CloseUI();
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