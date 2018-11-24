// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ParagonCharacter.generated.h"

UCLASS(config=Game)
class AParagonCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Collection sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollectionSphere;

	// Statbar
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UParagonWidgetComponent* HealthBarWidgetComp;

public:
	AParagonCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	////////////////////////
	// Helper functions

	void GetViewPoint(FVector& CameraLocation, FRotator& CamRotation);
	bool GetIsLocallyControlled();
	
	// --- Accessor and Mutator ---

	// Accessor function for initial health
	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetInitialHealth();

	// Accessor function for current health 
	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetCurrentHealth();

	UFUNCTION()
	void SetCurrentHealth(float NewHealth);

	// Accessor function for current health 
	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetHealthPercent();

	// Accessor function for initial health
	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetInitialMana();

	// Accessor function for current health 
	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetCurrentMana();

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	void RotateToCrossHair();

	// Helper functions

	void FloatingDamageText(float Damage);

	FHitResult Linetrace();

	void SpawnAbilityIndicator(UMaterialInterface* AbilityIndicator);
	void MoveAbilityIndicator();
	void HideAbilityIndicator();

protected:
	virtual void BeginPlay();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	// --- Character Initial Stats ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float InitialHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float InitialMana;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FName StatBarAttachPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> FloatingDamageTextWidgetTemplate;

	// OnRep

	UFUNCTION()
	virtual void OnRep_Health(float OldHealth);

private:

	// --- Character Current Stats ---

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Health, Category = "Stats")
	float CurrentHealth;

	UPROPERTY(VisibleAnywhere, Category = "Stats")
	float CurrentMana;

	float testSlowing;

	UPROPERTY()
	class ADecalActor* CurrentAbilityDecal = nullptr;

	UPROPERTY()
	FTimerHandle IndicatorTimerHandle;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns CollectionSphere subobject **/
	FORCEINLINE class USphereComponent* GetCollectionSphere() const { return CollectionSphere; }
};

