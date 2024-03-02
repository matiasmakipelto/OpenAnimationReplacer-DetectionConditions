#include "Conditions.h"
#include <numbers>


namespace Conditions
{
	/////////////////////////////////
	// 
	// Base
	// 
	/////////////////////////////////
	bool DetectionCondition::Evaluate(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator, bool isDetectedByCondition) const
	{
		resultText = "";
		if (a_refr)
		{
			RE::Actor* a_actor = a_refr->As<RE::Actor>();

			if (a_actor == nullptr)
				return false;
			
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
								
								if (detector->RequestDetectionLevel(detectee) > 0) {
									if (multiComponent->IsValid())
									{

										// Check child conditions
										bool passedEvaluation = true;
										multiComponent->ForEachCondition([&passedEvaluation, &a_actor, &target, &a_clipGenerator, &a_refr](auto& childCondition) {
											
											// DetectionDistance condition
											if (childCondition->GetName() == "DetectionDistance")
											{
												DetectionDistanceCondition* condition = static_cast<DetectionDistanceCondition*>(childCondition.get());
												if (condition)
												{
													if (condition->CustomEvaluate(a_refr, target.get()) == false)
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
													if (condition->CustomEvaluate(a_actor, target.get(), a_refr) == false)
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
													if (condition->CustomEvaluate(a_actor, target.get(), a_refr) == false)
													{
														passedEvaluation = false;
														return RE::BSVisit::BSVisitControl::kStop;
													}
												}
											}
											// Normal conditions
											else if (childCondition->Evaluate(target->As<RE::TESObjectREFR>(), a_clipGenerator) == false)
											{
												passedEvaluation = false;
												return RE::BSVisit::BSVisitControl::kStop;
											}

											return RE::BSVisit::BSVisitControl::kContinue;
										});


										if (passedEvaluation)
										{
											if (target.get()->GetName())
											{
												resultText = target.get()->GetName();
											}

											return true;
										}
									}
								}
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
								if (multiComponent->IsValid())
								{






									bool passedEvaluation = true;

									// Check child conditions
									multiComponent->ForEachCondition([&passedEvaluation, &a_actor, &playerActor, &a_clipGenerator, &a_refr](auto& childCondition) {

										// DetectionDistance condition
										if (childCondition->GetName() == "DetectionDistance")
										{
											DetectionDistanceCondition* condition = static_cast<DetectionDistanceCondition*>(childCondition.get());
											if (condition)
											{
												if (condition->CustomEvaluate(a_refr, playerActor) == false)
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
												if (condition->CustomEvaluate(a_actor, playerActor, a_refr) == false)
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
												if (condition->CustomEvaluate(a_actor, playerActor, a_refr) == false)
												{
													passedEvaluation = false;
													return RE::BSVisit::BSVisitControl::kStop;
												}
											}
										}
										// Normal conditions
										else if (childCondition->Evaluate(playerActor->As<RE::TESObjectREFR>(), a_clipGenerator) == false)
										{
											passedEvaluation = false;
											return RE::BSVisit::BSVisitControl::kStop;
										}

										return RE::BSVisit::BSVisitControl::kContinue;
										});







									if (passedEvaluation)
									{
										if (playerActor->GetName())
										{
											resultText = playerActor->GetName();
										}

										return true;
									}
								}
							}
						}
					}
				}
			}
		}

		return false;
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
	bool DetectionDistanceCondition::EvaluateImpl([[maybe_unused]] RE::TESObjectREFR* a_refr, [[maybe_unused]] RE::hkbClipGenerator* a_clipGenerator) const
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
	

	bool DetectionRelationshipCondition::EvaluateImpl([[maybe_unused]] RE::TESObjectREFR* a_refr, [[maybe_unused]] RE::hkbClipGenerator* a_clipGenerator) const
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
	bool DetectionAngleCondition::EvaluateImpl([[maybe_unused]] RE::TESObjectREFR* a_refr, [[maybe_unused]] RE::hkbClipGenerator* a_clipGenerator) const
	{
		// This should only be used inside a detection condition
		return false;
	}

	bool DetectionAngleCondition::CustomEvaluate(RE::Actor* actor, RE::Actor* target, RE::TESObjectREFR* a_refr)
	{
		float actorAngle = actor->GetAngleZ() / ((float)std::numbers::pi) * 180;

		RE::NiPoint3 targetPos = target->GetPosition();
		RE::NiPoint3 actorPos = actor->GetPosition();
		RE::NiPoint2 unitVectorToTarget = RE::NiPoint2((targetPos - actorPos).x, (targetPos - actorPos).y);
		unitVectorToTarget = unitVectorToTarget / unitVectorToTarget.Length();
		float angleToTarget = std::atan2(unitVectorToTarget.x, unitVectorToTarget.y) / ((float)std::numbers::pi) * 180;
		angleToTarget = angleToTarget < 0 ? angleToTarget + 360 : angleToTarget;

		float relativeAngleToTarget = std::fabs(actorAngle - angleToTarget);
		relativeAngleToTarget = relativeAngleToTarget > 180 ? 360 - relativeAngleToTarget : relativeAngleToTarget;

		return comparisonComponent->GetComparisonResult(relativeAngleToTarget, numericComponent->GetNumericValue(a_refr));
	}
}
