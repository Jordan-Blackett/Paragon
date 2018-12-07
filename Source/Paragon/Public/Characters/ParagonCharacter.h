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

	/**
	* Attempts to activate all abilities that match the specified tags
	* Returns true if it thinks it activated, but it may return false positives due to failure later in activation.
	* If bAllowRemoteActivation is true, it will remotely activate local/server abilities, if false it will only try to locally activate the ability
	*/
	//UFUNCTION(BlueprintCallable, Category = "Abilities")
	//bool ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation = true);

	/** Returns a list of active abilities matching the specified tags. This only returns if the ability is currently running */
	//UFUNCTION(BlueprintCallable, Category = "Abilities")
	//void GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<URPGGameplayAbility*>& ActiveAbilities);

	/** Returns total time and remaining time for cooldown tags. Returns false if no active cooldowns found */
	//UFUNCTION(BlueprintCallable, Category = "Abilities")
	//bool GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration);

	////////////////////////////////////
	// --- Helper Functions ---

	void GetViewPoint(FVector& CameraLocation, FRotator& CamRotation);
	bool GetIsLocallyControlled();
	
	////////////////////////////////////
	// --- Accessors And Mutators ---

	// Health

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetMaxHealth() const;

	//UFUNCTION(BlueprintPure, Category = "Stats")
	//float GetHealthPercent();

	// Mana

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetCurrentMana() const;

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetMaxMana() const;

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

	/**
	* Called when character takes damage, which may have killed them
	*
	* @param DamageAmount Amount of damage that was done, not clamped based on current health
	* @param HitInfo The hit info that generated this damage
	* @param DamageTags The gameplay tags of the event that did the damage
	* @param InstigatorCharacter The character that initiated this damage
	* @param DamageCauser The actual actor that did the damage, might be a weapon or projectile
	*/
	//UFUNCTION(BlueprintImplementableEvent)
	//void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ARPGCharacterBase* InstigatorCharacter, AActor* DamageCauser);

	/**
	* Called when health is changed, either from healing or from being damaged
	* For damage this is called in addition to OnDamaged/OnKilled
	*
	* @param DeltaValue Change in health value, positive for heal, negative for cost. If 0 the delta is unknown
	* @param EventTags The gameplay tags of the event that changed mana
	*/
	//UFUNCTION(BlueprintImplementableEvent)
	//void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/**
	* Called when mana is changed, either from healing or from being used as a cost
	*
	* @param DeltaValue Change in mana value, positive for heal, negative for cost. If 0 the delta is unknown
	* @param EventTags The gameplay tags of the event that changed mana
	*/
	//UFUNCTION(BlueprintImplementableEvent)
	//void OnManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/**
	* Called when movement speed is changed
	*
	* @param DeltaValue Change in move speed
	* @param EventTags The gameplay tags of the event that changed mana
	*/
	//UFUNCTION(BlueprintImplementableEvent)
	//void OnMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

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

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns CollectionSphere subobject **/
	FORCEINLINE class USphereComponent* GetCollectionSphere() const { return CollectionSphere; }

	UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystem; };
};

