// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageHitBox.h"
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

	void DealDamage(UPrimitiveComponent* PrimitiveComponent, FVector StartTracePos, FVector EndTracePos, float Damage, USoundBase* SoundBase, float ImpactPauseTime);
	FORCEINLINE TArray<FDamageHitBox> GetAllDamageHitBoxes() const { return DamageHitBoxes; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	
private:
	FTimerHandle ImpactTimer;
	void ImpactPause() const;
	void BoxTrace(const UPrimitiveComponent* PrimitiveComponent, FVector StartTracePos, FVector EndTracePos, FHitResult& HitResult);
	void AttackedGameCharacter(const AActor* ActorOwner, USoundBase* SoundBase, FVector ImpactPoint, const UPrimitiveComponent* PrimitiveComponent, const float ImpactPauseTime);
	static bool DidEnemyHitEnemy(AActor*& ActorOwner, AActor*& ActorHit);

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<AActor>> ActorsToIgnore;
	
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

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<FDamageHitBox> DamageHitBoxes;
};
