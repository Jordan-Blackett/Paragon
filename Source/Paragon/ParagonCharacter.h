// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "ParagonCharacter.generated.h"

UENUM(BlueprintType)
enum class AbilityInput : uint8
{
	Ability_1 UMETA(DisplayName = "Use Spell 1"),
	UseAbility2 UMETA(DisplayName = "Use Spell 2"),
	UseAbility3 UMETA(DisplayName = "Use Spell 3"),
	UseAbility4 UMETA(DisplayName = "Use Spell 4"),
	WeaponAbility UMETA(DisplayName = "Use Weapon"), //This finally maps the fifth ability(here designated to be your weaponability, or auto-attack, or whatever) to action mapping "WeaponAbility".

													 //You may also do something like define an enum element name that is not actually mapped to an input, for example if you have a passive ability that isn't supposed to have an input. This isn't usually necessary though as you usually grant abilities via input ID,
													 //which can be negative while enums cannot. In fact, a constant called "INDEX_NONE" exists for the exact purpose of rendering an input as unavailable, and it's simply defined as -1.
													 //Because abilities are granted by input ID, which is an int, you may use enum elements to describe the ID anyway however, because enums are fancily dressed up ints.
};

UCLASS(config=Game)
class AParagonCharacter : public ACharacter, public IAbilitySystemInterface
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

	// Healthbar
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UParagonWidgetComponent* HealthBarWidgetComp;

protected:

	/** Our ability system */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	class UAbilitySystemComponent* AbilitySystem;

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

	UFUNCTION(BlueprintPure, Category = "Stats")
	FVector GetAbilityPoint() { return AbilityPoint; }

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

	virtual void PossessedBy(AController * NewController) override;

	// --- Character Initial Stats ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float InitialHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float InitialMana;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FName StatBarAttachPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> FloatingDamageTextWidgetTemplate;

	FVector AbilityPoint;


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

	UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystem; };
};

