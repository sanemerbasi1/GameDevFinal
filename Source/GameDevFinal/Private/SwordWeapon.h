// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "SwordWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ASwordWeapon : public AWeapon
{
	GENERATED_BODY()

	public:
	virtual void Attack(AActor* Attacker, FVector ForwardVector, float StrengthStat) override;
	
};
