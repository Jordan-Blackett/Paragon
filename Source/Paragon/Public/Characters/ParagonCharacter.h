// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "ParagonAttributeSet.h"
#include "ParagonCharacter.generated.h"

UCLASS(config=Game)
class AParagonCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	////////////////////////////////////
	// --- Components ---

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Collection sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollectionSphere;

	/** Healthbar widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UParagonWidgetComponent* HealthBarWidgetComp;

public:
	////////////////////////////////////
	// --- Constructor And Overrides ---

	AParagonCharacter();
	virtual void BeginPlay();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	//virtual void OnRep_Controller() override;

	////////////////////////////////////
	// --- Functions ---

	void TakeDamageEffectSpecs(TArray<FGameplayEffectSpecHandle>& TargetGameplayEffectSpecs);

	////////////////////////////////////
	// --- Helper Functions ---

	void GetViewPoint(FVector& CameraLocation, FRotator& CamRotation);
	bool GetIsLocallyControlled();
	
	////////////////////////////////////
	// --- Accessors And Mutators ---

	// Health

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetHealth() const;

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetHealthRegen() const;

	// Mana

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetMana() const;

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetMaxMana() const;

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetManaRegen() const;

	// Attack

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetAttackDamage() const;

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetAttackSpeed() const;

	// Defense

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetAbilityDefense() const;

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetBaseDefense() const;

	// Movement speed

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetMoveSpeed() const;

	/** Returns the character level that is passed to the ability system */
	UFUNCTION(BlueprintPure, Category = "Stats")
	int32 GetCharacterLevel() const;

	/** Modifies the character level, this may change abilities. Returns true on success */
	//UFUNCTION(BlueprintCallable, Category = "Stats")
	//bool SetCharacterLevel(int32 NewLevel);

	// Abilities

	UFUNCTION(BlueprintPure)
	FVector GetAbilityPoint() { return AbilityPoint; }

protected:
	/** The level of this character, should not be modified directly once it has already spawned */
	UPROPERTY(EditAnywhere, Replicated, Category = Abilities)
	int32 CharacterLevel;

	////////////////////////////////////
	// --- Ability System ---

	/** Ability system - Used to handle ability system interactions */
	UPROPERTY()
	class UAbilitySystemComponent* AbilitySystem;

	/** List of attributes modified by the ability system */
	UPROPERTY()
	UParagonAttributeSet* AttributeSet;

	/** Abilities */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TArray<TSubclassOf<class UGameplayAbility>> AbilitiesSlots;
	
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitiesHandles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<UMaterialInterface*> AbilitiesIndicator;

	/** Passive gameplay effects applied on creation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	UPROPERTY()
	bool bAbilityPressed;

	UPROPERTY()
	int CurrentActiveAbility;

	UFUNCTION()
	void StartAbility();

	UFUNCTION()
	void AbilitySlot0();

	UFUNCTION()
	void AbilitySlot1();

	UFUNCTION()
	void AbilitySlot2();

	UFUNCTION()
	void AbilitySlot3();

	UFUNCTION()
	void AbilitySlot4();

	UFUNCTION()
	void AbilitySlot5();

	UFUNCTION()
	void ActivateAbilityInSlot(int32 Slot);

	//UPROPERTY()
	int32 bAbilitiesInitialized;

	UPROPERTY()
	FTimerHandle RegenTimerHandle;

	////////////////////////////////////
	// --- ---

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FName StatBarAttachPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> FloatingDamageTextWidgetTemplate;

	UPROPERTY(Replicated)
	FVector AbilityPoint;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY()
	class ADecalActor* CurrentAbilityDecal = nullptr;

	UPROPERTY()
	FTimerHandle IndicatorTimerHandle;

	////////////////////////////////////
	// --- Ability System ---

	/** Apply the startup gameplay abilities and effects */
	void InitGameplayAbilities();

	// Called from RPGAttributeSet, these call BP events above
	//virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ARPGCharacterBase* InstigatorCharacter, AActor* DamageCauser);
	//virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	//virtual void HandleManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	//virtual void HandleMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	// Friended to allow access to handle functions above
	//friend URPGAttributeSet;

	////////////////////////////////////
	// --- Functions ---

	// Movement

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

	////////////////////////////////////
	// --- Helper Functions ---

	void FloatingDamageText(float Damage);

	FHitResult Linetrace();

	void SpawnAbilityIndicator(UMaterialInterface* AbilityIndicator);
	void MoveAbilityIndicator();
	void HideAbilityIndicator();

	void Regen();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns CollectionSphere subobject **/
	FORCEINLINE class USphereComponent* GetCollectionSphere() const { return CollectionSphere; }

	UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystem; };
};

