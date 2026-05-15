#include "PlayerPawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ResourceActor.h" // 상호작용 대상 확인을 위해 추가

APlayerPawn::APlayerPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    // 1. 루트 충돌체 설정 [cite: 3747, 3801]
    boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxRoot"));
    SetRootComponent(boxComp);
    boxComp->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));

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

    // 5. 이동 컴포넌트 [cite: 3754]
    moveComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MoveComp"));
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

void APlayerPawn::Interact(const FInputActionValue& Value)
{
    // 1단계: 함수가 실행되는지 확인
    UE_LOG(LogTemp, Warning, TEXT("Interact 함수 진입! (E 키 눌림)"));

    if (interactionBoxComp == nullptr) 
    {
        UE_LOG(LogTemp, Error, TEXT("interactionBoxComp가 Null입니다!"));
        return;
    }

    // 2단계: 상호작용 박스에 뭐가 걸려있는지 확인
    TArray<AActor*> OverlappingActors;
    interactionBoxComp->GetOverlappingActors(OverlappingActors);
    
    UE_LOG(LogTemp, Warning, TEXT("현재 감지된 액터 개수: %d"), OverlappingActors.Num());

    for (AActor* Actor : OverlappingActors)
    {
       // 3단계: 걸린 액터 이름 출력
       UE_LOG(LogTemp, Warning, TEXT("감지된 액터: %s"), *Actor->GetName());

       if (AResourceActor* Resource = Cast<AResourceActor>(Actor))
       {
          UE_LOG(LogTemp, Warning, TEXT("자원 액터 발견! Gather 호출합니다."));
          Resource->Gather(1.0f);
       }
    }
}

// --- PlayerPawn.cpp 파일의 맨 아래에 추가해 주세요! ---
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}