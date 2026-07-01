// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetBaseCharacter.h"
#include "Weapon.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "NetBaseAvatar.generated.h"

/**
 * 
 */
UCLASS()
class ANetBaseAvatar : public ANetBaseCharacter
{
	GENERATED_BODY()

public:

	ANetBaseAvatar();

	UPROPERTY(EditAnywhere)
    TSubclassOf<AWeapon> DefaultWeaponClass;

    UFUNCTION(BlueprintCallable)
    void EquipWeapon(TSubclassOf<AWeapon> WeaponClass);

	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArm;

	//UPROPERTY(BlueprintReadOnly) 
	//float MaxHealth;

	//UPROPERTY(BlueprintReadOnly)
	//float CurrentHealth;

	UPROPERTY(BlueprintReadOnly)
	float MaxStamina;

	UPROPERTY(BlueprintReadOnly)
	float CurrentStamina;

	UPROPERTY(BlueprintReadOnly)
	float StaminaDrainRate;

	UPROPERTY(BlueprintReadOnly)
    float StaminaGainRate;

	UFUNCTION(BlueprintCallable)
	void SetAvatarValues();

	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float NewHealth);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeath();

	UFUNCTION(Server, Reliable)
	void ServerAttack();

	virtual void Attack() override;
	virtual void TakingDamage(float Damage) override;

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void MoveForward(float Amount);
	void MoveRight(float Amount);

	void StartSprint();
	void StopSprint();

protected:
	UPROPERTY(ReplicatedUsing=OnRep_IsSprinting, BlueprintReadOnly)
	bool bIsSprinting;

	UFUNCTION()
	void OnRep_IsSprinting();

	UFUNCTION(Server, Reliable)
	void ServerSetSprinting(bool bNewSprinting);

	UPROPERTY()
    AWeapon* EquippedWeapon;

public:

virtual void Tick(float DeltaTime) override;
};