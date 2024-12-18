// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Actor/MagicCircle.h"
#include "Components/DecalComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "UI/Widget/DamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	UpdateMagicCircleLocation();
	AutoRun();
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if (!IsValid(MagicCircle))
	{
		MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);
		if (DecalMaterial)
		{
			MagicCircle->MagicCircleDecalComponent->SetMaterial(0, DecalMaterial);
		}
	}
}

void AAuraPlayerController::HideMagicCircle()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->Destroy();
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) { return; }

	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->
			FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoAcceptanceRadius) { bAutoRunning = false; }
	}
}

void AAuraPlayerController::CursorTrace()
{
	if (GetAuraASC() && GetAuraASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_CursorTrace))
	{
		if (LastActor) { LastActor->UnHighLight(); }
		if (CurrentActor) { CurrentActor->UnHighLight(); }
		LastActor = nullptr;
		CurrentActor = nullptr;
		return;
	}
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) { return; }

	LastActor = CurrentActor;
	CurrentActor = Cast<IEnemyInterface>(CursorHit.GetActor());
	if (CurrentActor) { bTargeting = true; }
	else { bTargeting = false; }
	if (LastActor != CurrentActor)
	{
		if (LastActor) { LastActor->UnHighLight(); }
		if (CurrentActor) { CurrentActor->HighLight(); }
	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer());
	if (Subsystem) { Subsystem->AddMappingContext(AuraContext, 0); }

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputData;
	InputData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputData.SetHideCursorDuringCapture(false);
	SetInputMode(InputData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);

	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased,
	                                       &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::UpdateMagicCircleLocation()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
	}
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	if (GetAuraASC() && GetAuraASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed)) { return; }
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator ControllerRotation = GetControlRotation();
	const FRotator YawRotation(0.f, ControllerRotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		bAutoRunning = false;
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetAuraASC() && GetAuraASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed)) { return; }
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = CurrentActor ? true : false;
		bAutoRunning = false;
	}
	if (GetAuraASC()) { GetAuraASC()->AbilityInputTagPressed(InputTag);}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetAuraASC() && GetAuraASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputReleased)) { return; }
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetAuraASC()) GetAuraASC()->AbilityInputTagReleased(InputTag);
		return;
	}
	if (GetAuraASC()) { GetAuraASC()->AbilityInputTagReleased(InputTag); }

	if (!bTargeting || !bShiftKeyDown)
	{
		APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
				this, ControlledPawn->GetActorLocation(), CachedDestination);
			if (NavPath)
			{
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
				}
				if (NavPath->PathPoints.Num() > 0)
				{
					CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
					bAutoRunning = true;
				}
			}
			if (GetAuraASC() && !GetAuraASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
			}
		}
		bTargeting = false;
		FollowTime = 0.f;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetAuraASC() && GetAuraASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld)) { return; }
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetAuraASC()) { GetAuraASC()->AbilityInputTagHeld(InputTag); }
		return;
	}

	if (bTargeting || bShiftKeyDown)
	{
		if (GetAuraASC()) { GetAuraASC()->AbilityInputTagHeld(InputTag); }
	}
	else
	{
		if (CursorHit.bBlockingHit)
		{
			FollowTime += GetWorld()->GetDeltaSeconds();
			if (CursorHit.bBlockingHit) { CachedDestination = CursorHit.ImpactPoint; }

			if (APawn* ControlledPawn = GetPawn())
			{
				const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
				ControlledPawn->AddMovementInput(WorldDirection);
			}
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraASC()
{
	if (AuraASC == nullptr)
	{
		AuraASC = Cast<UAuraAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return AuraASC;
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextCompClass)
	{
		UDamageTextComponent* DamageTextComp = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextCompClass);
		DamageTextComp->RegisterComponent();
		DamageTextComp->AttachToComponent(TargetCharacter->GetRootComponent(),
		                                  FAttachmentTransformRules::KeepRelativeTransform);
		DamageTextComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageTextComp->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}
}
