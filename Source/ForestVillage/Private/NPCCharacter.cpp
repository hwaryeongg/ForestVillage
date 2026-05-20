#include "NPCCharacter.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "ForestVillageGameModeBase.h"
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
	FString DialogueText = TEXT("");

	// 퀘스트 단계별 대사 설정
	switch (QuestIdx)
	{
	case 0:
		DialogueText = TEXT("모모: 야아아옹-? (입구의 가시덤불을 치워달라는 듯 서글프게 운다.)\n라비: 걱정 마 모모야, 내가 이 입구부터 깨끗하게 치워줄게!");
		break;
	case 1:
		DialogueText = TEXT("모모: 골골골... (낡은 일기장을 가리키며 냐아옹!)\n라비: 할아버지의 일기장이잖아? 아궁이를 먼저 고쳐서 밥부터 먹어야겠어.");
		break;
	case 2:
		DialogueText = TEXT("모모: 냐-옹! (우물 기둥 위에서 힘차게 울며 라비를 기다린다.)\n라비: 이제 마지막이야. 우물을 정화해서 생명수를 다시 흐르게 하자!");
		break;
	default:
		DialogueText = TEXT("모모: 야오옹 (행복한 표정으로 가만히 라비를 바라본다.)");
		break;
	}

	UE_LOG(LogTemp, Warning, TEXT("=== Dialogue ===\n%s"), *DialogueText);

	// 대화 위젯 표시 로직 (필요 시)
	if (DialogueWidgetClass)
	{
		if (!CurrentDialogueUI)
		{
			CurrentDialogueUI = CreateWidget<UUserWidget>(GetWorld(), DialogueWidgetClass);
		}
		
		if (CurrentDialogueUI && !CurrentDialogueUI->IsInViewport())
		{
			CurrentDialogueUI->AddToViewport();
			// (참고: 대화 위젯 내부에 텍스트를 세팅하는 블루프린트 로직이 있다고 가정합니다.)
		}
	}
}
