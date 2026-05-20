#include "PlayerPawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ResourceActor.h"
#include "NPCCharacter.h" 
#include "ItemActor.h"    
#include "QuestWidget.h"
#include "InventoryWidget.h" // 신규 추가: 인벤토리 위젯
#include "Blueprint/UserWidget.h"

APlayerPawn::APlayerPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    // 1. 루트 충돌체 설정
    boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxRoot"));
    SetRootComponent(boxComp);
    boxComp->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
    boxComp->SetCollisionProfileName(TEXT("Pawn"));
    boxComp->SetCanEverAffectNavigation(true);

    // 2. 상호작용 박스 설정
    interactionBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
    interactionBoxComp->SetupAttachment(RootComponent);
    interactionBoxComp->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));
    interactionBoxComp->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));

    // 3. 메시 부착
    meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    meshComp->SetupAttachment(RootComponent);

    // 4. 카메라 설정 (GEMINI.md 규칙: SpringArm 없이 직접 부착)
    cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
    cameraComp->SetupAttachment(RootComponent); 
    cameraComp->SetRelativeLocation(FVector(-400.0f, 0.0f, 300.0f));
    cameraComp->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f));
    cameraComp->bUsePawnControlRotation = true;

    // 5. 이동 컴포넌트
    moveComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MoveComp"));
    moveComp->MaxSpeed = 800.0f;
    moveComp->Acceleration = 2000.0f;
    moveComp->Deceleration = 2000.0f;

    bUseControllerRotationYaw = true;
}

void APlayerPawn::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* pc = Cast<APlayerController>(GetController()))
    {
       if (UEnhancedInputLocalPlayerSubsystem* subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer()))
       {
          if (imc_data)
          {
             subsys->AddMappingContext(imc_data, 0);
          }
       }
    }
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
       EnhancedInput->BindAction(ia_move, ETriggerEvent::Triggered, this, &APlayerPawn::Move);
       EnhancedInput->BindAction(ia_look, ETriggerEvent::Triggered, this, &APlayerPawn::Look);
       EnhancedInput->BindAction(ia_jump, ETriggerEvent::Started, this, &APlayerPawn::Jump);
       EnhancedInput->BindAction(ia_jump, ETriggerEvent::Completed, this, &APlayerPawn::StopJumping);
       EnhancedInput->BindAction(ia_interact, ETriggerEvent::Started, this, &APlayerPawn::Interact);
       EnhancedInput->BindAction(ia_quest, ETriggerEvent::Started, this, &APlayerPawn::ToggleQuestUI);
       
       // I키: 인벤토리 입력 바인딩 연결
       if (ia_inventory)
       {
           EnhancedInput->BindAction(ia_inventory, ETriggerEvent::Started, this, &APlayerPawn::ToggleInventoryUI);
       }
    }
}

void APlayerPawn::Move(const FInputActionValue& value)
{
    FVector2D moveVec = value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        AddMovementInput(ForwardDirection, moveVec.Y);
        AddMovementInput(RightDirection, moveVec.X);
    }
}

void APlayerPawn::Look(const FInputActionValue& value)
{
    FVector2D lookVec = value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        AddControllerYawInput(lookVec.X);
        AddControllerPitchInput(lookVec.Y);
    }
}

void APlayerPawn::Interact(const FInputActionValue& Value)
{
    if (interactionBoxComp == nullptr) return;

    TArray<AActor*> OverlappingActors;
    interactionBoxComp->GetOverlappingActors(OverlappingActors);
    
    for (AActor* Actor : OverlappingActors)
    {
       if (Actor == nullptr) continue;

       // NPC(모모)와의 대화 상호작용
       if (ANPCCharacter* NPC = Cast<ANPCCharacter>(Actor))
       {
          NPC->OnInteract();
          return;
       }

       // 자원 타격 상호작용
       if (AResourceActor* Resource = Cast<AResourceActor>(Actor))
       {
          Resource->Gather(1.0f);
          return; 
       }

       // 아이템 줍기 상호작용
       if (AItemActor* Item = Cast<AItemActor>(Actor))
       {
          Item->Collect();
          return;
       }
    }
}

void APlayerPawn::ToggleQuestUI(const FInputActionValue& value)
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    if (CurrentQuestUI && CurrentQuestUI->IsInViewport())
    {
        CurrentQuestUI->RemoveFromParent();
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
        return;
    }

    if (QuestWidgetClass)
    {
        if (!CurrentQuestUI)
        {
            CurrentQuestUI = CreateWidget<UQuestWidget>(GetWorld(), QuestWidgetClass);
        }

        if (CurrentQuestUI)
        {
            CurrentQuestUI->AddToViewport();
            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(CurrentQuestUI->TakeWidget());
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
        }
    }
}

// 신규 추가: 인벤토리 UI 토글 함수
void APlayerPawn::ToggleInventoryUI(const FInputActionValue& value)
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    // 1. 인벤토리가 이미 열려있다면 닫기
    if (CurrentInventoryUI && CurrentInventoryUI->IsInViewport())
    {
        CurrentInventoryUI->CloseUI(); // CloseUI 내부에서 마우스 숨김 처리됨
        return;
    }

    // 2. 안 열려있다면 생성 및 화면에 띄우기
    if (InventoryWidgetClass)
    {
        if (!CurrentInventoryUI)
        {
            CurrentInventoryUI = CreateWidget<UInventoryWidget>(GetWorld(), InventoryWidgetClass);
        }

        if (CurrentInventoryUI)
        {
            CurrentInventoryUI->AddToViewport();
            CurrentInventoryUI->UpdateInventory(); // 열 때마다 최신 자원 개수 갱신

            // UI 조작 모드로 전환 및 마우스 표시
            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(CurrentInventoryUI->TakeWidget());
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
        }
    }
}

void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // 가상 중력을 위한 라인트레이스 충돌 검사
    FHitResult HitResult;
    FVector Start = GetActorLocation();
    FVector End = Start - FVector(0.0f, 0.0f, 55.0f);
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

    if (bHit)
    {
        if (!bIsOnGround && VerticalVelocity <= 0)
        {
            bIsOnGround = true;
            VerticalVelocity = 0.0f;
            bIsJumping = false;
        }
    }
    else
    {
        bIsOnGround = false;
    }

    if (!bIsOnGround)
    {
        VerticalVelocity -= 980.0f * GravityScale * DeltaTime;
    }

    if (FMath::Abs(VerticalVelocity) > 0.1f || !bIsOnGround)
    {
        FVector NewLocation = GetActorLocation() + FVector(0.0f, 0.0f, VerticalVelocity * DeltaTime);
        if (bHit && NewLocation.Z < HitResult.ImpactPoint.Z + 50.0f)
        {
            NewLocation.Z = HitResult.ImpactPoint.Z + 50.0f;
            VerticalVelocity = 0.0f;
            bIsOnGround = true;
        }
        SetActorLocation(NewLocation);
    }
}

void APlayerPawn::Jump(const FInputActionValue& value)
{
    if (bIsOnGround)
    {
        VerticalVelocity = JumpImpulse;
        bIsOnGround = false;
        bIsJumping = true;
    }
}

void APlayerPawn::StopJumping(const FInputActionValue& value)
{
    bIsJumping = false;
    if (VerticalVelocity > 0)
    {
        VerticalVelocity *= 0.5f; // 아날로그 점프(숏점프) 지원
    }
}