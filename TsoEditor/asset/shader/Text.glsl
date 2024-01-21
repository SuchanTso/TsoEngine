#type vertex

#version 330 core
           
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;



uniform mat4 u_ProjViewMat;
//uniform mat4 u_Transform;

out vec3 v_Position;
out vec2 v_TexCoord;
out vec4 v_Color;

void main(){
    v_Position = a_Position;
    v_TexCoord = a_TexCoord;
    v_Color = a_Color;
    gl_Position = u_ProjViewMat * vec4(a_Position , 1.0);
}


#type fragment

#version 330 core
           
layout(location = 0) out vec4 color;
in vec3 v_Position;
in vec2 v_TexCoord;
in vec4 v_Color;

uniform vec4 fgColor;

uniform sampler2D u_Textures;

float screenPxRange() {
    const float pxRange = 2.0;
    vec2 unitRange = vec2(pxRange)/vec2(textureSize(u_Textures, 0));
    vec2 screenTexSize = vec2(1.0)/fwidth(v_TexCoord);
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main(){
    color =  texture(u_Textures[int(v_TexIndex)] , v_TexCoord) * v_Color;
    //color = vec4(v_TexIndex , 0.0 , 0.0 ,1.0);

    const vec4 bgColor = vec4(0.0, 0.0, 0.0, 1.0);
    vec3 msd = texture(u_Textures, v_TexCoord).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange()*(sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    color = mix(bgColor, v_Color, opacity);
}
