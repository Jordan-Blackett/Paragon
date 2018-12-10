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

	CharacterLevel = 15;
	bAbilitiesInitialized = false;
}

void AParagonCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	InitGameplayAbilities();

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

	// Initialize our abilities
	if (AbilitySystem)
	{
		AbilitySystem->InitAbilityActorInfo(this, this);
		//AddStartupGameplayAbilities();
	}

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
		AbilitySystem->TryActivateAbilityByClass(AbilitiesSlots[Slot]);
	}
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
	if (bAbilityPressed)
	{
		HideAbilityIndicator();

		//if (Ability2Animation)
		//{
		//	PlayAnimMontage(Ability2Animation);
		//}

		// Cast ability
		if (AbilitiesSlots.Num() > CurrentActiveAbility)
		{
			if (AbilitySystem->TryActivateAbility(AbilitiesHandles[CurrentActiveAbility])) {
			} else {
				UE_LOG(LogTemp, Warning, TEXT("Ability Failed"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Ability In Array"));
		}

		bAbilityPressed = false;
	}
	else
	{
		CurrentActiveAbility = 0;
		StartAbility();
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

bool AParagonCharacter::GetIsLocallyControlled()
{
	return IsLocallyControlled();
}

void AParagonCharacter::FloatingDamageText(float Damage)
{
	if (FloatingDamageTextWidgetTemplate != nullptr)
	{
		UParagonWidget_FloatingDamageText* FloatingText = CreateWidget<UParagonWidget_FloatingDamageText>(GetWorld(), FloatingDamageTextWidgetTemplate);
		if (FloatingText) {
			FloatingText->SetDamageValue(Damage);

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

FHitResult AParagonCharacter::Linetrace() // Linetracefromsocket(string name)
{
	FVector CamLoc;
	FRotator CamRot;
	GetViewPoint(CamLoc, CamRot);

	FVector ShootDir = CamRot.Vector();
	FVector Origin = GetMesh()->GetSocketLocation("Gun_LOS");

	// Show ability indicator
	const float AdjustRange = 1000.0f; // range
	const FVector StartTrace = Origin;
	const FVector EndTrace = StartTrace + ShootDir * AdjustRange;

	//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, true);

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
		FHitResult HitResult = Linetrace(); //range
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
}

// --- Accessor and Mutator ---

float AParagonCharacter::GetHealth() const
{
	return AttributeSet->GetHealth();;
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

void AParagonCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AParagonCharacter, CharacterLevel);
	DOREPLIFETIME(AParagonCharacter, AbilityPoint);
}