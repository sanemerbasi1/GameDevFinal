// Fill out your copyright notice in the Description page of Project Settings.
#include "NetGameInstance.h"
#include "Kismet/GameplayStatics.h"


void UNetGameInstance::Host(FString MapName, FSPlayerInfo Info)
{
    GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, TEXT("Hosting Game..."));
    PlayerInfo = Info;
    UGameplayStatics::OpenLevel(GetWorld(), FName(*MapName), true, "?listen");}

void UNetGameInstance::Join(FString Address, FSPlayerInfo Info)
{
    GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Green, 
    FString::Printf(TEXT("Joining Game at %s..."),*Address));
    PlayerInfo = Info;
    GWorld->GetFirstPlayerController()->ClientTravel(Address,
    ETravelType::TRAVEL_Absolute);
}
void UNetGameInstance::LeaveGame(FString LeaveMapName)
{
    PlayerInfo.CharStats.Stats.Init(1, (int)ECharStats::COUNT);

    GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, TEXT("Returning to Main Menu..."));
    UGameplayStatics::OpenLevel(GetWorld(), FName(*LeaveMapName), true);

}