// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TransformComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLD3D_API UTransformComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	UTransformComponent();
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDefaultDelegate);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FDefaultDelegate TransformToFirstForm;

	UPROPERTY(BlueprintAssignable)
	FDefaultDelegate TransformToSecondForm;

protected:
	virtual void BeginPlay() override;
};
