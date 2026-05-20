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

	// --- 컴포넌트 선언 [cite: 3778] ---
	UPROPERTY(EditAnywhere, Category = "Components")
	class UBoxComponent* boxComp;

	// 상호작용 범위를 체크할 박스 컴포넌트 추가
	UPROPERTY(EditAnywhere, Category = "Components")
	class UBoxComponent* interactionBoxComp;

	UPROPERTY(EditAnywhere, Category = "Components")
	class UStaticMeshComponent* meshComp;

	// GEMINI.md 규칙에 따라 SpringArm 제거 (Camera를 Root에 직접 부착)
	UPROPERTY(EditAnywhere, Category = "Components")
	class UCameraComponent* cameraComp;

	UPROPERTY(EditAnywhere, Category = "Components")
	class UFloatingPawnMovement* moveComp;

	// --- 입력 에셋 설정 ---
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* imc_data;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* ia_move;

	// 마우스 회전을 위한 입력 에셋
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* ia_look;

	// 점프를 위한 입력 에셋
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* ia_jump;

	// 상호작용 키 (E키 등)를 위한 입력 에셋
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* ia_interact;

	// 퀘스트/인벤토리 키 (J키)를 위한 입력 에셋
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* ia_quest;

	// --- UI 설정 ---
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UQuestWidget> QuestWidgetClass;

	UPROPERTY()
	class UQuestWidget* CurrentQuestUI;

	// --- 이동 및 점프 변수 ---
	UPROPERTY(EditAnywhere, Category = "Movement")
	float VerticalVelocity = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float GravityScale = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float JumpImpulse = 500.0f;

	bool bIsOnGround = true;
	bool bIsJumping = false;

	// 함수 매개변수 타입을 FInputActionValue&로 명시 [cite: 4282]
	void Move(const FInputActionValue& value);

	// 마우스 회전 함수
	void Look(const FInputActionValue& value);

	// 점프 함수
	void Jump(const FInputActionValue& value);
	void StopJumping(const FInputActionValue& value);

	// 상호작용 실행 함수 (진단용 로그 버전의 매개변수 추가)
	void Interact(const FInputActionValue& value);

	// 퀘스트 UI 토글 함수
	void ToggleQuestUI(const FInputActionValue& value);
};

