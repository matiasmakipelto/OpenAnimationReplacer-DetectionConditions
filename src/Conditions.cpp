#include "Conditions.h"
#include <numbers>


namespace Conditions
{
	/////////////////////////////////
	// 
	// Base
	// 
	/////////////////////////////////
	bool DetectionCondition::Evaluate(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator, void* a_parentSubMod, bool isDetectedByCondition) const
	{
		resultText = "";
		if (a_refr)
		{
			RE::Actor* a_actor = a_refr->As<RE::Actor>();

			if (a_actor == nullptr)
				return false;
			if (!multiComponent->IsValid())
				return false;
			
			std::vector<RE::Actor*> detectors;

			if (a_actor->GetActorRuntimeData().currentProcess) {
				if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
					bool actorsExist = false;
					for (auto& targetHandle : processLists->highActorHandles) {
						if (const auto target = targetHandle.get(); target && target->GetActorRuntimeData().currentProcess) {
							if (const auto base = target->GetActorBase(); base && !base->AffectsStealthMeter()) {
								continue;
							}

							actorsExist = true;
							if (ValidateTarget(a_actor, target.get())) {
								RE::Actor* detector = isDetectedByCondition ? target.get() : a_actor;
								RE::Actor* detectee = isDetectedByCondition ? a_actor : target.get();
								
								if (detector->RequestDetectionLevel(detectee) > 0)
									detectors.push_back(target.get());
							}
						}
					}

					if (actorsExist) { // This if-condition exists because this function gets called even in main menu.
						// The loop doesn't trigger for player. Thus, check player separately
						const RE::FormID playerRefId = 0x14;
						const auto playerRef = RE::TESForm::LookupByID(playerRefId);
						const auto playerActor = playerRef->As<RE::Actor>();
						if (ValidateTarget(a_actor, playerActor)) {
							RE::Actor* detector = isDetectedByCondition ? playerActor : a_actor;
							RE::Actor* detectee = isDetectedByCondition ? a_actor : playerActor;

							if (detector->RequestDetectionLevel(detectee) > 0) {
								detectors.push_back(playerActor);
							}
						}
					}

					if (detectors.size() > 0)
					{
						// Sorting is actually simply useless. The only difference would be in resultText
						// 
						//RE::NiPoint3 a_actor_pos = a_actor->GetPosition();
						//std::sort(detectors.begin(), detectors.end(), [&a_actor_pos](const auto& a, const auto& b)
						//	{
						//		return a->GetPosition().GetSquaredDistance(a_actor_pos) < b->GetPosition().GetSquaredDistance(a_actor_pos);
						//	});

						for (const auto& target : detectors)
						{
							bool passedEvaluation = CheckMultiCondition(a_actor, target, a_refr, a_clipGenerator, a_parentSubMod);

							if (passedEvaluation)
							{
								if (target->GetName())
								{
									resultText = target->GetName();
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

	bool DetectionCondition::CheckMultiCondition(RE::Actor* actor, RE::Actor* target, RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator, void* a_parentSubMod) const
	{
		// Check child conditions
		bool passedEvaluation = true;
		multiComponent->ForEachCondition([&passedEvaluation, &actor, &target, &a_refr, &a_clipGenerator, &a_parentSubMod](auto& childCondition) {

			// DetectionDistance condition
			if (childCondition->GetName() == "DetectionDistance")
			{
				DetectionDistanceCondition* condition = static_cast<DetectionDistanceCondition*>(childCondition.get());
				if (condition)
				{
					if (condition->CustomEvaluate(a_refr, target) == false)
					{
						passedEvaluation = false;
						return RE::BSVisit::BSVisitControl::kStop;
					}
				}
			}
			// DetectionRelationship condition
			else if (childCondition->GetName() == "DetectionRelationship")
			{
				DetectionRelationshipCondition* condition = static_cast<DetectionRelationshipCondition*>(childCondition.get());
				if (condition)
				{
					if (condition->CustomEvaluate(actor, target, a_refr) == false)
					{
						passedEvaluation = false;
						return RE::BSVisit::BSVisitControl::kStop;
					}
				}
			}
			// DetectionAngle condition
			else if (childCondition->GetName() == "DetectionAngle")
			{
				DetectionAngleCondition* condition = static_cast<DetectionAngleCondition*>(childCondition.get());
				if (condition)
				{
					if (condition->CustomEvaluate(actor, target, a_refr) == false)
					{
						passedEvaluation = false;
						return RE::BSVisit::BSVisitControl::kStop;
					}
				}
			}
			// Normal conditions
			else if (childCondition->Evaluate(target->As<RE::TESObjectREFR>(), a_clipGenerator, a_parentSubMod) == false)
			{
				passedEvaluation = false;
				return RE::BSVisit::BSVisitControl::kStop;
			}

			return RE::BSVisit::BSVisitControl::kContinue;
			});

		return passedEvaluation;
	}

	bool DetectionCondition::ValidateTarget(RE::Actor* actor, RE::Actor* target) const
	{
		if (!target || !actor)
		{
			return false;
		}

		if (target == actor)
		{
			return false;
		}

		if (target->IsDeleted() || target->IsDisabled())
		{
			return false;
		}

		if (target->GetRace())
		{
			std::string raceEdid = target->GetRace()->GetFormEditorID();
			if (raceEdid == "ManakinRace")
			{
				return false;
			}
		}

		return true;
	}





	/////////////////////////////////
	// 
	// Detection distance
	// 
	/////////////////////////////////
	bool DetectionDistanceCondition::EvaluateImpl([[maybe_unused]] RE::TESObjectREFR* a_refr, [[maybe_unused]] RE::hkbClipGenerator* a_clipGenerator, [[maybe_unused]] void* a_parentSubMod) const
	{
		// This should only be used inside a detection condition
		return false;
	}

	bool DetectionDistanceCondition::CustomEvaluate(RE::TESObjectREFR* a_refr, const RE::Actor* target)
	{
		const auto refrPos = a_refr->GetPosition();
		const auto targetPos = target->GetPosition();
		const auto numericVal = numericComponent->GetNumericValue(a_refr);
		return comparisonComponent->GetComparisonResult(refrPos.GetSquaredDistance(targetPos), numericVal * numericVal);  // compare squared to avoid sqrt
	}


	
	

	/////////////////////////////////
	// 
	// Detection relationship
	// 
	/////////////////////////////////

	// Copied from OAR source
	static int32_t* g_RelationshipRankTypeIdsByIndex = (int32_t*)REL::VariantID(502260, 369311, 0x1E911A0).address();
	using tTESNPC_GetRelationshipRankIndex = int32_t(*)(RE::TESNPC* a_npc1, RE::TESNPC* a_npc2);
	static REL::Relocation<tTESNPC_GetRelationshipRankIndex> TESNPC_GetRelationshipRankIndex{ REL::VariantID(23624, 24076, 0x355790) };  // 345ED0, 35C270, 355790
	bool GetRelationshipRank(RE::TESNPC* a_npc1, RE::TESNPC* a_npc2, int32_t& a_outRank)
	{
		if (a_npc1 && a_npc2)
		{
			a_outRank = g_RelationshipRankTypeIdsByIndex[TESNPC_GetRelationshipRankIndex(a_npc1, a_npc2)];
			return true;
		}
	
		return false;
	}
	

	bool DetectionRelationshipCondition::EvaluateImpl([[maybe_unused]] RE::TESObjectREFR* a_refr, [[maybe_unused]] RE::hkbClipGenerator* a_clipGenerator, [[maybe_unused]] void* a_parentSubMod) const
	{
		// This should only be used inside a detection condition
		return false;
	}

	bool DetectionRelationshipCondition::CustomEvaluate(RE::Actor* actor, RE::Actor* target, RE::TESObjectREFR* a_refr)
	{
		int32_t relationshipRank = 0;
		GetRelationshipRank(actor->GetActorBase(), target->GetActorBase(), relationshipRank);
		return comparisonComponent->GetComparisonResult(static_cast<float>(relationshipRank), numericComponent->GetNumericValue(a_refr));
	}





	/////////////////////////////////
	// 
	// Detection angle
	// 
	/////////////////////////////////
	bool DetectionAngleCondition::EvaluateImpl([[maybe_unused]] RE::TESObjectREFR* a_refr, [[maybe_unused]] RE::hkbClipGenerator* a_clipGenerator, [[maybe_unused]] void* a_parentSubMod) const
	{
		// This should only be used inside a detection condition
		return false;
	}

	bool DetectionAngleCondition::CustomEvaluate(RE::Actor* actor, RE::Actor* target, RE::TESObjectREFR* a_refr)
	{
		bool swapActors = booleanComponent->GetBoolValue();
		RE::Actor* angleActor = swapActors ? target : actor;
		RE::Actor* targetActor = swapActors ? actor : target;

		float actorAngle = angleActor->GetAngleZ() / ((float)std::numbers::pi) * 180;

		RE::NiPoint3 targetPos = targetActor->GetPosition();
		RE::NiPoint3 actorPos = angleActor->GetPosition();
		RE::NiPoint2 unitVectorToTarget = RE::NiPoint2((targetPos - actorPos).x, (targetPos - actorPos).y);
		unitVectorToTarget = unitVectorToTarget / unitVectorToTarget.Length();
		float angleToTarget = std::atan2(unitVectorToTarget.x, unitVectorToTarget.y) / ((float)std::numbers::pi) * 180;
		angleToTarget = angleToTarget < 0 ? angleToTarget + 360 : angleToTarget;

		if (std::fabs(actorAngle - angleToTarget) > 180) // different phases
		{
			if (actorAngle < angleToTarget)
				actorAngle += 360;
			else
				angleToTarget += 360;
		}

		bool targetOnRight = actorAngle - angleToTarget < 0;

		float relativeAngleToTarget = std::fabs(actorAngle - angleToTarget);
		relativeAngleToTarget = relativeAngleToTarget > 180 ? 360 - relativeAngleToTarget : relativeAngleToTarget;
		if (rightBooleanComponent->GetBoolValue())
			if (!targetOnRight)
				return false;
		if (leftBooleanComponent->GetBoolValue())
			if (targetOnRight)
				return false;

		return comparisonComponent->GetComparisonResult(relativeAngleToTarget, numericComponent->GetNumericValue(a_refr));
	}
}
