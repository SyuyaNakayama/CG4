struct DirLight
{
	float3 lightv; // ライトへの方向の単位ベクトル
	float3 lightcolor; // ライトの色(RGB)
	uint active;
};

struct PointLight
{
	float3 lightpos; // ライト座標
	float3 lightcolor; // ライトの色(RGB)
	float3 lightatten; // ライト距離減衰係数
	uint active;
};

struct SpotLight
{
	float3 lightv; // ライトの光線方向の逆ベクトル
	float3 lightpos; // ライト座標
	float3 lightcolor; // ライトの色(RGB)
	float3 lightatten; // ライト距離減衰係数
	float2 lightfactoranglecos; // ライト減衰角度のcos
	uint active;
};

struct CircleShadow
{
	float3 dir; // 投影方向の逆ベクトル(単位ベクトル)
	float3 casterPos; // キャスター座標
	float distanceCasterLight; // キャスターとライトの距離
	float3 atten; // 距離減衰係数
	float2 factorAngleCos; // 減衰角度のcos
	uint active;
};

static const uint DIRLIGHT_NUM = 3;
static const uint POINTLIGHT_NUM = 3;
static const uint SPOTLIGHT_NUM = 3;
static const uint CIRCLESHADOW_NUM = 1;