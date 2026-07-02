// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "BowWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ABowWeapon : public AWeapon
{
	GENERATED_BODY()

	public:
	virtual void Attack(AActor* Attacker, FVector ForwardVector, float StrengthStat) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<AActor> ProjectileBP;
};
