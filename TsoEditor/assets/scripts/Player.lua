-- [MODIFIED] 使用标准的 require 路径
local Keycode = require("core.Keycode")

local Player = {}
Player.__index = Player

-- [MODIFIED] 修正了 Fields 表中 uuid 的类型
Player.Fields = {
    -- uuid 字段不再需要，因为它是实体自带的，而不是一个可编辑的公共字段
    -- Player = "Tso.Entity", -- 如果Player是一个实体引用字段
    Speed = "System.Single"
}

-- [MODIFIED] 构造函数使用冒号语法，接收 self 和 uuid_str
function Player:new(uuid_str)
    local instance = setmetatable({}, Player)
    
    instance.uuid = uuid_str -- 存储字符串形式的ID
    instance.Speed = 10.0
    
    -- 通过 World 全局API获取与此脚本实例关联的C++ Entity对象
    instance.entity = World.GetEntityByUUID(uuid_str)
    
    return instance
end

function Player:OnCreate()
    World.Log("Player script created for entity: " .. tostring(self.uuid))
    if self.entity then
        World.Log("Successfully fetched C++ entity object in OnCreate.")
    else
        World.Log("Warning: Could not fetch C++ entity object in OnCreate.")
    end
end

function Player:OnUpdate(ts)
    -- 确保我们有关联的C++实体
    if not self.entity then return end

    -- 获取 TransformComponent
    local transform = self.entity:GetComponent("TransformComponent")
    if not transform then return end
    
    -- [MODIFIED] 正确地操作 usertype
    -- 1. 获取位置对象 (这是一个 userdata，代表 glm::vec3)
    local pos = transform.position -- 使用我们绑定的 sol::property
    
    -- 2. 在 Lua 中修改位置
    if World.IsKeyPressed(Keycode.TSO_KEY_W) then
        pos.y = pos.y + self.Speed * ts
    end
    if World.IsKeyPressed(Keycode.TSO_KEY_S) then
        pos.y = pos.y - self.Speed * ts
    end
    if World.IsKeyPressed(Keycode.TSO_KEY_A) then
        pos.x = pos.x - self.Speed * ts
    end
    if World.IsKeyPressed(Keycode.TSO_KEY_D) then
        pos.x = pos.x + self.Speed * ts
    end

    -- 3. 将修改后的位置对象设置回去 (如果 position 是属性，这一步是可选的，因为我们直接修改了C++对象)
    -- transform.position = pos 
    -- 由于 sol::property 直接修改了 C++ 内存，所以上面这行通常不需要，但写上更清晰。

    -- [MODIFIED] 示范如何操作 Renderable
    local renderable = self.entity:GetComponent("Renderable")
    if renderable then
        -- 假设我们为 Renderable 也绑定了 'spriteIndex' 属性
        -- local sprite_index = renderable.spriteIndex
        -- if World.IsKeyPressed(Keycode.TSO_KEY_W) then
        --     sprite_index.y = 1 -- 假设向上走的动画在第1行
        -- end
        -- renderable.spriteIndex = sprite_index
    end
end

return Player