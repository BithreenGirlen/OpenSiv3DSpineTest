#ifndef SIV3D_SPINE_H_
#define SIV3D_SPINE_H_

/* avoid conflict between <MathUtils.h> and <Windows.h> */
#undef min
#undef max
#include <spine/spine.h>

/* avoid ambiguity between s3v::color and spine::color */
#define NO_S3D_USING
#include <Siv3D.hpp>

class CS3dSpineDrawable
{
public:
	CS3dSpineDrawable(spine::SkeletonData* pSkeletonData, spine::AnimationStateData* pAnimationStateData = nullptr);
	~CS3dSpineDrawable();

	spine::Skeleton* skeleton = nullptr;
	spine::AnimationState* animationState = nullptr;
	float timeScale = 1.f;

	bool isAlphaPremultiplied = false;
	bool isBlendModeNormalForced = false;

	void Update(float fDelta);
	void Draw();

	void SetSlotsToLeaveOut(spine::Vector<spine::String>& slotNames);
	void SetLeaveOutCallback(bool (*pFunc)(const char*, size_t)) { m_pLeaveOutCallback = pFunc; }
private:
	bool m_hasOwnAnimationStateData = false;

	spine::Vector<float> m_worldVertices;

	s3d::Buffer2D m_buffer2d;

	spine::Vector<unsigned short> m_quadIndices;

	spine::SkeletonClipping m_skeletonClipping;

	spine::Vector<spine::String> m_slotsToLeaveOut;

	bool IsSlotToBeLeftOut(const spine::String& slotName);
	bool (*m_pLeaveOutCallback)(const char*, size_t) = nullptr;
};

class CS3dTextureLoader : public spine::TextureLoader
{
public:
	CS3dTextureLoader() {};
	virtual ~CS3dTextureLoader() {};

	virtual void load(spine::AtlasPage& page, const spine::String& path);
	virtual void unload(void* texture);
};

#endif // SIV3D_SPINE_H_
