// Fill out your copyright notice in the Description page of Project Settings.


#include "Common/SanzoGameplayTag.h"

UE_DEFINE_GAMEPLAY_TAG(SanzoTags::Aiming, "Character.Action.Movable.Aiming");
UE_DEFINE_GAMEPLAY_TAG(SanzoTags::Sprint, "Character.Action.Movable.Sprint");
UE_DEFINE_GAMEPLAY_TAG(SanzoTags::Attack, "Character.Action.Movable.Attack");
UE_DEFINE_GAMEPLAY_TAG(SanzoTags::Parry, "Character.Action.Fixed.Parry");
UE_DEFINE_GAMEPLAY_TAG(SanzoTags::Dodge, "Character.Action.Fixed.Dodge");
UE_DEFINE_GAMEPLAY_TAG(SanzoTags::Exhausted, "Character.Status.Exhausted");


UE_DEFINE_GAMEPLAY_TAG(SanzoTags::Action_Fixed, "Character.Action.Fixed");
UE_DEFINE_GAMEPLAY_TAG(SanzoTags::ParryWindow, "Character.State.ParryWindow");
UE_DEFINE_GAMEPLAY_TAG(SanzoTags::IFrame, "Character.State.IFrame");


SanzoGameplayTag::SanzoGameplayTag()
{
}

SanzoGameplayTag::~SanzoGameplayTag()
{
}
