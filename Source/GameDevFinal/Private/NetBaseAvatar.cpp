// Fill out your copyright notice in the Description page of Project Settings.
#include "NetBaseAvatar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ANetBaseAvatar::ANetBaseAvatar()
{
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

    bIsSprinting = false;
}

void ANetBaseAvatar::BeginPlay()
{
    Super::BeginPlay();

    Camera->bUsePawnControlRotation = false;
    SpringArm->bUsePawnControlRotation = true;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ANetBaseAvatar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("Turn", this, &ACharacter::AddControllerYawInput);
    PlayerInputComponent->BindAxis("Lookup", this, &ACharacter::AddControllerPitchInput);

    PlayerInputComponent->BindAxis("MoveForward", this, &ANetBaseAvatar::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ANetBaseAvatar::MoveRight);

    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ANetBaseAvatar::StartSprint);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ANetBaseAvatar::StopSprint);
}

void ANetBaseAvatar::MoveForward(float Scale)
{
    FRotator Rotation = GetController()->GetControlRotation();
    FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
    FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    AddMovementInput(ForwardDirection, Scale);
}

void ANetBaseAvatar::MoveRight(float Scale)
{
    FRotator Rotation = GetController()->GetControlRotation();
    FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
    FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
    AddMovementInput(ForwardDirection, Scale);
}

void ANetBaseAvatar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ANetBaseAvatar, bIsSprinting);
}
void ANetBaseAvatar::StartSprint()
{
    ServerSetSprinting(true);
}

void ANetBaseAvatar::StopSprint()
{
    ServerSetSprinting(false);
}

void ANetBaseAvatar::ServerSetSprinting_Implementation(bool bNewSprinting)
{
    bIsSprinting = bNewSprinting;
    OnRep_IsSprinting();
}

void ANetBaseAvatar::OnRep_IsSprinting()
{
    GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? 1000.0f : 500.0f;
}