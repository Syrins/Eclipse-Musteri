#include "user_interface.h"

#pragma warning (disable : 4244) // O_o

struct tab_anim
{
    int hovered_anim;
    int active_anim;
};

bool custom_interface::tab(const char* label, bool selected)
{
    MsTrGWindow* window = MsTrG::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    MsTrGContext& g = *GMsTrG;
    const MsTrGStyle& style = g.Style;
    const MsTrGID id = window->GetID(label);
    const ImVec2 label_size = MsTrG::CalcTextSize(label, NULL, true);
    ImVec2 pos = window->DC.CursorPos;

    const ImRect rect(pos, ImVec2(pos.x + 50, pos.y + 50));
    MsTrG::ItemSize(ImVec4(rect.Min.x, rect.Min.y, rect.Max.x, rect.Max.y + 5), style.FramePadding.y);
    if (!MsTrG::ItemAdd(rect, id))
        return false;

    bool hovered, held;
    bool pressed = MsTrG::ButtonBehavior(rect, id, &hovered, &held, NULL);

    static std::map <MsTrGID, tab_anim> anim;
    auto it_anim = anim.find(id);
    if (it_anim == anim.end())
    {
        anim.insert({ id, {0, 0} });
        it_anim = anim.find(id);
    }

    if (hovered)
        it_anim->second.hovered_anim += 8 * (1.f - MsTrG::GetIO().DeltaTime);
    else
        it_anim->second.hovered_anim -= 8 * (1.f - MsTrG::GetIO().DeltaTime);

    if (it_anim->second.hovered_anim > 50)
        it_anim->second.hovered_anim = 50;
    else if (it_anim->second.hovered_anim < 0)
        it_anim->second.hovered_anim = 0;

    if (selected)
        it_anim->second.active_anim += 16 * (1.f - MsTrG::GetIO().DeltaTime);
    else
        it_anim->second.active_anim -= 16 * (1.f - MsTrG::GetIO().DeltaTime);

    if (it_anim->second.active_anim > 155)
        it_anim->second.active_anim = 155;
    else if (it_anim->second.active_anim < 0)
        it_anim->second.active_anim = 0;

    window->DrawList->AddText(ImVec2(rect.Min.x + 25, rect.Min.y + 25), ImColor(255, 255, 255, 100 + it_anim->second.hovered_anim + it_anim->second.active_anim), label);

    return pressed;
}

struct subtab_anim
{
    int active_text_anim;
    int active_rect_alpha;
};

bool custom_interface::subtab(const char* label, bool selected)
{
    MsTrGWindow* window = MsTrG::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    MsTrGContext& g = *GMsTrG;
    const MsTrGStyle& style = g.Style;
    const MsTrGID id = window->GetID(label);
    const ImVec2 label_size = MsTrG::CalcTextSize(label, NULL, true);
    ImVec2 pos = window->DC.CursorPos;

    const ImRect rect(pos, ImVec2(pos.x + label_size.x, pos.y + 20));
    MsTrG::ItemSize(ImVec4(rect.Min.x, rect.Min.y, rect.Max.x + 2.f, rect.Max.y), style.FramePadding.y);
    if (!MsTrG::ItemAdd(rect, id))
        return false;

    bool hovered, held;
    bool pressed = MsTrG::ButtonBehavior(rect, id, &hovered, &held, NULL);

    static std::map <MsTrGID, subtab_anim> anim;
    auto it_anim = anim.find(id);
    if (it_anim == anim.end())
    {
        anim.insert({ id, {0, 0} });
        it_anim = anim.find(id);
    }

    if (selected)
        it_anim->second.active_text_anim += 16 * (1.f - MsTrG::GetIO().DeltaTime);
    else
        it_anim->second.active_text_anim -= 16 * (1.f - MsTrG::GetIO().DeltaTime);

    if (it_anim->second.active_text_anim > 155)
        it_anim->second.active_text_anim = 155;
    else if (it_anim->second.active_text_anim < 0)
        it_anim->second.active_text_anim = 0;

    if (selected)
        it_anim->second.active_rect_alpha += 20 * (1.f - MsTrG::GetIO().DeltaTime);
    else
        it_anim->second.active_rect_alpha -= 20 * (1.f - MsTrG::GetIO().DeltaTime);

    if (it_anim->second.active_rect_alpha > 255)
        it_anim->second.active_rect_alpha = 255;
    else if (it_anim->second.active_rect_alpha < 0)
        it_anim->second.active_rect_alpha = 0;

    window->DrawList->AddRectFilled(ImVec2(rect.Min.x, rect.Min.y + 22), ImVec2(rect.Max.x, rect.Max.y), ImColor(255, 128, 0, it_anim->second.active_rect_alpha));
    window->DrawList->AddText(ImVec2((rect.Min.x + rect.Max.x) / 2.f - (label_size.x / 2.f), (rect.Min.y + rect.Max.y) / 2.f - (label_size.y / 2.f) - 5), ImColor(255, 255, 255, (100 + it_anim->second.active_text_anim)), label);

    return pressed;
}
