// Fill out your copyright notice in the Description page of Project Settings.
#include "NetBaseAvatar.h"
#include "Weapon.h"
#include "NetBaseZombie.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ANetBaseAvatar::ANetBaseAvatar()
{
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

    bIsSprinting = false;

    StaminaDrainRate = 20;
    StaminaGainRate = 10;
}

void ANetBaseAvatar::BeginPlay()
{
    Super::BeginPlay();

    Camera->bUsePawnControlRotation = false;
    SpringArm->bUsePawnControlRotation = true;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    EquipWeapon(DefaultWeaponClass);
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
    if (CurrentStamina > 0)
    {
        ServerSetSprinting(true);
        bIsSprinting = true;
    }
}

void ANetBaseAvatar::StopSprint()
{
    ServerSetSprinting(false);
    bIsSprinting = false;
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


//strenght damage arttırıcak, intelligence de magic range arttırıcak.

void ANetBaseAvatar::SetAvatarValues()
{
    int HealthIndex = (int)ECharStats::Health;
    int DexterityIndex = (int)ECharStats::Dexterity;
    int IntelligenceIndex = (int)ECharStats::Intelligence;
    int StrenghtIndex = (int)ECharStats::Strenght;

    if (PlayerInfo.CharStats.Stats.IsValidIndex(HealthIndex))
    {
        MaxHealth = 100 * PlayerInfo.CharStats.Stats[HealthIndex];
    }
    else
    {
        MaxHealth = 100; 
    }

    if (PlayerInfo.CharStats.Stats.IsValidIndex(DexterityIndex))
    {
        MaxStamina = 100 * PlayerInfo.CharStats.Stats[DexterityIndex];
    }
    else 
    {
        MaxStamina = 100;
    }

    CurrentHealth = MaxHealth;
    CurrentStamina = MaxStamina;
}

void ANetBaseAvatar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsSprinting && CurrentStamina > 0)
    {
        CurrentStamina -= StaminaDrainRate * DeltaTime;

        if (CurrentStamina <= 0)
        {
            CurrentStamina = 0;
            StopSprint();
        }
    }

    if (!bIsSprinting && CurrentStamina < MaxStamina)
    {
        CurrentStamina += StaminaGainRate * DeltaTime;

        if (CurrentStamina > MaxStamina)
        {
            CurrentStamina = MaxStamina;
        }
    }
}

void ANetBaseAvatar::EquipWeapon(TSubclassOf<AWeapon> WeaponClass)
{
    if (!WeaponClass) return; 

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;
        
    EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);    
    
    if (EquippedWeapon)
    {
        EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("hand_rSocket"));
    }
}

void ANetBaseAvatar::Attack()
{
    if (!EquippedWeapon) return;

    FHitResult HitResult;
    FVector Start = EquippedWeapon->GetActorLocation(); 
    FVector End = Start + (GetActorForwardVector() * 200.0f);
    FCollisionShape Capsule = FCollisionShape::MakeCapsule(20.0f, 100.0f); 

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());

    bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult, Start, End, FQuat::Identity, 
        ECC_GameTraceChannel1, Capsule, QueryParams
    );

    // --- DRAW DEBUG SHAPE START ---
    // Calculate the center point of the sweep line to position the debug capsule
    FVector CenterPoint = (Start + End) * 0.5f;
    
    // Choose color: Red if it hit something, Green if it missed
    FColor DebugColor = bHit ? FColor::Red : FColor::Green;

    // Calculate rotation so the capsule aligns with the direction of the swing
    FQuat CapsuleRotation = FRotationMatrix::MakeFromZ(End - Start).ToQuat();

    DrawDebugCapsule(
        GetWorld(),
        CenterPoint,
        100.0f,          // Half-Height (Must match your FCollisionShape)
        20.0f,          // Radius (Must match your FCollisionShape)
        CapsuleRotation,
        DebugColor,
        false,          // bPersistentLines (false means it disappears)
        2.0f            // Duration in seconds the shape stays on screen
    );
    // --- DRAW DEBUG SHAPE END ---

if (bHit && HitResult.GetActor())
{
    ANetBaseZombie* HitEnemy = Cast<ANetBaseZombie>(HitResult.GetActor());
    if (HitEnemy)
    {
        HitEnemy->TakingDamage(20.0f); 
    }
}
}