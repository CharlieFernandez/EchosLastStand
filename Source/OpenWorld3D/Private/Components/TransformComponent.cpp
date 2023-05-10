// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TransformComponent.h"

UTransformComponent::UTransformComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UTransformComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UTransformComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

}