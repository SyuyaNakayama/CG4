struct DirLight
{
	float3 lightv; // ���C�g�ւ̕����̒P�ʃx�N�g��
	float3 lightcolor; // ���C�g�̐F(RGB)
	uint active;
};

struct PointLight
{
	float3 lightpos; // ���C�g���W
	float3 lightcolor; // ���C�g�̐F(RGB)
	float3 lightatten; // ���C�g���������W��
	uint active;
};

struct SpotLight
{
	float3 lightv; // ���C�g�̌��������̋t�x�N�g��
	float3 lightpos; // ���C�g���W
	float3 lightcolor; // ���C�g�̐F(RGB)
	float3 lightatten; // ���C�g���������W��
	float2 lightfactoranglecos; // ���C�g�����p�x��cos
	uint active;
};

struct CircleShadow
{
	float3 dir; // ���e�����̋t�x�N�g��(�P�ʃx�N�g��)
	float3 casterPos; // �L���X�^�[���W
	float distanceCasterLight; // �L���X�^�[�ƃ��C�g�̋���
	float3 atten; // ���������W��
	float2 factorAngleCos; // �����p�x��cos
	uint active;
};

static const uint DIRLIGHT_NUM = 3;
static const uint POINTLIGHT_NUM = 3;
static const uint SPOTLIGHT_NUM = 3;
static const uint CIRCLESHADOW_NUM = 1;