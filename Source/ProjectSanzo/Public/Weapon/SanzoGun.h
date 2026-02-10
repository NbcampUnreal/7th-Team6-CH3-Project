// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/SanzoWeaponBase.h"
#include "SanzoGun.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoGun : public ASanzoWeaponBase
{
  GENERATED_BODY()

public:
	ASanzoGun();

protected:

	// 마우스 누름: 연사 타이머 시작
	virtual void StartFire() override;

	// 마우스 뗌: 연사 타이머 중지
	virtual void StopFire() override;

	// 실제 발사 로직: 라인트레이스 및 디버그 라인 그리기
	virtual void Fire() override;

public:

	// 후에 드랍된 총알 먹었을 때 탄약 보충될 함수
	void AddAmmo(int32 Amount);
};
