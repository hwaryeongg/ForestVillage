#include "NPCCharacter.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "ForestVillageGameModeBase.h"
#include "DialogueWidget.h" // 신규 대화 위젯 헤더 포함
#include "GameFramework/PlayerController.h"

ANPCCharacter::ANPCCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    InteractionArea = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionArea"));
    InteractionArea->SetupAttachment(RootComponent);
    InteractionArea->SetBoxExtent(FVector(200.f, 200.f, 100.f));
}

void ANPCCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void ANPCCharacter::OnInteract()
{
    AForestVillageGameModeBase* GM = Cast<AForestVillageGameModeBase>(GetWorld()->GetAuthGameMode());
    if (!GM) return;

    int32 QuestIdx = GM->GetCurrentQuestIndex();

    // 1. 대사 리스트(배열)를 담을 그릇 생성
    TArray<FDialogueLine> DialogueList;

    // 2. 퀘스트 단계별 대사 설정 및 순간이동 처리
    switch (QuestIdx)
    {
    case 0:
       DialogueList.Add({ TEXT("모모"), TEXT("야아아옹-? 마을 입구에 있는 나무들을 치워줘)") });
       DialogueList.Add({ TEXT("라비"), TEXT("마을 꼴이 이게 뭐니... ") });
       DialogueList.Add({ TEXT("라비"), TEXT("걱정 마 모모야, 내가 이 입구부터 깨끗하게 치워줄게!") });
       break;

    case 1:
       // 1번째 퀘스트 완료 직후 (아궁이 제단 위치로 이동)
       SetActorLocation(FVector(2570.0f, 4210.0f, 910.0f));

       DialogueList.Add({ TEXT("모모"), TEXT("앞에 있는 돌을 캐서 보석을 모아 나에게 가져다 줘 ! )") });
       DialogueList.Add({ TEXT("라비"), TEXT("알겠어 ") });
       DialogueList.Add({ TEXT("라비"), TEXT("E를 눌러 돌을 캐보자 ") });
       break;

    case 2:
       // 2번째 퀘스트 완료 직후 (최종 우물 제단 위치로 이동)
       SetActorLocation(FVector(1450.0f, 970.0f, 910.0f));

       DialogueList.Add({ TEXT("모모"), TEXT("냐-옹! (광장 중앙의 우물 기둥 위에서 힘차게 울며 라비를 기다린다.)") });
       DialogueList.Add({ TEXT("라비"), TEXT("이제 마지막 제단이야. 우물을 정화해서 마을에 생명수를 다시 흐르게 하자!") });
       break;

    default:
       DialogueList.Add({ TEXT("모모"), TEXT("야오옹~ (행복한 표정으로 가만히 라비를 바라본다.)") });
       DialogueList.Add({ TEXT("라비"), TEXT("우리 마을이 드디어 원래 모습을 되찾았어. 고마워 모모야!") });
       break;
    }

    // 3. 이미 대화창이 떠 있다면 무시 (클릭 연타 방지)
    if (CurrentDialogueUI && CurrentDialogueUI->IsInViewport())
    {
       return;
    }

    // 4. 대화창 생성 및 화면 출력
    if (DialogueWidgetClass)
    {
       if (!CurrentDialogueUI)
       {
          CurrentDialogueUI = CreateWidget<UDialogueWidget>(GetWorld(), DialogueWidgetClass);
       }
       
       if (CurrentDialogueUI)
       {
          CurrentDialogueUI->AddToViewport();

          // UI 내부의 StartDialogue 함수를 호출하여 배열 데이터 전달
          CurrentDialogueUI->StartDialogue(DialogueList);
          
          // 마우스 활성화 및 UI 전용 조작으로 변경
          APlayerController* PC = GetWorld()->GetFirstPlayerController();
          if (PC)
          {
             FInputModeGameAndUI InputMode;
             InputMode.SetWidgetToFocus(CurrentDialogueUI->TakeWidget());
             PC->SetInputMode(InputMode);
             PC->bShowMouseCursor = true;
          }
       }
    }
}