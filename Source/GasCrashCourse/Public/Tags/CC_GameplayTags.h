#pragma once
#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace CCTags
{
	namespace Ability
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(AutoActivate)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Death)
		namespace Player
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary)
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ListenKillScored)
		}
		namespace Enemy
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary)
		}
	}
	namespace Event
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact)
		namespace Player
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(KillScored)
		}
		namespace Enemy
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(PrimaryAttack)
		}
	}
	namespace Status
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dead)
		namespace Enemy
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Engaged)
		}
	}
}
