// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetBaseCharacter.h"
#include "NetBaseZombie.generated.h"

class UWidgetComponent;

/**
 * 
 */
UCLASS()
class ANetBaseZombie : public ANetBaseCharacter
{
	GENERATED_BODY()

public:

ANetBaseZombie();

UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
class UWidgetComponent* HealthBarWidget;

UFUNCTION(BlueprintImplementableEvent)
void OnHealthChanged(float NewHealth);

UFUNCTION(BlueprintCallable)
void FollowPlayer(ANetBaseCharacter* Player);


UPROPERTY(BlueprintReadOnly)
float AttackDamage = 30;

virtual void Attack() override;
virtual void TakingDamage(float Damage) override;

protected:
virtual void UpdateBodyParts() override;
	
};
