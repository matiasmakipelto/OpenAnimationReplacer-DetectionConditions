#include "Conditions.h"


namespace Conditions
{
	// Copied from OAR source
	static int32_t* g_RelationshipRankTypeIdsByIndex = (int32_t*)REL::VariantID(502260, 369311, 0x1E911A0).address();
	using tTESNPC_GetRelationshipRankIndex = int32_t(*)(RE::TESNPC* a_npc1, RE::TESNPC* a_npc2);
	static REL::Relocation<tTESNPC_GetRelationshipRankIndex> TESNPC_GetRelationshipRankIndex{ REL::VariantID(23624, 24076, 0x355790) };  // 345ED0, 35C270, 355790
	
	bool GetRelationshipRank(RE::TESNPC* a_npc1, RE::TESNPC* a_npc2, int32_t& a_outRank)
	{
		if (a_npc1 && a_npc2) {
			a_outRank = g_RelationshipRankTypeIdsByIndex[TESNPC_GetRelationshipRankIndex(a_npc1, a_npc2)];
			return true;
		}
	
		return false;
	}
	




	IsDetectedByCondition::IsDetectedByCondition()
	{
		// Human
		useBoolComponent = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Use the humanoid condition"));
		isHuman = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Is humanoid"));

		// Gender
		useBoolComponent1 = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Use the gender condition"));
		isFemale = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Is Female"));

		// Relationship
		useBoolComponent2 = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Use the relationship condition"));
		comparisonComponent = static_cast<IComparisonConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kComparison,
			"Comparison"));
		numericComponent = static_cast<INumericConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kNumeric,
			"Relationship [-4 = Archnemesis, .., 4 = Lover]"));

		// Faction
		useBoolComponent3 = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Use the faction condition"));
		notComponent = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Is NOT"));
		factionComponent = static_cast<IFormConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kForm,
			"Faction"));

		// ActorBase
		useBoolComponent4 = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Use the actorbase condition"));
		notComponent2 = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Is NOT"));
		actorBaseComponent = static_cast<IFormConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kForm,
			"Actor base"));
	}

	bool IsDetectedByCondition::IsRelationship(RE::TESNPC* base, RE::TESNPC* target, RE::TESObjectREFR* a_refr) const
	{
		int32_t relationshipRank = 0;
		GetRelationshipRank(base, target, relationshipRank);
		return comparisonComponent->GetComparisonResult(static_cast<float>(relationshipRank), numericComponent->GetNumericValue(a_refr));
	}

	bool IsDetectedByCondition::EvaluateImpl([[maybe_unused]] RE::TESObjectREFR* a_refr,
		[[maybe_unused]] RE::hkbClipGenerator* a_clipGenerator) const
	{
		if (a_refr)
		{
			RE::Actor* a_actor = a_refr->As<RE::Actor>();

			if (a_actor == nullptr)
				return false;

			if (a_actor->GetActorRuntimeData().currentProcess) {
				if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
					for (auto& targetHandle : processLists->highActorHandles) {
						if (const auto target = targetHandle.get(); target && target->GetActorRuntimeData().currentProcess) {
							if (const auto base = target->GetActorBase(); base && !base->AffectsStealthMeter()) {
								continue;
							}

							if (ValidateTarget(a_actor, target.get())) {
								if (target->RequestDetectionLevel(a_actor) > 0) {
									if (FitsConditions(a_actor, target.get(), a_refr)) {
										return true;
									}
								}
							}
						}
					}
				}
			}

			// The loop doesn't trigger for player. Thus, check player separately
			const RE::FormID playerRefId = 0x00000014;
			const auto playerRef = RE::TESForm::LookupByID(playerRefId);
			const auto playerActor = playerRef->As<RE::Actor>();
			if (ValidateTarget(a_actor, playerActor)) {
				if (playerActor->RequestDetectionLevel(a_actor) > 0) {
					if (FitsConditions(a_actor, playerActor, a_refr)) {
						return true;
					}
				}
			}
		}

		return false;
	}

	bool IsDetectedByCondition::ValidateTarget(RE::Actor* actor, RE::Actor* target) const
	{
		if (target == actor) {
			return false;
		}

		if (target->IsDead() || target->IsDeleted() || target->IsDisabled()) {
			return false;
		}

		return true;
	}

	bool IsDetectedByCondition::FitsConditions(RE::Actor* actor, RE::Actor* target, RE::TESObjectREFR* a_refr) const
	{
		// Humanoid
		if (useBoolComponent->GetBoolValue()) {
			if (isHuman->GetBoolValue() != target->HasKeywordString("ActorTypeNPC")) {
				return false;
			}
		}

		// Female
		if (useBoolComponent1->GetBoolValue()) {
			if (isFemale->GetBoolValue() != target->GetActorBase()->IsFemale()) {
				return false;
			}
		}

		// Relationship
		if (useBoolComponent2->GetBoolValue()) {
			if (!IsRelationship(actor->GetActorBase(), target->GetActorBase(), a_refr)) {
				return false;
			}
		}

		// Faction
		if (useBoolComponent3->GetBoolValue() && factionComponent->IsValid()) {
			if (const auto faction = factionComponent->GetTESFormValue()->As<RE::TESFaction>()) { // Faction was provided
				if (notComponent->GetBoolValue() /* Read as "Must not belong to a faction"*/ == target->IsInFaction(faction))
					return false;
			}
			else if (!notComponent->GetBoolValue() /* Read as "Must belong to a faction"*/) { // Faction wasn't provided
				return false;
			}
		}

		// ActorBase
		if (useBoolComponent4->GetBoolValue() && actorBaseComponent->IsValid()) {
			if (notComponent2->GetBoolValue()) { /* Shouldn't be the actor base */
				if (target->GetActorBase() == actorBaseComponent->GetTESFormValue()) {
					return false;
				}
			}
			else { /* Should be the actor base */
				if (target->GetActorBase() != actorBaseComponent->GetTESFormValue()) {
					return false;
				}
			}
		}

		return true;
	};









	DetectsCondition::DetectsCondition()
	{
		// Human
		useBoolComponent = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Use the humanoid condition"));
		isHuman = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Is humanoid"));

		// Gender
		useBoolComponent1 = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Use the gender condition"));
		isFemale = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Is Female"));

		// Relationship
		useBoolComponent2 = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Use the relationship condition"));
		comparisonComponent = static_cast<IComparisonConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kComparison,
			"Comparison"));
		numericComponent = static_cast<INumericConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kNumeric,
			"Relationship [-4 = Archnemesis, .., 4 = Lover]"));

		// Faction
		useBoolComponent3 = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Use the faction condition"));
		notComponent = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Is NOT"));
		factionComponent = static_cast<IFormConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kForm,
			"Faction"));

		// ActorBase
		useBoolComponent4 = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Use the actorbase condition"));
		notComponent2 = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Is NOT"));
		actorBaseComponent = static_cast<IFormConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kForm,
			"Actor base"));
	}

	bool DetectsCondition::IsRelationship(RE::TESNPC* base, RE::TESNPC* target, RE::TESObjectREFR* a_refr) const
	{
		int32_t relationshipRank = 0;
		GetRelationshipRank(base, target, relationshipRank);
		return comparisonComponent->GetComparisonResult(static_cast<float>(relationshipRank), numericComponent->GetNumericValue(a_refr));
	}

	bool DetectsCondition::EvaluateImpl([[maybe_unused]] RE::TESObjectREFR* a_refr,
		[[maybe_unused]] RE::hkbClipGenerator* a_clipGenerator) const
	{
		if (a_refr)
		{
			RE::Actor* a_actor = a_refr->As<RE::Actor>();

			if (a_actor == nullptr)
				return false;

			if (a_actor->GetActorRuntimeData().currentProcess) {
				if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
					for (auto& targetHandle : processLists->highActorHandles) {
						if (const auto target = targetHandle.get(); target && target->GetActorRuntimeData().currentProcess) {
							if (const auto base = target->GetActorBase(); base && !base->AffectsStealthMeter()) {
								continue;
							}

							if (ValidateTarget(a_actor, target.get())) {
								if (a_actor->RequestDetectionLevel(target.get()) > 0) {
									if (FitsConditions(a_actor, target.get(), a_refr)) {
										return true;
									}
								}
							}
						}
					}
				}
			}

			// The loop doesn't trigger for player. Thus, check player separately
			const RE::FormID playerRefId = 0x00000014;
			const auto playerRef = RE::TESForm::LookupByID(playerRefId);
			const auto playerActor = playerRef->As<RE::Actor>();
			if (ValidateTarget(a_actor, playerActor)) {
				if (a_actor->RequestDetectionLevel(playerActor) > 0) {
					if (FitsConditions(a_actor, playerActor, a_refr)) {
						return true;
					}
				}
			}
		}

		return false;
	}

	bool DetectsCondition::ValidateTarget(RE::Actor* actor, RE::Actor* target) const
	{
		if (target == actor) {
			return false;
		}

		if (target->IsDead() || target->IsDeleted() || target->IsDisabled()) {
			return false;
		}

		return true;
	}

	bool DetectsCondition::FitsConditions(RE::Actor* actor, RE::Actor* target, RE::TESObjectREFR* a_refr) const
	{
		// Humanoid
		if (useBoolComponent->GetBoolValue()) {
			if (isHuman->GetBoolValue() != target->HasKeywordString("ActorTypeNPC")) {
				return false;
			}
		}

		// Female
		if (useBoolComponent1->GetBoolValue()) {
			if (isFemale->GetBoolValue() != target->GetActorBase()->IsFemale()) {
				return false;
			}
		}

		// Relationship
		if (useBoolComponent2->GetBoolValue()) {
			if (!IsRelationship(actor->GetActorBase(), target->GetActorBase(), a_refr)) {
				return false;
			}
		}

		// Faction
		if (useBoolComponent3->GetBoolValue() && factionComponent->IsValid()) {
			if (const auto faction = factionComponent->GetTESFormValue()->As<RE::TESFaction>()) { // Faction was provided
				if (notComponent->GetBoolValue() /* Read as "Must not belong to a faction"*/ == target->IsInFaction(faction))
					return false;
			}
			else if (!notComponent->GetBoolValue() /* Read as "Must belong to a faction"*/) { // Faction wasn't provided
				return false;
			}
		}

		// ActorBase
		if (useBoolComponent4->GetBoolValue() && actorBaseComponent->IsValid()) {
			if (notComponent2->GetBoolValue()) { /* Shouldn't be the actor base */
				if (target->GetActorBase() == actorBaseComponent->GetTESFormValue()) {
					return false;
				}
			}
			else { /* Should be the actor base */
				if (target->GetActorBase() != actorBaseComponent->GetTESFormValue()) {
					return false;
				}
			}
		}

		return true;
	};
}
