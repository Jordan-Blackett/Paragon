// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ParagonCharacter.h"
#include "Paragon.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/Engine.h"
#include "Math/Vector.h"
#include "UMG/Public/Blueprint/UserWidget.h"
#include "ParagonWidgetComponent.h"
#include "ParagonUserWidget.h"
#include "ParagonWidget_FloatingDamageText.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "ParagonDebugConsoleVariables.h"

extern TAutoConsoleVariable<int32> CVarDebugBasicAttack;

//////////////////////////////////////////////////////////////////////////
// AParagonCharacter

AParagonCharacter::AParagonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create the collection sphere
	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionShpere"));
	CollectionSphere->AttachTo(RootComponent);
	CollectionSphere->SetSphereRadius(200.0f);

	// Create widget comp
	HealthBarWidgetComp = CreateDefaultSubobject<UParagonWidgetComponent>(TEXT("WidgetComponent"));
	HealthBarWidgetComp->SetupAttachment(GetMesh(), StatBarAttachPoint);
	HealthBarWidgetComp->InitWidget();

	// Our ability system component.
	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);

	// Create the attribute set, this replicates by default
	AttributeSet = CreateDefaultSubobject<UParagonAttributeSet>(TEXT("AttributeSet"));

	CharacterLevel = 1;
	Experience = 0;
	bAbilitiesInitialized = false;
}

void AParagonCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Initialize our abilities
	if (AbilitySystem)
	{
		InitGameplayAbilities();
		AbilitySystem->InitAbilityActorInfo(this, this);
	}

	if (Role == ROLE_Authority)
	{
		// Regen
		GetWorldTimerManager().SetTimer(RegenTimerHandle, this, &AParagonCharacter::Regen, 1.f, true);
	}
}

void AParagonCharacter::InitGameplayAbilities()
{
	check(AbilitySystem);

	if (Role == ROLE_Authority && !bAbilitiesInitialized)
	{
		// Grant abilities, but only on the server	
		for (TSubclassOf<UGameplayAbility>& Abilities : AbilitiesSlots)
		{
			AbilitiesHandles.Add(AbilitySystem->GiveAbility(FGameplayAbilitySpec(Abilities, GetCharacterLevel(), INDEX_NONE, this)));
		}

		// Now apply passives
		for (TSubclassOf<UGameplayEffect>& GameplayEffect : PassiveGameplayEffects)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystem->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle NewHandle = AbilitySystem->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
			if (NewHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystem->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystem);
			}
		}

		AbilitySystem->InitAbilityActorInfo(this, this);

		bAbilitiesInitialized = true;
	}
}

void AParagonCharacter::RemoveStartupGameplayAbilities()
{
	check(AbilitySystem);

	if (Role == ROLE_Authority && bAbilitiesInitialized)
	{
		// Remove any abilities added from a previous call
		TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
		for (const FGameplayAbilitySpec& Spec : AbilitySystem->GetActivatableAbilities())
		{
			if ((Spec.SourceObject == this) && AbilitiesSlots.Contains(Spec.Ability->GetClass()))
			{
				AbilitiesToRemove.Add(Spec.Handle);
			}
		}

		// Do in two passes so the removal happens after we have the full list
		for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
		{
			AbilitySystem->ClearAbility(AbilitiesToRemove[i]);
		}

		// Remove all of the passive gameplay effects that were applied by this character
		FGameplayEffectQuery Query;
		Query.EffectSource = this;
		AbilitySystem->RemoveActiveEffects(Query);

		bAbilitiesInitialized = false;
	}
}

void AParagonCharacter::HandleDamage(float DamageAmount, const FHitResult & HitInfo, const FGameplayTagContainer & DamageTags, AParagonCharacter * InstigatorCharacter, AActor * DamageCauser, bool DamageType)
{
	FloatingDamageText(DamageAmount, DamageType);
}

void AParagonCharacter::HandleHealthChanged(float DeltaValue, const FGameplayTagContainer & EventTags)
{

}

void AParagonCharacter::HandleManaChanged(float DeltaValue, const FGameplayTagContainer & EventTags)
{
}

void AParagonCharacter::HandleMoveSpeedChanged(float DeltaValue, const FGameplayTagContainer & EventTags)
{
}

//////////////////////////////////////////////////////////////////////////
// Input

void AParagonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AParagonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AParagonCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AParagonCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AParagonCharacter::LookUpAtRate);

	// Set up Ability key bindings
	//AbilitySystem->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds("ConfirmInput", "CancelInput", "AbilityInput"));
	PlayerInputComponent->BindAction("Ability_0", IE_Pressed, this, &AParagonCharacter::AbilitySlot0);
	PlayerInputComponent->BindAction("Ability_1", IE_Pressed, this, &AParagonCharacter::AbilitySlot1);
	PlayerInputComponent->BindAction("Ability_2", IE_Pressed, this, &AParagonCharacter::AbilitySlot2);
	PlayerInputComponent->BindAction("Ability_3", IE_Pressed, this, &AParagonCharacter::AbilitySlot3);
	PlayerInputComponent->BindAction("Ability_4", IE_Pressed, this, &AParagonCharacter::AbilitySlot4);
}

void AParagonCharacter::PossessedBy(AController * NewController)
{
	Super::PossessedBy(NewController);

	//// Initialize our abilities
	//if (AbilitySystem)
	//{
	//	AbilitySystem->InitAbilityActorInfo(this, this);
	//	//AddStartupGameplayAbilities();
	//}

	AbilitySystem->RefreshAbilityActorInfo();
}

void AParagonCharacter::TakeDamageEffectSpecs(TArray<FGameplayEffectSpecHandle>& TargetGameplayEffectSpecs)
{
	if (Role == ROLE_Authority) {
		//Decrease the character's hp - Apply gameplay effect
		for (const FGameplayEffectSpecHandle& SpecHandle : TargetGameplayEffectSpecs)
		{
			if (SpecHandle.IsValid())
			{
				AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}

void AParagonCharacter::ActivateAbilityInSlot(int32 Slot)
{
	if (AbilitySystem)
	{
		if (AbilitySystem && AbilitiesSlots.Num() > Slot)
		{
			if (AbilitiesSlots[Slot] != nullptr)
			{
				AbilitySystem->TryActivateAbilityByClass(AbilitiesSlots[Slot], true);
			}
		}
	}

	//AbilitySystem->TryActivateAbilityByClass(AbilitySystem->ActiveSlots[Slot]);
	// Cast ability
	//if (AbilitySystem && AbilitiesHandles.Num() > Slot)
	//{
	//	if (AbilitySystem->TryActivateAbility(AbilitiesHandles[Slot])) {
	//	} else {
	//		UE_LOG(LogTemp, Warning, TEXT("Ability Failed"));
	//	}
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("No Ability In Array"));
	//}
}

void AParagonCharacter::StartAbility()
{
	if (!bAbilityPressed)
	{
		// Spawn decal / change decal
		if (AbilitiesIndicator.Num() > CurrentActiveAbility)
		{
			SpawnAbilityIndicator(AbilitiesIndicator[CurrentActiveAbility]);
		}

		bAbilityPressed = true;
	}
}

void AParagonCharacter::AbilitySlot0()
{
	if (AbilitySystem && AbilitiesSlots.Num() > 0)
	{
		if (AbilitiesSlots[0] != nullptr)
		{
			if (bAbilityPressed)
			{
				HideAbilityIndicator();

				ActivateAbilityInSlot(CurrentActiveAbility);

				bAbilityPressed = false;
			}
			else
			{
				CurrentActiveAbility = 0;
				ActivateAbilityInSlot(CurrentActiveAbility);
			}
		}
	}
}

void AParagonCharacter::AbilitySlot1()
{
	CurrentActiveAbility = 1;
	StartAbility();
}

void AParagonCharacter::AbilitySlot2()
{
	ActivateAbilityInSlot(2);
}

void AParagonCharacter::AbilitySlot3()
{
	ActivateAbilityInSlot(3);
}

void AParagonCharacter::AbilitySlot4()
{
	ActivateAbilityInSlot(4);
}

void AParagonCharacter::AbilitySlot5()
{
	ActivateAbilityInSlot(5);
}

void AParagonCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AParagonCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AParagonCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		////test slowing - TODO: Find a better way
		//// Slowing fwd to bwd
		//if (Value > 0 && Value > testSlowing)
		//{
		//	testSlowing += 0.02f;
		//}
		//// Slowing bwd to fwd
		//else if (Value < 0 && Value < testSlowing)
		//{
		//	testSlowing -= 0.02f;
		//}
		//else
		//{
		//}

		GetCharacterMovement()->bOrientRotationToMovement = true;
		// Walk backwards face forwards
		if (Value < 0)
		{
			FRotator YawRotation(0, GetViewRotation().Yaw, 0);
			SetActorRotation(YawRotation);
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}

		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value); // Value testSlowing	
	}
}

void AParagonCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AParagonCharacter::RotateToCrossHair()
{
	if ((Controller != NULL))
	{
		// Compare player and camera forward vector to decide if the player is looking away
		FVector PlayerForwardVector = GetActorForwardVector();
		PlayerForwardVector.Normalize();

		FVector CameraForwardVector = GetFollowCamera()->GetForwardVector();
		CameraForwardVector.Normalize();

		float AngleInDegree = FMath::Acos(FVector::DotProduct(PlayerForwardVector, CameraForwardVector));
		AngleInDegree = FMath::RadiansToDegrees(AngleInDegree);

		if (AngleInDegree >= 140)
		{
			FRotator YawRotation(0, GetViewRotation().Yaw, 0);
			SetActorRotation(YawRotation);

			//AddMovementInput(GetActorForwardVector(), 1);
		}
	}
}

void AParagonCharacter::GetViewPoint(FVector & CameraLocation, FRotator & CamRotation)
{
	if (GetController())
	{
		GetController()->GetPlayerViewPoint(CameraLocation, CamRotation);
	}
}

FHitResult AParagonCharacter::LinetraceFromCamera(float LineTraceRange) // TODO
{
	FVector CamLoc;
	FRotator CamRot;
	GetViewPoint(CamLoc, CamRot);

	FVector ShootDir = CamRot.Vector();
	FVector Origin = GetMesh()->GetSocketLocation("Gun_LOS"); // TODO 

	const float AdjustRange = LineTraceRange;
	const FVector StartTrace = Origin;
	const FVector EndTrace = StartTrace + ShootDir * AdjustRange;

	//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, true);

	return Linetrace(StartTrace, EndTrace);
}

FHitResult AParagonCharacter::LinetraceFromSocket(FName SocketName, float LineTraceRange)
{
	FVector CamLoc;
	FRotator CamRot;
	GetViewPoint(CamLoc, CamRot);

	FVector ShootDir = CamRot.Vector();
	FVector Origin = GetMesh()->GetSocketLocation(SocketName);

	const float AdjustRange = LineTraceRange;
	const FVector StartTrace = Origin;
	const FVector EndTrace = StartTrace + ShootDir * AdjustRange;

	DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, true);

	return Linetrace(StartTrace, EndTrace);
}

FHitResult AParagonCharacter::LinetraceFromSocketOut(FName SocketName, float LineTraceRange, FVector & OutShootDir, FVector & OutOrigin)
{
	FVector CamLoc;
	FRotator CamRot;
	GetViewPoint(CamLoc, CamRot);

	FVector ShootDir = CamRot.Vector();
	FVector Origin = GetMesh()->GetSocketLocation(SocketName);

	// Trace from camera to check what's under crosshair
	const float AdjustRange = LineTraceRange; // Cross point
	const FVector StartTrace = CamLoc + ShootDir * ((GetActorLocation() - CamLoc) | ShootDir);
	const FVector EndTrace = StartTrace + ShootDir * AdjustRange;
	
	int32 Debug = CVarDebugBasicAttack.GetValueOnGameThread();
	if (Debug)
	{
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, true);
	}

	FHitResult Impact = Linetrace(StartTrace, EndTrace);

	// and adjust directions to hit that actor
	if (Impact.bBlockingHit)
	{
		const FVector AdjustedDir = (Impact.ImpactPoint - Origin).GetSafeNormal();
		bool bWeaponPenetration = false;

		const float DirectionDot = FVector::DotProduct(AdjustedDir, ShootDir);
		if (DirectionDot < 0.0f)
		{
			// shooting backwards = weapon is penetrating
			bWeaponPenetration = true;
		}
		else if (DirectionDot < 0.5f)
		{
			// check for weapon penetration if angle difference is big enough
			// raycast along weapon mesh to check if there's blocking hit

			FVector MuzzleStartTrace = Origin - GetMesh()->GetSocketRotation(SocketName).Vector() * 150.0f;
			FVector MuzzleEndTrace = Origin;
			FHitResult MuzzleImpact = Linetrace(MuzzleStartTrace, MuzzleEndTrace);

			if (MuzzleImpact.bBlockingHit)
			{
				bWeaponPenetration = true;
			}
		}

		if (bWeaponPenetration)
		{
			// spawn at crosshair position
			Origin = Impact.ImpactPoint - ShootDir * 10.0f;
		}
		else
		{
			// adjust direction to hit
			ShootDir = AdjustedDir;
		}
	}

	if (Debug)
	{
		const FVector StartTrace2 = Origin;
		const FVector EndTrace2 = StartTrace + ShootDir * AdjustRange;
		DrawDebugLine(GetWorld(), StartTrace2, EndTrace2, FColor::Yellow, true);
	}

	OutOrigin = Origin;
	OutShootDir = ShootDir;
	return Linetrace(StartTrace, EndTrace); // this is pointless remove!
}

bool AParagonCharacter::GetIsLocallyControlled()
{
	return IsLocallyControlled();
}

void AParagonCharacter::GiveExperience(float Exp)
{
	float NewExperience = Experience + Exp;
	if (!ExperienceCurveTable.IsNull())
	{
		float NextLevelExperience = 0;
		ExperienceCurveTable.Eval((CharacterLevel + 1), &NextLevelExperience, "");

		if (NewExperience >= NextLevelExperience)
		{
			Experience = NewExperience - NextLevelExperience;

			// Level up!
			LevelUp(1);
		} else {
			Experience = NewExperience;
		}
	}
}

void AParagonCharacter::LevelUp(int Levels)
{
	int NewLevel = CharacterLevel + Levels;
	if (NewLevel > MaxLevel)
		NewLevel = MaxLevel;
	else if(NewLevel < 1)
		NewLevel = 1;

	// Our level changed so we need to refresh abilities
	RemoveStartupGameplayAbilities();
	CharacterLevel = NewLevel;
	InitGameplayAbilities();
}

void AParagonCharacter::FloatingDamageText(float Damage, bool DamageType)
{
	if (FloatingDamageTextWidgetTemplate != nullptr)
	{
		UParagonWidget_FloatingDamageText* FloatingText = CreateWidget<UParagonWidget_FloatingDamageText>(GetWorld(), FloatingDamageTextWidgetTemplate);
		if (FloatingText) {
			FloatingText->SetDamageValue(FMath::RoundToInt(Damage));
			FloatingText->DamageType(DamageType);

			// Get hit position to screen
			APlayerController* PlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
			//const APlayerController* const PlayerController = Cast<const APlayerController>(GetController());

			if (PlayerController)
			{
				FVector2D ScreenLocation; //PlayerController->GetPawn()->GetActorLocation()
				PlayerController->ProjectWorldLocationToScreen(GetActorLocation(), ScreenLocation);

				int32 ScreenWidth = 0;
				int32 ScreenHeight = 0;
				PlayerController->GetViewportSize(ScreenWidth, ScreenHeight);

				FloatingText->SetInitialScreenLocation(ScreenLocation);
			}

			FloatingText->Init();
			FloatingText->AddToViewport();
		}
	}
}

FHitResult AParagonCharacter::Linetrace(FVector StartTrace, FVector EndTrace)
{
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);

	return Hit;
}

void AParagonCharacter::SpawnAbilityIndicator(UMaterialInterface* AbilityIndicator)
{
	// Spawn Decal
	if (CurrentAbilityDecal == nullptr)
	{
		ADecalActor* decal = GetWorld()->SpawnActor<ADecalActor>(); //HitResult.Location, FRotator()

		if (decal && AbilityIndicator != nullptr)
		{
			decal->SetDecalMaterial(AbilityIndicator);
			CurrentAbilityDecal = decal;
			GetWorldTimerManager().SetTimer(IndicatorTimerHandle, this, &AParagonCharacter::MoveAbilityIndicator, 0.05f, true);
		}
	} else {
		// Change decal and move to new location
		if (CurrentAbilityDecal && AbilityIndicator != nullptr)
		{
			MoveAbilityIndicator();
			CurrentAbilityDecal->SetActorHiddenInGame(false);
			CurrentAbilityDecal->SetDecalMaterial(AbilityIndicator);
			GetWorldTimerManager().SetTimer(IndicatorTimerHandle, this, &AParagonCharacter::MoveAbilityIndicator, 0.05f, true);
		}
	}
}

void AParagonCharacter::MoveAbilityIndicator()
{
	if (CurrentAbilityDecal != nullptr)
	{
		FHitResult HitResult = LinetraceFromCamera(1000); //range
		//if(HitResult.bBlockingHit)
		//{
		//	DrawDebugPoint(GetWorld(), HitResult.Location, 20.f, FColor::Red, true);

		//}

		// TODO MOVE TO FUNCTION
		if (!HitResult.bBlockingHit)
		{
			const FVector StartTrace = HitResult.TraceEnd;
			const FVector EndTrace = StartTrace + (-FVector::UpVector) * 10000;

			//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Blue, true);

			// Perform trace to retrieve hit info
			FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, Instigator);
			TraceParams.bTraceAsyncScene = true;
			TraceParams.bReturnPhysicalMaterial = true;

			FHitResult Hit(ForceInit);
			GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);
			//DrawDebugPoint(GetWorld(), Hit.Location, 20.f, FColor::Blue, true);
			CurrentAbilityDecal->SetActorLocation(Hit.Location);
			float PlayerRotYaw = UKismetMathLibrary::FindLookAtRotation(Hit.Location, GetActorLocation()).Yaw;
			FRotator PlayerRot = FRotator(-90, PlayerRotYaw, 0);
			CurrentAbilityDecal->SetActorRotation(PlayerRot);

			AbilityPoint = Hit.Location;

			//UE_LOG(LogTemp, Warning, TEXT("MyCharacter's Location is %s"), *Hit.Location.ToString());
		} 
		else 
		{
			CurrentAbilityDecal->SetActorLocation(HitResult.Location);
			float PlayerRotYaw = UKismetMathLibrary::FindLookAtRotation(HitResult.Location, GetActorLocation()).Yaw;
			FRotator PlayerRot = FRotator(-90, PlayerRotYaw, 0);
			CurrentAbilityDecal->SetActorRotation(PlayerRot);

			AbilityPoint = HitResult.Location;
		}
	}
}

void AParagonCharacter::HideAbilityIndicator()
{
	// Hide
	CurrentAbilityDecal->SetActorHiddenInGame(true);
	GetWorldTimerManager().ClearTimer(IndicatorTimerHandle);
}

void AParagonCharacter::Regen()
{
	// Health Regen
	float NewHealth = GetHealth() + GetHealthRegen();
	NewHealth = (NewHealth >= GetMaxHealth()) ? GetMaxHealth() : NewHealth;
	AttributeSet->SetHealth(NewHealth);

	// Mana Regen
	float NewMana = GetMana() + GetManaRegen();
	NewMana = (NewMana >= GetMaxMana()) ? GetMaxMana() : NewMana;
	AttributeSet->SetMana(NewMana);

	GiveExperience(40);
}

// --- Accessor and Mutator ---

float AParagonCharacter::GetHealth() const
{
	return AttributeSet->GetHealth();
}

void AParagonCharacter::SetHealth(float NewHealth)
{
	AttributeSet->SetHealth(NewHealth);
}

float AParagonCharacter::GetMaxHealth() const
{
	return AttributeSet->GetMaxHealth();
}

float AParagonCharacter::GetHealthRegen() const
{
	return AttributeSet->GetHealthRegen();
}

float AParagonCharacter::GetMana() const
{
	return AttributeSet->GetMana();
}

float AParagonCharacter::GetMaxMana() const
{
	return AttributeSet->GetMaxMana();
}

float AParagonCharacter::GetManaRegen() const
{
	return AttributeSet->GetManaRegen();
}

float AParagonCharacter::GetAttackDamage() const
{
	return AttributeSet->GetAttackDamage();
}

float AParagonCharacter::GetAttackSpeed() const
{
	return AttributeSet->GetAttackSpeed();
}

float AParagonCharacter::GetAbilityDefense() const
{
	return AttributeSet->GetAbilityDefense();
}

float AParagonCharacter::GetBaseDefense() const
{
	return AttributeSet->GetBaseDefense();
}

float AParagonCharacter::GetMoveSpeed() const
{
	return AttributeSet->GetMoveSpeed();
}

int32 AParagonCharacter::GetCharacterLevel() const
{
	return CharacterLevel;
}

float AParagonCharacter::GetCharacterExperience() const
{
	return Experience;
}

float AParagonCharacter::GetNextLevelExperience() const
{
	float NextLevelExperience = 0;
	if (!ExperienceCurveTable.IsNull())
	{
		ExperienceCurveTable.Eval((CharacterLevel+1), &NextLevelExperience, "");
	}
	return NextLevelExperience;
}

void AParagonCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AParagonCharacter, CharacterLevel);
	//DOREPLIFETIME(AParagonCharacter, AbilityPoint);
	//DOREPLIFETIME(AParagonCharacter, AbilitiesHandles);
	DOREPLIFETIME_CONDITION(AParagonCharacter, AbilityPoint, COND_OwnerOnly);
}