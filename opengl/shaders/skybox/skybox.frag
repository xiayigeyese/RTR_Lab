#version 330 core
out vec4 FragColor;

in vec3 fragTexCoords;

uniform samplerCube u_skybox;

void main()
{    
    vec3 color = texture(u_skybox, fragTexCoords).rgb;
    FragColor = vec4(color, 1.0f);
}