#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 fragTexCoords;

uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    fragTexCoords = aPos;
    vec4 pos = u_projection * u_view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}  