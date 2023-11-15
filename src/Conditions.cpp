#include "Conditions.h"


namespace Conditions
{
	// Helper functions. Actual condition is at bottom.

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
	
	bool IsDetectedByCondition::IsRelationship(RE::TESNPC* base, RE::TESNPC* target, RE::TESObjectREFR* a_refr) const
	{
		int32_t relationshipRank = 0;
		GetRelationshipRank(base, target, relationshipRank);
		return comparisonComponent->GetComparisonResult(static_cast<float>(relationshipRank), numericComponent->GetNumericValue(a_refr));
	}
	
	














	IsDetectedByCondition::IsDetectedByCondition()
	{
		// Relationship
		useBoolComponent1 = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Use the relationship condition"));
		comparisonComponent = static_cast<IComparisonConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kComparison,
			"Comparison"));
		numericComponent = static_cast<INumericConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kNumeric,
			"Relationship [-4 = Archnemesis, .., 4 = Lover]"));

		// Gender
		useBoolComponent2 = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Use the gender condition"));
		isFemale = static_cast<IBoolConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kBool,
			"Is Female"));

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
	}

	bool IsDetectedByCondition::EvaluateImpl([[maybe_unused]] RE::TESObjectREFR* a_refr,
		[[maybe_unused]] RE::hkbClipGenerator* a_clipGenerator) const
	{
		if (a_refr)
		{
			RE::Actor* a_actor = a_refr->As<RE::Actor>();

			if (a_actor->GetActorRuntimeData().currentProcess) {
				if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
					for (auto& targetHandle : processLists->highActorHandles) {
						if (const auto target = targetHandle.get(); target && target->GetActorRuntimeData().currentProcess) {
							if (const auto base = target->GetActorBase(); base && !base->AffectsStealthMeter()) {
								continue;
							}

							if (target->RequestDetectionLevel(a_actor) > 0) {

								if (useBoolComponent1->GetBoolValue()) {
									if (!IsRelationship(a_actor->GetActorBase(), target->GetActorBase(), a_refr)) {
										continue;
									}
								}

								if (useBoolComponent2->GetBoolValue()) {
									if (isFemale->GetBoolValue() != target->GetActorBase()->IsFemale()) {
										continue;
									}
								}

								if (useBoolComponent3->GetBoolValue() && factionComponent->IsValid()) {
									if (const auto faction = factionComponent->GetTESFormValue()->As<RE::TESFaction>()) {
										if (notComponent->GetBoolValue() /* Read as "Must not belong to a faction"*/ == target->IsInFaction(faction))
											continue;
									}
									else if (!notComponent->GetBoolValue() /* Read as "Must belong to a faction"*/) {
										continue;
									}
								}

								return true;
							}
						}
					}
				}
			}
		}

		return false;
	}
}
