#include "InventoryWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "ForestVillageGameModeBase.h" // 자원 데이터를 가져오기 위해 포함
#include "GameFramework/PlayerController.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 닫기 버튼 이벤트 연결
	if (Btn_Close)
	{
		Btn_Close->OnClicked.AddDynamic(this, &UInventoryWidget::CloseUI);
	}

	// 위젯이 생성될 때 한 번 데이터 갱신
	UpdateInventory();
}

void UInventoryWidget::UpdateInventory()
{
	// 게임모드에서 실시간 자원 개수를 가져와 텍스트에 반영
	if (AForestVillageGameModeBase* GM = Cast<AForestVillageGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		if (Text_Apple)   Text_Apple->SetText(FText::AsNumber(GM->GetResourceCount(EResourceType::Apple)));
		if (Text_Orange)  Text_Orange->SetText(FText::AsNumber(GM->GetResourceCount(EResourceType::Orange)));
		if (Text_Wood)    Text_Wood->SetText(FText::AsNumber(GM->GetResourceCount(EResourceType::Wood)));
		if (Text_Stone)   Text_Stone->SetText(FText::AsNumber(GM->GetResourceCount(EResourceType::Stone)));
		if (Text_Ruby)    Text_Ruby->SetText(FText::AsNumber(GM->GetResourceCount(EResourceType::Ruby)));
		if (Text_Diamond) Text_Diamond->SetText(FText::AsNumber(GM->GetResourceCount(EResourceType::Diamond)));
		if (Text_Lapis)   Text_Lapis->SetText(FText::AsNumber(GM->GetResourceCount(EResourceType::Lapis)));
	}
}

void UInventoryWidget::CloseUI()
{
	RemoveFromParent();
	
	// 마우스 커서를 숨기고 조작 권한을 게임으로 돌려줌
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}
}