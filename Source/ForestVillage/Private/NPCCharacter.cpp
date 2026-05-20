#include "NPCCharacter.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "ForestVillageGameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h" // 리치 텍스트 지원

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
	// 1. 이미 대화창이 열려 있다면? -> 타이머 취소하고 즉시 닫기
	if (CurrentDialogueUI && CurrentDialogueUI->IsInViewport())
	{
		CloseDialogueUI();
		return;
	}

	AForestVillageGameModeBase* GM = Cast<AForestVillageGameModeBase>(GetWorld()->GetAuthGameMode());
	if (!GM) return;

	int32 QuestIdx = GM->GetCurrentQuestIndex();
	FString DialogueText = TEXT("");

	// v5.7 최종 시나리오 스크립트 적용
	switch (QuestIdx)
	{
	case 0: // Chapter 1: 낡은 벤치 위의 고양이
		DialogueText = TEXT("모모: (꼬리를 탁, 탁 치며 라비를 물끄러미 바라본다.) ...야아아옹-?\n")
					   TEXT("라비: 어라... 너 모모 맞지? 할아버지가 늘 자랑하시던 꼬마 턱시도 신사... 나 기억하는구나.\n")
					   TEXT("모모: (입구가 가시덤불로 완전히 막혀버린 진입로를 가리키며 작게 운다.) 애옹...\n")
					   TEXT("라비: 할아버지의 도구 가방... 알았어, 모모야. 주변 소나무를 베어서 이 입구부터 싹 치워보자!");
		break;

	case 1: // Chapter 2: 일기장의 약속과 따뜻한 요리솥
		DialogueText = TEXT("모모: (낡은 가죽 책 위를 꾹꾹이하듯 밟으며 조용히 가릉거린다.) 골골골...\n")
					   TEXT("라비: 이건 할아버지의 연구 일지잖아? 정령석을 광산 바위에 봉인해 두셨다고...?\n")
					   TEXT("라비: 모모야, 네가 나를 일기장으로 이끈 이유가 이거였구나! 근데 배가 너무 고프네.\n")
					   TEXT("라비: 일단 아궁이를 먼저 고쳐서 따뜻한 사과 스튜부터 끓여 먹자!");
		break;

	case 2: // Chapter 3: 다시 흐르는 생명수
		DialogueText = TEXT("모모: (메마른 우물 기둥 위에서 라비를 뚫어지게 바라본다.) 야옹.\n")
					   TEXT("라비: 우물을 정화하려면 푸른 보석 '라피스 라즐리'와 수호석 '다이아몬드'가 필요해.\n")
					   TEXT("라비: 돌 10개에 목재 5개... 좋아, 광산으로 가서 마지막 정령석들을 찾아오자!");
		break;

	default: // 최종 엔딩
		DialogueText = TEXT("라비: 부서진 울타리는 다시 단단해졌고, 요리솥 아래엔 따뜻한 온기가 돌기 시작했어.\n")
					   TEXT("라비: 할아버지, 저 모모와 함께 이곳에서 아주 따뜻하고 행복하게 지내볼게요.\n")
					   TEXT("모모: 골골골... 냐아아옹! (라비의 어깨 위에서 행복하게 가릉거린다.)");
		break;
	}

	// 2. 위젯 표시 및 텍스트 설정
	if (DialogueWidgetClass)
	{
		if (!CurrentDialogueUI)
		{
			CurrentDialogueUI = CreateWidget<UUserWidget>(GetWorld(), DialogueWidgetClass);
		}
		
		if (CurrentDialogueUI)
		{
			if (!CurrentDialogueUI->IsInViewport())
			{
				CurrentDialogueUI->AddToViewport();
			}

			// 위젯 내 "Text_Message" 위젯 검색
			UWidget* FoundWidget = CurrentDialogueUI->GetWidgetFromName(TEXT("Text_Message"));
			if (FoundWidget)
			{
				UTextBlock* MsgLabel = Cast<UTextBlock>(FoundWidget);
				if (MsgLabel)
				{
					MsgLabel->SetText(FText::FromString(DialogueText));
					UE_LOG(LogTemp, Warning, TEXT("NPC: Dialogue Text Set (TextBlock)"));
				}
				else
				{
					URichTextBlock* RichMsgLabel = Cast<URichTextBlock>(FoundWidget);
					if (RichMsgLabel)
					{
						RichMsgLabel->SetText(FText::FromString(DialogueText));
						UE_LOG(LogTemp, Warning, TEXT("NPC: Dialogue Text Set (RichTextBlock)"));
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("NPC: 'Text_Message' is a '%s', NOT a TextBlock/RichTextBlock!"), *FoundWidget->GetClass()->GetName());
					}
				}
			}

			// --- [추가] 3초 뒤 자동 닫기 타이머 설정 ---
			GetWorldTimerManager().ClearTimer(DialogueTimerHandle);
			GetWorldTimerManager().SetTimer(DialogueTimerHandle, this, &ANPCCharacter::CloseDialogueUI, 3.0f, false);
		}
	}
}

void ANPCCharacter::CloseDialogueUI()
{
	if (CurrentDialogueUI && CurrentDialogueUI->IsInViewport())
	{
		CurrentDialogueUI->RemoveFromParent();
		UE_LOG(LogTemp, Warning, TEXT("NPC: Dialogue UI Closed by Timer."));
	}

	// 타이머 핸들 정리
	GetWorldTimerManager().ClearTimer(DialogueTimerHandle);
}