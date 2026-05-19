#include "PlayerPawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ResourceActor.h"
#include "NPCCharacter.h" // NPC 클래스 확인을 위해 추가
#include "ItemActor.h"    // 아이템 획득을 위해 추가

APlayerPawn::APlayerPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    // 1. 루트 충돌체 설정 [cite: 3747, 3801]
    boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxRoot"));
    SetRootComponent(boxComp);
    boxComp->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
    
    // 충돌 설정 추가: 다른 물체와 부딪히도록 Pawn 프로필 적용
    boxComp->SetCollisionProfileName(TEXT("Pawn"));
    boxComp->SetCanEverAffectNavigation(true); // 내비게이션에 영향을 주도록 설정 (필요 시)

    // 2. 상호작용 박스 설정 (캐릭터 정면 100유닛 위치)
    interactionBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
    interactionBoxComp->SetupAttachment(RootComponent);
    interactionBoxComp->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));
    interactionBoxComp->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));

    // 3. 메시 부착 [cite: 3753, 3804]
    meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    meshComp->SetupAttachment(RootComponent);

    // 4. 카메라 설정 (GEMINI.md 규칙: SpringArm 없이 직접 부착)
    cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
    cameraComp->SetupAttachment(RootComponent); 
    cameraComp->SetRelativeLocation(FVector(-400.0f, 0.0f, 300.0f)); // 뒤쪽 위에서 내려다보는 오프셋
    cameraComp->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f));
    
    // 카메라도 컨트롤러의 회전을 따르도록 설정 (마우스 상하 회전 적용을 위해 필요)
    cameraComp->bUsePawnControlRotation = true;

    // 5. 이동 컴포넌트 [cite: 3754]
    moveComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MoveComp"));
    moveComp->MaxSpeed = 800.0f;       // 이동 속도 설정
    moveComp->Acceleration = 2000.0f; // 가속도 (수치가 낮을수록 부드럽게 출발)
    moveComp->Deceleration = 2000.0f; // 감속도 (수치가 낮을수록 부드럽게 멈춤)

    // 마우스 좌우 회전에 따라 Pawn(캐릭터)이 함께 회전하도록 설정
    bUseControllerRotationYaw = true;
}

void APlayerPawn::BeginPlay()
{
    Super::BeginPlay();

    // IMC 등록 로직 [cite: 4219]
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

    // 액션 바인딩 [cite: 4017, 4285]
    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
       EnhancedInput->BindAction(ia_move, ETriggerEvent::Triggered, this, &APlayerPawn::Move);
       
       // 마우스 회전 액션 바인딩 추가
       EnhancedInput->BindAction(ia_look, ETriggerEvent::Triggered, this, &APlayerPawn::Look);

       // 점프 액션 바인딩 추가
       EnhancedInput->BindAction(ia_jump, ETriggerEvent::Started, this, &APlayerPawn::Jump);
       EnhancedInput->BindAction(ia_jump, ETriggerEvent::Completed, this, &APlayerPawn::StopJumping);

       // 상호작용 키 바인딩 추가
       EnhancedInput->BindAction(ia_interact, ETriggerEvent::Started, this, &APlayerPawn::Interact);
    }
}

void APlayerPawn::Move(const FInputActionValue& value)
{
    FVector2D moveVec = value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // 컨트롤러의 회전 방향을 기준으로 앞뒤/좌우 계산 [cite: 4272, 4274]
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
        // 마우스 좌우 입력 -> Yaw 회전 (캐릭터와 카메라가 함께 회전)
        AddControllerYawInput(lookVec.X);
        // 마우스 상하 입력 -> Pitch 회전 (카메라만 상하로 회전)
        AddControllerPitchInput(lookVec.Y);
    }
}

void APlayerPawn::Interact(const FInputActionValue& Value)
{
    // 1단계: 함수가 실행되는지 확인 (이 로그가 안 뜨면 입력 설정 문제)
    UE_LOG(LogTemp, Warning, TEXT("=== Interact 함수 진입! (E 키 눌림) ==="));

    if (interactionBoxComp == nullptr) 
    {
        UE_LOG(LogTemp, Error, TEXT("interactionBoxComp가 Null입니다!"));
        return;
    }

    // 2단계: 상호작용 박스에 뭐가 걸려있는지 확인
    TArray<AActor*> OverlappingActors;
    interactionBoxComp->GetOverlappingActors(OverlappingActors);
    
    for (AActor* Actor : OverlappingActors)
    {
       if (Actor == nullptr) continue;

       // 3단계: NPC인지 먼저 확인 (새로 추가된 로직)
       if (ANPCCharacter* NPC = Cast<ANPCCharacter>(Actor))
       {
          UE_LOG(LogTemp, Warning, TEXT("NPC 발견! OnInteract를 호출합니다: %s"), *NPC->GetName());
          NPC->OnInteract();
          return; // NPC와 상호작용하면 루프 종료
       }

       // 4단계: 자원 액터인지 확인
       if (AResourceActor* Resource = Cast<AResourceActor>(Actor))
       {
          UE_LOG(LogTemp, Warning, TEXT("자원 액터 발견! Gather 호출합니다."));
          Resource->Gather(1.0f);
          return; 
       }

       // 5단계: 아이템 액터(사과/나무/오렌지)인지 확인 -> [추가된 로직]
       if (AItemActor* Item = Cast<AItemActor>(Actor))
       {
          UE_LOG(LogTemp, Warning, TEXT("떨어진 아이템 발견! 수동 획득을 시도합니다."));
          Item->Collect();
          return; // 하나 주우면 종료
       }
    }

    UE_LOG(LogTemp, Warning, TEXT("상호작용 범위 내에 대상이 없습니다."));
}

void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // --- 수동 중력 및 점프 로직 추가 ---
    
    // 1. 바닥 체크 (LineTrace)
    FHitResult HitResult;
    FVector Start = GetActorLocation();
    FVector End = Start - FVector(0.0f, 0.0f, 55.0f); // 박스 절반 높이(50) + 여유분(5)
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    // 바닥과 충돌하는지 확인
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

    if (bHit)
    {
        // 바닥에 닿음
        if (!bIsOnGround && VerticalVelocity <= 0)
        {
            bIsOnGround = true;
            VerticalVelocity = 0.0f;
            bIsJumping = false;
        }
    }
    else
    {
        // 공중에 떠 있음
        bIsOnGround = false;
    }

    // 2. 중력 적용
    if (!bIsOnGround)
    {
        // 중력 가속도 적용 (980 * GravityScale)
        VerticalVelocity -= 980.0f * GravityScale * DeltaTime;
    }

    // 3. 수직 이동 적용
    if (FMath::Abs(VerticalVelocity) > 0.1f || !bIsOnGround)
    {
        FVector NewLocation = GetActorLocation() + FVector(0.0f, 0.0f, VerticalVelocity * DeltaTime);
        
        // 바닥을 뚫고 내려가지 않도록 보정
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
        UE_LOG(LogTemp, Warning, TEXT("Jump!"));
    }
}

void APlayerPawn::StopJumping(const FInputActionValue& value)
{
    bIsJumping = false;
    // 점프 키를 뗐을 때 상승 중이라면 속도를 줄여서 짧은 점프 구현 (선택 사항)
    if (VerticalVelocity > 0)
    {
        VerticalVelocity *= 0.5f;
    }
}