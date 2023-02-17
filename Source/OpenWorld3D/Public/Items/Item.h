// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class OPENWORLD3D_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//BlueprintCallable || BlueprintPure
	UFUNCTION(BlueprintPure)
	float TransformSine();

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;

	// 1st Argument: EditDefaultsOnly || EditInstanceOnly || EditAnywhere || VisibleDefaultsOnly || VisibleInstanceOnly || VisibleAnywhere
	// 2nd Argument: BlueprintReadOnly || BlueprintReadWrite
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters", meta = (AllowPrivateAccess = "true"))
	float Amplitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters", meta = (AllowPrivateAccess = "true"))
	float TimeConstant;
};
