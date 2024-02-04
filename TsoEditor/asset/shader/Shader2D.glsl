#type vertex

#version 330 core
           
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in int a_EntityID;



uniform mat4 u_ProjViewMat;
//uniform mat4 u_Transform;

out vec3 v_Position;
out vec2 v_TexCoord;
out vec4 v_Color;
out float v_TexIndex;
flat out int v_EntityID;

void main(){
    v_Position = a_Position;
    v_TexCoord = a_TexCoord;
    v_Color = a_Color;
    v_TexIndex = a_TexIndex;
    v_EntityID = a_EntityID;
    gl_Position = u_ProjViewMat * vec4(a_Position , 1.0);
}


#type fragment

#version 330 core
           
layout(location = 0) out vec4 color;
layout(location = 1) out int o_EntityID;
in vec3 v_Position;
in vec2 v_TexCoord;
in vec4 v_Color;
in float v_TexIndex;
flat in int v_EntityID;



uniform sampler2D u_Textures[16];

void main(){
    color =  texture(u_Textures[int(v_TexIndex)] , v_TexCoord) * v_Color;
    o_EntityID = v_EntityID;
}
