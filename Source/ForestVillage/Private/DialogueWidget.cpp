#include "DialogueWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "Input/Reply.h" // FReply 제어를 위해 필수 포함

void UDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 위젯이 키보드 포커스를 직접 획득할 수 있도록 포커스 활성화 설정을 켭니다.
	bIsFocusable = true;

	if (Btn_Next)
	{
		Btn_Next->OnClicked.AddDynamic(this, &UDialogueWidget::OnNextClicked);
	}
}

// 플레이어가 키보드를 누를 때 자동으로 트리거되는 엔진 내부 함수
FReply UDialogueWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// 1. 입력된 키가 'E' 키인지 정밀 대조합니다.
	if (InKeyEvent.GetKey() == EKeys::E)
	{
		// 다음 대사로 이행
		OnNextClicked();
		
		// [매우 중요] 입력을 처리 완료(Handled) 상태로 돌려줍니다.
		// 이렇게 하면 'E' 키 입력이 캐릭터 폰(Pawn)으로 더 이상 내려가지 않고 위젯에서 소멸하므로,
		// 대화 중에 등 뒤에 있는 자원을 때리거나 줍는 원치 않는 오작동 버그를 확실하게 방어합니다.
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UDialogueWidget::StartDialogue(const TArray<FDialogueLine>& Lines)
{
	CurrentDialogueLines = Lines;
	CurrentLineIndex = 0; 
	
	DisplayCurrentLine();
}

void UDialogueWidget::OnNextClicked()
{
	CurrentLineIndex++;
	DisplayCurrentLine();
}

void UDialogueWidget::DisplayCurrentLine()
{
	if (CurrentLineIndex < CurrentDialogueLines.Num())
	{
		FDialogueLine CurrentLine = CurrentDialogueLines[CurrentLineIndex];

		if (Text_SpeakerName) Text_SpeakerName->SetText(FText::FromString(CurrentLine.SpeakerName));
		if (Text_Dialogue) Text_Dialogue->SetText(FText::FromString(CurrentLine.Text));

		if (Image_Portrait)
		{
			if (CurrentLine.SpeakerName == TEXT("모모") && MomoPortrait)
			{
				Image_Portrait->SetBrushFromTexture(MomoPortrait);
			}
			else if (CurrentLine.SpeakerName == TEXT("라비") && RaviPortrait)
			{
				Image_Portrait->SetBrushFromTexture(RaviPortrait);
			}
		}
	}
	else
	{
		RemoveFromParent();
		
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = false;
		}
	}
}