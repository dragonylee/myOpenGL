#version 430 core

// 从前面传下来的数据
in vec3 fragNormal;     // 片段法向量（非单位向量）
in vec3 FragPos;        // 片段位置
in vec2 TexCoords;      // 纹理采样坐标

// 片段着色器的颜色输出
out vec4 FragColor;

// 定义物体材质，因为这里用了图片纹理做光照贴图，因此就可以用采样器直接获得对应坐标的RGB
// 也可以直接定义纯色的RGB向量
struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;    // 反光度：越高，反光能力越强，散射的越少，高光点越小
};
uniform Material material;  // 物体材质
uniform vec3 viewPos;       // 观察者位置


// 各个光源类型的定义，以及对应求解反射光的函数定义
// 函数参数的normal、viewDir必须是单位向量

//----------------- 方向光 ------------------
struct DirLight
{
    vec3 direction;     // 光照方向
    // 三个光照分量
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 calDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // 环境光照
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    // 漫反射光照
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    // 镜面光照
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);   // 材质中【反光度】决定的系数
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    // 合并
    return (ambient + diffuse + specular);
}
//-------------------------------------------

//----------------- 点光源 ------------------
struct PointLight
{
    vec3 position;  // 光源位置
    // 三个衰减系数
    float constant;
    float linear;
    float quadratic;
    // 三个光照分量
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 calPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // 环境光照
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    // 漫反射光照
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    // 镜面光照
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);   // 材质中【反光度】决定的系数
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    // 合并，考虑光强衰减
    float distance = length(light.position - fragPos);
    float luminosity = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);   // 光强衰减系数
    return (ambient + diffuse + specular) * luminosity;
}
//-------------------------------------------

//----------------- 聚光灯 ------------------
struct SpotLight
{
    vec3 direction;     // 光照方向
    vec3 position;      // 光源位置
    float innerCos;     // 内圆锥半角余弦值
    float outerCos;     // 外圆锥半角余弦值
    // 三个衰减系数
    float constant;
    float linear;
    float quadratic;
    // 三个光照分量
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 calSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // 环境光照
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    // 漫反射光照
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    // 镜面光照
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);   // 材质中【反光度】决定的系数
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    // 考虑光强衰减
    float distance = length(light.position - fragPos);
    float luminosity = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);   // 光强衰减系数
    // 从这里开始加入聚光灯的判断
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCos - light.outerCos;
    float intensity = clamp((theta - light.outerCos) / epsilon, 0.0, 1.0);    // 聚光灯决定的光强系数

    return (ambient + diffuse + specular) * luminosity * intensity;
}
//-------------------------------------------


// ！！！！！ 前面部分定义了一些到目前为止必需的东西，而后面可以根据自己的需要进行编写 ！！！！！
// 比如后面，我定义了一个方向光、一个聚光灯和四个点光源

uniform DirLight dirLight;
#define NR_POINT_LIGHTS 1
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform int isSpotLight;    // 用这个控制聚光灯的亮灭

void main()
{
    vec3 norm = normalize(fragNormal);              // 法向量的单位向量
    vec3 viewDir = normalize(viewPos - FragPos);    // 观察方向的单位向量
    vec3 result = vec3(0.0f, 0.0f, 0.0f);

    // 后面直接调用函数就可以很方便地计算出每种光照的影响
    result += calDirLight(dirLight, norm, viewDir);
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += calPointLight(pointLights[i], norm, FragPos, viewDir);
    if(isSpotLight == 1)
        result += calSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result*2, 1.0f);
    //FragColor = texture(material.texture_diffuse1, TexCoords);
}

