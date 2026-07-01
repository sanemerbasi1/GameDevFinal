#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "Net/UnrealNetwork.h"
#include "NetBaseCharacter.generated.h" 

	UENUM(BlueprintType)
	enum class EBodyPart : uint8
	{
		BP_Face = 0,
		BP_Hair = 1,
		BP_Chest = 2,
		BP_Hands = 3,
		BP_Legs = 4,
		BP_Beard = 5,
		BP_Eyebrows = 6,
		BP_COUNT = 7,
	};

	UENUM(BlueprintType)
	enum class ECharStats : uint8
	{
		Health = 0,
		Strenght = 1,
		Dexterity = 2,
		Intelligence = 3,
		COUNT = 4,
	};

	USTRUCT(BlueprintType)
	struct FSMeshAssetList : public FTableRowBase
	{
		GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<USkeletalMesh*> ListSkeletal;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UStaticMesh*> ListStatic;
	};

	USTRUCT(BlueprintType)
	struct FSBodyPartSelection
	{
		GENERATED_USTRUCT_BODY()

		UPROPERTY()
		int Indices[(int)EBodyPart::BP_COUNT];

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isFemale;
	};

	USTRUCT(BlueprintType)
	struct FSCharStatsSelection
	{
		GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
    	TArray<int> Stats;

	};

	USTRUCT(BlueprintType)
	struct FSPlayerInfo
	{
		GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Nickname;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSBodyPartSelection BodyParts;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSCharStatsSelection CharStats;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int RemainingStatPoints = 5;

		UPROPERTY(BlueprintReadWrite)
		bool Ready = false;
	};

UCLASS()
class ANetBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANetBaseCharacter();

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void ChangeBodyPart(EBodyPart index, int value, bool DirectSet);

	UFUNCTION(BlueprintCallable)
	void ChangeCharStat(ECharStats index, int value, bool DirectSet, int& NewValue);

	UFUNCTION(BlueprintCallable)
	void ChangeGender(bool isFemale);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PlayerInfoChanged)
	FSPlayerInfo PlayerInfo;

	UFUNCTION(Server, Reliable)
	void SubmitPlayerInfoToServer(FSPlayerInfo Info);

	//UFUNCTION(BlueprintCallable, Server, Reliable)
	//void Server_RestartStatValues();

	UFUNCTION()
	void OnRep_PlayerInfoChanged();

	UFUNCTION(BlueprintCallable)
	virtual void Attack();

	UFUNCTION(BlueprintCallable)
	virtual void TakingDamage(float Damage);

	UPROPERTY(BlueprintReadOnly)
	float CurrentHealth;

	UPROPERTY(BlueprintReadOnly) 
	float MaxHealth;


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* PartFace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PartHair;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PartBeard;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PartEyes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* PartHands;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* PartLegs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PartEyebrows;

	static FSMeshAssetList* GetBodyPartList(EBodyPart part, bool isFemale);


protected:
virtual void UpdateBodyParts();
};