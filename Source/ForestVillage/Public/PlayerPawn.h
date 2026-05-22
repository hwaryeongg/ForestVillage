#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h" // FInputActionValue 사용을 위해 필요
#include "PlayerPawn.generated.h"

UCLASS()
class FORESTVILLAGE_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	APlayerPawn();

protected:
	virtual void BeginPlay() override;

public: 
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// --- 1. 컴포넌트 선언 ---
	UPROPERTY(EditAnywhere, Category = "Components")
	class UBoxComponent* boxComp;

	// 상호작용 범위를 체크할 박스 컴포넌트
	UPROPERTY(EditAnywhere, Category = "Components")
	class UBoxComponent* interactionBoxComp;

	UPROPERTY(EditAnywhere, Category = "Components")
	class UStaticMeshComponent* meshComp;

	// GEMINI.md 규칙에 따라 SpringArm 제거 (Camera를 Root에 직접 부착)
	UPROPERTY(EditAnywhere, Category = "Components")
	class UCameraComponent* cameraComp;

	UPROPERTY(EditAnywhere, Category = "Components")
	class UFloatingPawnMovement* moveComp;


	// --- 2. 입력 에셋(Enhanced Input) 설정 ---
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* imc_data;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* ia_move;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* ia_look;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* ia_jump;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* ia_interact;

	// J키: 퀘스트 창 토글용
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* ia_quest;

	// I키: 인벤토리 창 토글용 (신규 추가)
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* ia_inventory; 


	// --- 3. UI 클래스 설정 ---
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UQuestWidget> QuestWidgetClass;

	UPROPERTY()
	class UQuestWidget* CurrentQuestUI;

	// 인벤토리 위젯 클래스 (신규 추가)
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UInventoryWidget> InventoryWidgetClass; 

	// 인벤토리 인스턴스 보관용 (신규 추가)
	UPROPERTY()
	class UInventoryWidget* CurrentInventoryUI;


	// --- 4. 이동 및 점프 물리 변수 ---
	UPROPERTY(EditAnywhere, Category = "Movement")
	float VerticalVelocity = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float GravityScale = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float JumpImpulse = 500.0f;

	// --- 킬존 설정 ---
	UPROPERTY(EditAnywhere, Category = "Movement")
	float KillZoneHeight = -1000.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	FVector SafeLocation = FVector(-2780.0f, 790.0f, 1100.0f);

	bool bIsOnGround = true;
	bool bIsJumping = false;


	// --- 5. 조작 및 시스템 함수 ---
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void Jump(const FInputActionValue& value);
	void StopJumping(const FInputActionValue& value);
	void Interact(const FInputActionValue& value);
	
	// UI 토글 함수들
	void ToggleQuestUI(const FInputActionValue& value);
	void ToggleInventoryUI(const FInputActionValue& value); // 인벤토리 토글 (신규 추가)
};