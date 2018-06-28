// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Public/CollisionQueryParams.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Engine.h"
#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();

}



void UGrabber::FindPhysicsHandleComponent()
{
	///look for attached physics handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle) {
		//is physics handle is found
		UE_LOG(LogTemp, Warning, TEXT("physics handle found"))
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s missing physics handle component"), *GetOwner()->GetName())
	}
}

void UGrabber::SetupInputComponent()
{
	///look for attached input component (only appears at runtime)
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) {
		//is input component is found
		UE_LOG(LogTemp, Warning, TEXT("Input component found"))
		
		///Bind the input axis
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s missing input component"), *GetOwner()->GetName())
	}
}


void UGrabber::Grab() {
	//ray cast and grab what's in reach
	UE_LOG(LogTemp, Warning, TEXT("Grab key pressed"))

	///LINE TRACE and see if we reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();  //gets the mesh in our case
	auto ActorHit = HitResult.GetActor();

	///if we hit something then attach a physics handle
	if (ActorHit)
	{
		UE_LOG(LogTemp, Error, TEXT("you have hit an actor!"))
		
		PhysicsHandle->GrabComponent(   // attach physics handle
			ComponentToGrab,     
			NAME_None,    //no bones needed
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true // allow rotation
		);
	}
}

void UGrabber::Release() {
	UE_LOG(LogTemp, Warning, TEXT("Grab key released"))
	PhysicsHandle->ReleaseComponent();
}

/// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (PhysicsHandle->GrabbedComponent)
	{
		//move the object that we're holding
		PhysicsHandle->SetTargetLocation(GetReachLineEnd());
	}

}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	
	///Setup query parameters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	///Line-trace (aka Ray-cast) out to reach distance
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		GetReachLineStart(),
		GetReachLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	///See what we hit
	AActor* ActorHit = HitResult.GetActor();
	if (ActorHit) {
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit: %s"), *(ActorHit->GetName()))
	}
	
	return HitResult;
}

FVector UGrabber::GetReachLineStart()
{
	/// Get player view point this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	//this is the end of the player reach line
	return PlayerViewPointLocation;
}

FVector UGrabber::GetReachLineEnd()
{
	/// Get player view point this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	//this is the end of the player reach line
	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}
