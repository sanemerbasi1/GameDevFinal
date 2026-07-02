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
    bUsingMainWeapon = true;

    Camera->bUsePawnControlRotation = false;
    SpringArm->bUsePawnControlRotation = true;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;

    if (HasAuthority())
    {
        EquipWeapon(DefaultWeaponClass);
    }
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

    PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ANetBaseAvatar::Attack);

    PlayerInputComponent->BindAction("SwapWeapon", IE_Pressed, this, &ANetBaseAvatar::SwapWeaponInput);
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
    DOREPLIFETIME(ANetBaseAvatar, EquippedWeapon);
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
    GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? PlayerInfo.CharStats.Stats[(int)ECharStats::Speed] * 700.0f : PlayerInfo.CharStats.Stats[(int)ECharStats::Speed] * 300.0f;
}

void ANetBaseAvatar::SetAvatarValues()
{
    int HealthIndex = (int)ECharStats::Health;
    int DexterityIndex = (int)ECharStats::Dexterity;
    int SpeedIndex = (int)ECharStats::Speed;
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
    if (!HasAuthority() || !WeaponClass) return; 

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;
        
    EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);    
    
    OnRep_EquippedWeapon();
}

void ANetBaseAvatar::Attack()
{
    if (CurrentStamina >= 20)
    {
        ServerAttack();
    }
}

void ANetBaseAvatar::ServerAttack_Implementation()
{
    if (CurrentStamina < 20 || !EquippedWeapon) return;

    float StrengthStat = PlayerInfo.CharStats.Stats[(int)ECharStats::Strenght];
    EquippedWeapon->Attack(this, GetActorForwardVector(), StrengthStat);

    CurrentStamina -= 20;

    MulticastPlayAttackVisuals(EquippedWeapon->AttackMontage);
}

void ANetBaseAvatar::TakingDamage(float Damage)
{
    if (!HasAuthority()) return;

    CurrentHealth -= Damage;

    if (CurrentHealth <= 0)
    {
        CurrentHealth = 0;
        OnDeath();
    }
    OnHealthChanged(CurrentHealth);
}

void ANetBaseAvatar::MulticastPlayAttackVisuals_Implementation(UAnimMontage* MontageToPlay)
{
    if (MontageToPlay)
    {
        PlayAnimMontage(MontageToPlay, 1.0f);
    }
}

void ANetBaseAvatar::SwapWeaponInput()
{
    ServerSwapWeapon();
}

void ANetBaseAvatar::ServerSwapWeapon_Implementation()
{
    bUsingMainWeapon = !bUsingMainWeapon;
    TSubclassOf<AWeapon> TargetClass = bUsingMainWeapon ? DefaultWeaponClass : SecondaryWeaponClass;

    if (EquippedWeapon)
    {
        EquippedWeapon->Destroy();
    }

    EquipWeapon(TargetClass);
}

void ANetBaseAvatar::OnRep_EquippedWeapon()
{
    if (EquippedWeapon)
    {
        EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("hand_rSocket"));
    }
}

