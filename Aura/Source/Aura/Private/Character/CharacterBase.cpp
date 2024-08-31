// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/CharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	DebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>(TEXT("DebuffComponent"));
	DebuffComponent->SetupAttachment(GetRootComponent());
	DebuffComponent->SetDebuffTag(FAuraGameplayTags::Get().Debuff_Burn);
	StunDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>(TEXT("StunDebuffComponent"));
	StunDebuffComponent->SetupAttachment(GetRootComponent());
	StunDebuffComponent->SetDebuffTag(FAuraGameplayTags::Get().Debuff_Stun);

	EffectAttachComp = CreateDefaultSubobject<USceneComponent>(TEXT("EffectAttachComp"));
	EffectAttachComp->SetupAttachment(GetRootComponent());
	HaloOfProtectionNiagaraComp = CreateDefaultSubobject<UPassiveNiagaraComponent>(TEXT("HaloOfProtectionNiagaraComp"));
	HaloOfProtectionNiagaraComp->SetupAttachment(EffectAttachComp);
	LifeSiphonNiagaraComp = CreateDefaultSubobject<UPassiveNiagaraComponent>(TEXT("LifeSiphonNiagaraComp"));
	LifeSiphonNiagaraComp->SetupAttachment(EffectAttachComp);
	ManaSiphonNiagaraComp = CreateDefaultSubobject<UPassiveNiagaraComponent>(TEXT("ManaSiphonNiagaraComp"));
	ManaSiphonNiagaraComp->SetupAttachment(EffectAttachComp);
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

FVector ACharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	if (MontageTag.MatchesTagExact(FAuraGameplayTags::Get().CombatSocket_Weapon) && IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(WeaponTipSockName);
	}
	if (MontageTag.MatchesTagExact(FAuraGameplayTags::Get().CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSockName);
	}
	if (MontageTag.MatchesTagExact(FAuraGameplayTags::Get().CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSockName);
	}
	if (MontageTag.MatchesTagExact(FAuraGameplayTags::Get().CombatSocket_Tail))
	{
		return GetMesh()->GetSocketLocation(TailSockName);
	}
	return FVector();
}

TArray<FTaggedMontage> ACharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* ACharacterBase::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

FTaggedMontage ACharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& Tag)
{
	for (FTaggedMontage TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag.MatchesTagExact(Tag))
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}

int32 ACharacterBase::GetNumMinionsCount_Implementation()
{
	return NumMinions;
}

void ACharacterBase::IncrementMinionCount_Implementation(int32 Amount)
{
	NumMinions += Amount;
}

ECharacterCatrgory ACharacterBase::GetCharacterCategory_Implementation()
{
	return CharacterCategory;
}

USkeletalMeshComponent* ACharacterBase::GetWeapon_Implementation()
{
	return Weapon;
}

void ACharacterBase::SetIsBeingShocked_Implementation(bool InState)
{
	bIsBeingShocked = InState;
}

bool ACharacterBase::IsBeingShocked_Implementation()
{
	return bIsBeingShocked;
}

FOnASCRegisteredSignature ACharacterBase::GetOnAscRegisteredDelegate()
{
	return OnAscRegisteredDelegate;
}

FOnDeathSignuture& ACharacterBase::GetOnDeathDelegate()
{
	return OnDeathDelegate;
}

UAnimMontage* ACharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void ACharacterBase::Die(const FVector& DeathImpulse)
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath(DeathImpulse);
}

void ACharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());

	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(DeathImpulse * .1f, NAME_None, true);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();

	bDead = true;
	if (DebuffComponent) { DebuffComponent->Deactivate(); }
	if (StunDebuffComponent) { StunDebuffComponent->Deactivate(); }
	OnDeathDelegate.Broadcast(this);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterBase, bInStun);
	DOREPLIFETIME(ACharacterBase, bInBurn);
	DOREPLIFETIME(ACharacterBase, bIsBeingShocked);
}

bool ACharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* ACharacterBase::GetAvatar_Implementation()
{
	return this;
}

void ACharacterBase::InitAbilityActorinfo()
{
}

void ACharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GE, float Level) const
{
	check(GE);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	check(ASC);
	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle GameplayEffectSpecHandle = ASC->MakeOutgoingSpec(GE, Level, EffectContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpecHandle.Data.Get());
}

void ACharacterBase::InitDefaultAttributeByGE() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void ACharacterBase::AddCharacterAbilities()
{
	if (!HasAuthority()) { return; }

	UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	ASC->AddCharacterAbilities(StartupAbilities);
	ASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

void ACharacterBase::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMI = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMI);
		StartDissolveTimeLine(DynamicMI);
	}
	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMI = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMI);
		StartWeaponDissolveTimeLine(DynamicMI);
	}
}

void ACharacterBase::OnRep_Stunned()
{
}

void ACharacterBase::OnRep_Burned()
{
}

void ACharacterBase::OnStuntagChanged(const FGameplayTag StunTag, int32 NewCount)
{
	bInStun = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bInStun ? 0.f : BaseWalkSpeed;
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
