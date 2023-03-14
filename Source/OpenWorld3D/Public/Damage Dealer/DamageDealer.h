// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageDealer.generated.h"


class UFieldSystemMetaDataFilter;
class URadialVector;
class URadialFalloff;
class UFieldSystemComponent;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLD3D_API UDamageDealer : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDamageDealer();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DealDamage(UPrimitiveComponent* PrimitiveComponent, FVector StartTracePos, FVector EndTracePos, float Damage, USoundBase* SoundBase);

	// UFUNCTION(BlueprintPure)
	// FORCEINLINE UFieldSystemComponent* GetFieldSystemComponent() const { return FieldSystemComponent; }
	//
	// UFUNCTION(BlueprintPure)
	// FORCEINLINE URadialFalloff* GetRadialFalloff() const {return RadialFalloff; }
	//
	// UFUNCTION(BlueprintPure)
	// FORCEINLINE URadialVector* GetRadialVector() const { return RadialVector; }
	//
	// UFUNCTION(BlueprintPure)
	// FORCEINLINE UFieldSystemMetaDataFilter* GetFSMetaDataFilter() const { return FieldSystemMetaDataFilter; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);
	
private:
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<AActor>> ActorsToIgnore;
	FTimerHandle ImpactTimer;
	
	void ImpactPause() const;

	UFUNCTION(BlueprintCallable)
	void SetHitBoxCollisionType(TArray<UPrimitiveComponent*> PrimitiveComponents, ECollisionEnabled::Type CollisionType);

	// Field System Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFieldSystemComponent> FieldSystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URadialFalloff> RadialFalloff;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URadialVector> RadialVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFieldSystemMetaDataFilter> FieldSystemMetaDataFilter;
};
