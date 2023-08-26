#include "Hooks.h"

namespace Hooks {
    void Install() {
        stl::write_vfunc<RE::Character, Load3D>();
        logger::info("Installed Character::Load3D hook");
    }

    RE::NiAVObject* Load3D::Thunk(RE::Character* a_this, bool a_arg1) {
        const auto result{ func(a_this, a_arg1) };

        std::jthread([=] {
            std::this_thread::sleep_for(1s);
            SKSE::GetTaskInterface()->AddTask([=] {
                if (!a_this->IsPlayerRef() && a_this->HasKeywordString("ActorTypeNPC"sv) && !a_this->IsPlayerTeammate()) {
                    const auto body_worn{ a_this->GetWornArmor(body_slot) };
                    const auto head_worn{ a_this->GetWornArmor(head_slot) };
                    const auto hands_worn{ a_this->GetWornArmor(hands_slot) };
                    const auto feet_worn{ a_this->GetWornArmor(feet_slot) };
                    if (!(body_worn && head_worn && hands_worn && feet_worn)) {
                        const auto name{ a_this->GetName() };
                        const auto form_id{ a_this->GetFormID() };
                        const auto manager{ RE::ActorEquipManager::GetSingleton() };
                        const auto inv{ a_this->GetInventory([](RE::TESBoundObject& obj) { return true; }, true) };
                        bool       body_found{}, head_found{}, hands_found{}, feet_found{};
                        for (const auto& item : inv | std::views::keys) {
                            if (const auto armo{ item->As<RE::TESObjectARMO>() }) {
                                const auto armo_name{ armo->GetName() };
                                const auto armo_form_id{ armo->GetFormID() };
                                if (armo->GetSlotMask() <=> hands_slot == 0 && !hands_worn && !hands_found) {
                                    hands_found = true;
                                    manager->EquipObject(a_this, item, nullptr, 1, nullptr, true, false, false, false);
                                    logger::debug("{} (0x{:x}): Equipped hands slot item {} (0x{:x}) found in inventory on 3D load", name, form_id,
                                                  armo_name, armo_form_id);
                                }
                                if (armo->GetSlotMask() <=> feet_slot == 0 && !feet_worn && !feet_found) {
                                    feet_found = true;
                                    manager->EquipObject(a_this, item, nullptr, 1, nullptr, true, false, false, false);
                                    logger::debug("{} (0x{:x}): Equipped feet slot item {} (0x{:x}) found in inventory on 3D load", name, form_id,
                                                  armo_name, armo_form_id);
                                }
                                if (armo->GetSlotMask() <=> head_slot == 0 && !head_worn && !head_found) {
                                    head_found = true;
                                    manager->EquipObject(a_this, item, nullptr, 1, nullptr, true, false, false, false);
                                    logger::debug("{} (0x{:x}): Equipped head slot item {} (0x{:x}) found in inventory on 3D load", name, form_id,
                                                  armo_name, armo_form_id);
                                }
                                if (armo->GetSlotMask() <=> body_slot == 0 && !body_worn && !body_found) {
                                    body_found = true;
                                    manager->EquipObject(a_this, item, nullptr, 1, nullptr, true, false, false, false);
                                    logger::debug("{} (0x{:x}): Equipped body slot item {} (0x{:x}) found in inventory on 3D load", name, form_id,
                                                  armo_name, armo_form_id);
                                }
                            }
                        }
                    }
                }
            });
        }).detach();

        return result;
    }
}
