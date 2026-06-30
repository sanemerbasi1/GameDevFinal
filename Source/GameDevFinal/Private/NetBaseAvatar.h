// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetBaseCharacter.h"
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
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(BlueprintReadOnly) 
	int MaxHealth;

	UPROPERTY(BlueprintReadOnly)
	int CurrentHealth;

	UPROPERTY(BlueprintReadOnly)
	int MaxStamina;

	UPROPERTY(BlueprintReadOnly)
	int CurrentStamina;

	UFUNCTION(BlueprintCallable)
	void SetAvatarValues();

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
};