// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#define OUT


// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	AActor* Owner = GetOwner();
}

void UOpenDoor::OpenDoor()
{
	//Find owning actor
	AActor* Owner = GetOwner();

	//create rotator
	FRotator NewRotation = FRotator(0.0f, 90.0f, 0.0f);

	//set the door rotation
	Owner->SetActorRotation(NewRotation);
}

void UOpenDoor::CloseDoor()
{
	//Find owning actor
	AActor* Owner = GetOwner();

	//create rotator
	FRotator NewRotation = FRotator(0.0f, 0.0f, 0.0f);

	//set the door rotation
	Owner->SetActorRotation(NewRotation);
}



// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	//if the actor that opens is in the volume 
	if (GetTotalMassOfActorsOnPlate() > 30.f) {        //TODO make into a parameter
		OpenDoor();
		LastDoorOpenTime = GetWorld()->GetTimeSeconds();
	}
	
	//if door has been open for longer than delay, close door
	if (GetWorld()->GetTimeSeconds() - LastDoorOpenTime >= DoorCloseDelay) {
		CloseDoor();
	}
	
}

float UOpenDoor::GetTotalMassOfActorsOnPlate()
{
	float TotalMass = 0.f;

	//Find all the overlapping Actors
	TArray<AActor*> OverlappingActors;

	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	//iterate through them adding their masses
	for (const auto& Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("%s on pressure plate"), *Actor->GetName());
	}
	
	return TotalMass;

}

