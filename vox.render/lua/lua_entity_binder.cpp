//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "lua_entity_binder.h"
#include "entity.h"

namespace vox {
void LuaEntityBinder::bindEntity(sol::state& luaState) {
    luaState.new_usertype<Entity>("entity");
}

}
