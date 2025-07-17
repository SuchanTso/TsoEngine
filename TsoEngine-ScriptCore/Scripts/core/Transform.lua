-- assets/scripts/components/TransformComponent.lua

-- 这个模块包装了 C++ 绑定的 TransformComponent
local TransformComponent = {}
TransformComponent.__index = TransformComponent

-- 构造函数，接收一个 userdata
function TransformComponent.new(transform_userdata)
    if not transform_userdata then return nil end
    -- 直接将我们的方法设置给这个 userdata
    return setmetatable(transform_userdata, TransformComponent)
end

-- GetPosition/SetPosition 是 C++ 绑定的，在元表里已经有了，这里不需要重复定义

-- 在 Lua 中定义的高级、复合函数
function TransformComponent:Translate(offset_vec)
    local pos = self:GetPosition() -- 调用 C++ 绑定的 GetPosition
    pos.x = pos.x + offset_vec.x
    pos.y = pos.y + offset_vec.y
    pos.z = pos.z + offset_vec.z
    -- 因为 GetPosition 返回的是一个可变的 userdata 指针，
    -- 上面的修改已经直接作用于 C++内存，所以不需要再 SetPosition
end

function TransformComponent:LookAt(target_pos)
    World.Log("LookAt function called (implemented in Lua)")
    -- 这里可以实现复杂的 LookAt 逻辑
end

return TransformComponent