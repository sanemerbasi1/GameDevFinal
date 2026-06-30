// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetBaseCharacter.h"
#include "NetBaseZombie.generated.h"

/**
 * 
 */
UCLASS()
class ANetBaseZombie : public ANetBaseCharacter
{
	GENERATED_BODY()

public:

UFUNCTION(BlueprintCallable)
void TakingDamage(float Damage);
	
};
