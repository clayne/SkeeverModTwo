#include "Hooks.h"

namespace Hooks
{
    void Install() noexcept
    {
        stl::write_vfunc<RE::Character, Load3D>();
        logger::info("Installed Character::Load3D hook");
    }

    RE::NiAVObject* Load3D::Thunk(RE::Character* a_this, bool a_arg1)
    {
        if (!a_this) {
            return func(a_this, a_arg1);
        }

        if (!a_this->GetActorBase()) {
            return func(a_this, a_arg1);
        }

        if (!a_this->IsDead() || a_this->IsPlayerRef() || a_this->IsPlayerTeammate() || a_this->IsChild() || !a_this->HasKeywordString("ActorTypeNPC"sv)) {
            return func(a_this, a_arg1);
        }

        if (const auto race{ a_this->GetRace() }) {
            if (!strcmp(race->GetFormEditorID(), "ManakinRace")) {
                return func(a_this, a_arg1);
            }
        }

        const auto body_worn{ a_this->GetWornArmor(body_slot) };
        const auto head_worn{ a_this->GetWornArmor(head_slot) };
        const auto hands_worn{ a_this->GetWornArmor(hands_slot) };
        const auto feet_worn{ a_this->GetWornArmor(feet_slot) };

        if (body_worn) {
            return func(a_this, a_arg1);
        }

        const auto actor_name{ a_this->GetName() };
        const auto actor_form_id{ a_this->GetFormID() };
        const auto manager{ RE::ActorEquipManager::GetSingleton() };
        const auto inv{ a_this->GetInventory(RE::TESObjectREFR::DEFAULT_INVENTORY_FILTER, true) };
        bool       body_found{}, head_found{}, hands_found{}, feet_found{};
        for (const auto& item : inv | std::views::keys) {
            if (const auto armo{ item->As<RE::TESObjectARMO>() }) {
                const auto armo_name{ armo->GetName() };
                const auto armo_form_id{ armo->GetFormID() };
                if (armo->HasPartOf(hands_slot) && !hands_worn && !hands_found) {
                    hands_found = true;
                    manager->EquipObject(a_this, armo, nullptr, 1, nullptr, true, false, false, false);
                    logger::debug("{} (0x{:x}): Equipped hands slot item {} (0x{:x}) found in inventory on 3D load", actor_name, actor_form_id, armo_name, armo_form_id);
                }
                if (armo->HasPartOf(feet_slot) && !feet_worn && !feet_found) {
                    feet_found = true;
                    manager->EquipObject(a_this, armo, nullptr, 1, nullptr, true, false, false, false);
                    logger::debug("{} (0x{:x}): Equipped feet slot item {} (0x{:x}) found in inventory on 3D load", actor_name, actor_form_id, armo_name, armo_form_id);
                }
                if (armo->HasPartOf(head_slot) && !head_worn && !head_found) {
                    head_found = true;
                    manager->EquipObject(a_this, armo, nullptr, 1, nullptr, true, false, false, false);
                    logger::debug("{} (0x{:x}): Equipped head slot item {} (0x{:x}) found in inventory on 3D load", actor_name, actor_form_id, armo_name, armo_form_id);
                }
                if (armo->HasPartOf(body_slot) && !body_found) {
                    body_found = true;
                    manager->EquipObject(a_this, armo, nullptr, 1, nullptr, true, false, false, false);
                    logger::debug("{} (0x{:x}): Equipped body slot item {} (0x{:x}) found in inventory on 3D load", actor_name, actor_form_id, armo_name, armo_form_id);
                }
            }
        }

        return func(a_this, a_arg1);
    }
} // namespace Hooks
