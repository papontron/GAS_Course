#include "Tags/CC_GameplayTags.h"

namespace CCTags
{
	namespace Ability
	{
		UE_DEFINE_GAMEPLAY_TAG(AutoActivate,TEXT("CCTags.Ability.AutoActivate"))
		UE_DEFINE_GAMEPLAY_TAG(HitReact, "CCTags.Ability.HitReact")
		UE_DEFINE_GAMEPLAY_TAG(Death, "CCTags.Ability.Death")
		namespace Player
		{
			UE_DEFINE_GAMEPLAY_TAG(Primary,TEXT("CCTags.Ability.Player.Primary"))
			UE_DEFINE_GAMEPLAY_TAG(ListenKillScored, TEXT("CCTags.Ability.Player.ListenKillScored"))
		}
		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG(Primary,"CCTags.Ability.Enemy.Primary")
		}
	}
	namespace Event
	{
		UE_DEFINE_GAMEPLAY_TAG(HitReact, "CCTags.Event.HitReact")
		namespace Player
		{
			UE_DEFINE_GAMEPLAY_TAG(KillScored, "CCTags.Event.Player.KillScored")
		}
		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG(PrimaryAttack, "CCTags.Event.Enemy.PrimaryAttack")
		}
	}
	namespace Status
	{
		UE_DEFINE_GAMEPLAY_TAG(Dead, "CCTags.Status.Dead")
		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG(Engaged, "CCTags.Status.Enemy.Engaged")
		}
	}
}