#version 330 core
 
in vec3 FragPos;  
in vec3 Normal; 
in vec3 inLightPos;
in vec3 inViewPos;
in vec3 dir;
in vec3 ex_Color; 
 
out vec4 out_Color;
 
uniform vec3 lightColor;
uniform int codCol;
uniform float ambientStrength;
 
void main(void)
{
    switch (codCol) {
        case 0:
            // Ambient
            float _ambientStrength = ambientStrength;
            vec3 ambient = _ambientStrength * lightColor;
            
            // Diffuse 
            vec3 normala = normalize(Normal);
            vec3 lightDir = normalize(inLightPos - FragPos);
            //vec3 lightDir = normalize(dir);
            float diff = max(dot(normala, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;
            
            // Specular
            float specularStrength = 0.5f;
            vec3 viewDir = normalize(inViewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, normala);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
            vec3 specular = specularStrength * spec * lightColor;  
            vec3 emission=vec3(0.0, 0.0, 0.0);
            vec3 result = emission + (ambient + diffuse + specular) * ex_Color;
            out_Color = vec4(result, 1.0f);
            
            break;
            
        case 1:
            vec3 black = vec3 (0.0, 0.0, 0.0);
            out_Color = vec4 (black, 1.0);
    }
}