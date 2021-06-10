#version 330 core
out vec4 FragColor;

in vec3 fragTexCoords;

uniform samplerCube u_skybox;

void main()
{    
    vec3 color = texture(u_skybox, fragTexCoords).rgb; 
    // test cubeMap mipmap
    // vec3 color = textureLod(u_skybox, fragTexCoords, 4.0).rgb; 
    color = color / (color + 1.0f);
    color = pow(color, vec3(1.0 / 2.2));
    FragColor = vec4(color, 1.0f);
}