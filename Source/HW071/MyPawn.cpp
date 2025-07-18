// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ĸ�� �����ʺz�� �����ϰ� ��Ʈ�� ����
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComp;
	// ���� �ùķ��̼� off
	CapsuleComp->SetSimulatePhysics(false);

	// ���̷�Ż �޽� ������Ʈ�� �����ϰ� ĸ���� ����
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	// ���� �ùķ��̼� off
	Mesh->SetSimulatePhysics(false);
	
	// ������ ���� �����ϰ� ĸ���� ����
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f; // ī�޶���� �Ÿ�

	// ī�޶� �����ϰ� ������ �� ���� ����
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// �Է� ���� ���ؽ�Ʈ�� �߰�
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AMyPawn::Move(const FInputActionValue& Value)
{
	CurrentMoveInput = Value.Get<FVector2D>();
}



void AMyPawn::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddActorLocalRotation(FRotator(0.f, LookAxisVector.X * LookSpeed * GetWorld()->GetDeltaSeconds(), 0.f));

	FRotator CurrentSpringArmRotation = SpringArm->GetRelativeRotation();

	float NewPitch = CurrentSpringArmRotation.Pitch - LookAxisVector.Y * LookSpeed * GetWorld()->GetDeltaSeconds();

	NewPitch = FMath::Clamp(NewPitch, -75.f, 0.f); // ��ġ ����

	SpringArm->SetRelativeRotation(FRotator(NewPitch, 0.f, 0.f));
}
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector DeltaLocation(CurrentMoveInput.X, CurrentMoveInput.Y, 0.f);																																		

	DeltaLocation.Normalize();

	AddActorLocalOffset(DeltaLocation * MoveSpeed * DeltaTime);
}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(
			MoveAction,
			ETriggerEvent::Triggered,
			this,
			&AMyPawn::Move
		);
		
		EnhancedInputComponent->BindAction(
			LookAction,
			ETriggerEvent::Triggered,
			this,
			&AMyPawn::Look
		);
	}
}

