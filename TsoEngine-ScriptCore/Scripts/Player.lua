-- player.lua
local Keycode = require("script.core.Keycode")
local Player = {}
Player.__index = Player

Player.Fields = {
    uuid = "System.Byte",
    Player = "Tso.Entity",
    Speed = "System.Single"
}

-- 构造函数现在接收一个 uuid
function Player.new(uuid)
    local instance = setmetatable({}, Player)
    instance.uuid = uuid -- 存储这个ID
    instance.Speed = 10.0
    World.Log("get uuid :" .. uuid)
    
    -- 我们可以立即用这个ID获取 C++ Entity 对象
    -- World.GetEntityByUUID 是你需要在 C++ 中暴露的函数
    instance.entity = World.GetEntityByUUID(uuid)
    
    return instance
end

function Player:OnCreate()
    -- 现在可以直接用 self.entity
    -- print("Player has been created! My entity ID is: " .. tostring(self.uuid))
    World.Log("Player script created for entity: " ..  tostring(self.uuid))
    if self.entity then
        print("Successfully fetched C++ entity object.")
    end
end

function Player:OnUpdate(ts)
    if not self.entity then return end

    if World.IsKeyPressed(Keycode.TSO_KEY_W) then
        local transform = self.entity:GetComponent("TransformComponent")
        if transform then
            transform:Translate({ x = 0, y = self.Speed * ts, z = 0 })
        end
        local render = self.entity:GetComponent("Renderable")
        if render then
            render:ChangeSpriteIndex({ x =1, y =0 })
        end
    end
    if World.IsKeyPressed(Keycode.TSO_KEY_S) then
        
        local transform = self.entity:GetComponent("TransformComponent")
        if transform then
            transform:Translate({ x = 0, y = - self.Speed * ts, z = 0 })
        end
        local render = self.entity:GetComponent("Renderable")
        if render then
            render:SetSpriteIndex({ x = 1, y =0 })
        end
    end

    -- local transform = self.entity:GetComponent("Transform")
    -- ...
end

return Player