

#include "siv3d_spine.h"
#include "siv3d_spine_blendmode.h"

namespace spine
{
	spine::SpineExtension* getDefaultExtension()
	{
		static DefaultSpineExtension s_defaultSpineExtension;
		return &s_defaultSpineExtension;
	}
}

CS3dSpineDrawable::CS3dSpineDrawable(spine::SkeletonData* pSkeletonData, spine::AnimationStateData* pAnimationStateData)
{
	spine::Bone::setYDown(true);

	skeleton = new spine::Skeleton(pSkeletonData);

	if (pAnimationStateData == nullptr)
	{
		pAnimationStateData = new spine::AnimationStateData(pSkeletonData);
		m_hasOwnAnimationStateData = true;
	}
	animationState = new spine::AnimationState(pAnimationStateData);

	m_quadIndices.add(0);
	m_quadIndices.add(1);
	m_quadIndices.add(2);
	m_quadIndices.add(2);
	m_quadIndices.add(3);
	m_quadIndices.add(0);
}

CS3dSpineDrawable::~CS3dSpineDrawable()
{
	if (animationState != nullptr)
	{
		if (m_hasOwnAnimationStateData)
		{
			delete animationState->getData();
		}

		delete animationState;
	}
	if (skeleton != nullptr)
	{
		delete skeleton;
	}
}

void CS3dSpineDrawable::Update(float fDelta)
{
	if (skeleton != nullptr && animationState != nullptr)
	{
#ifndef SPINE_4_1_OR_LATER
		skeleton->update(fDelta);
#endif
		animationState->update(fDelta * timeScale);
		animationState->apply(*skeleton);
		skeleton->updateWorldTransform();
	}
}

void CS3dSpineDrawable::Draw()
{
	if (skeleton == nullptr || animationState == nullptr)return;

	if (skeleton->getColor().a == 0)return;

	for (size_t i = 0; i < skeleton->getSlots().size(); ++i)
	{
		spine::Slot& slot = *skeleton->getDrawOrder()[i];
		spine::Attachment* pAttachment = slot.getAttachment();

		if (pAttachment == nullptr || slot.getColor().a == 0 || !slot.getBone().isActive())
		{
			m_skeletonClipping.clipEnd(slot);
			continue;
		}

		if (IsSlotToBeLeftOut(slot.getData().getName()))
		{
			m_skeletonClipping.clipEnd(slot);
			continue;
		}

		spine::Vector<float>* pVertices = &m_worldVertices;
		spine::Vector<float>* pAttachmentUvs = nullptr;
		spine::Vector<unsigned short>* pIndices = nullptr;

		spine::Color* pAttachmentColor = nullptr;

		s3d::Texture *pTexture = nullptr;

		if (pAttachment->getRTTI().isExactly(spine::RegionAttachment::rtti))
		{
			spine::RegionAttachment* pRegionAttachment = static_cast<spine::RegionAttachment*>(pAttachment);
			pAttachmentColor = &pRegionAttachment->getColor();

			if (pAttachmentColor->a == 0)
			{
				m_skeletonClipping.clipEnd(slot);
				continue;
			}

			m_worldVertices.setSize(8, 0);
#ifdef SPINE_4_1_OR_LATER
			pRegionAttachment->computeWorldVertices(slot, m_worldVertices, 0, 2);
#else
			pRegionAttachment->computeWorldVertices(slot.getBone(), m_worldVertices, 0, 2);
#endif
			pAttachmentUvs = &pRegionAttachment->getUVs();
			pIndices = &m_quadIndices;

#ifdef SPINE_4_1_OR_LATER
			spine::AtlasRegion* pAtlasRegion = static_cast<spine::AtlasRegion*>(pRegionAttachment->getRegion());

			isAlphaPremultiplied = pAtlasRegion->page->pma;
			pTexture = reinterpret_cast<s3d::Texture*>(pAtlasRegion->rendererObject);
#else
			spine::AtlasRegion* pAtlasRegion = static_cast<spine::AtlasRegion*>(pRegionAttachment->getRendererObject());
#ifdef SPINE_4_0
			isAlphaPremultiplied = pAtlasRegion->page->pma;
#endif
			pTexture = reinterpret_cast<s3d::Texture*>(pAtlasRegion->page->getRendererObject());
#endif
		}
		else if (pAttachment->getRTTI().isExactly(spine::MeshAttachment::rtti))
		{
			spine::MeshAttachment* pMeshAttachment = static_cast<spine::MeshAttachment*>(pAttachment);
			pAttachmentColor = &pMeshAttachment->getColor();

			if (pAttachmentColor->a == 0)
			{
				m_skeletonClipping.clipEnd(slot);
				continue;
			}
			m_worldVertices.setSize(pMeshAttachment->getWorldVerticesLength(), 0);
			pMeshAttachment->computeWorldVertices(slot, 0, pMeshAttachment->getWorldVerticesLength(), m_worldVertices, 0, 2);
			pAttachmentUvs = &pMeshAttachment->getUVs();
			pIndices = &pMeshAttachment->getTriangles();

#ifdef SPINE_4_1_OR_LATER
			spine::AtlasRegion* pAtlasRegion = static_cast<spine::AtlasRegion*>(pMeshAttachment->getRegion());

			isAlphaPremultiplied = pAtlasRegion->page->pma;
			pTexture = reinterpret_cast<s3d::Texture*>(pAtlasRegion->rendererObject);
#else
			spine::AtlasRegion* pAtlasRegion = static_cast<spine::AtlasRegion*>(pMeshAttachment->getRendererObject());
#ifdef SPINE_4_0
			isAlphaPremultiplied = pAtlasRegion->page->pma;
#endif
			pTexture = reinterpret_cast<s3d::Texture*>(pAtlasRegion->page->getRendererObject());
#endif
		}
		else if (pAttachment->getRTTI().isExactly(spine::ClippingAttachment::rtti))
		{
			spine::ClippingAttachment* pClippingAttachment = static_cast<spine::ClippingAttachment*>(slot.getAttachment());
			m_skeletonClipping.clipStart(slot, pClippingAttachment);
			continue;
		}
		else
		{
			m_skeletonClipping.clipEnd(slot);
			continue;
		}

		if (m_skeletonClipping.isClipping())
		{
			m_skeletonClipping.clipTriangles(m_worldVertices, *pIndices, *pAttachmentUvs, 2);
			if (m_skeletonClipping.getClippedTriangles().size() == 0)
			{
				m_skeletonClipping.clipEnd(slot);
				continue;
			}
			pVertices = &m_skeletonClipping.getClippedVertices();
			pAttachmentUvs = &m_skeletonClipping.getClippedUVs();
			pIndices = &m_skeletonClipping.getClippedTriangles();
		}

		const spine::Color& skeletonColor = skeleton->getColor();
		const spine::Color& slotColor = slot.getColor();
		spine::Color tint
		(
			skeletonColor.r * slotColor.r * pAttachmentColor->r,
			skeletonColor.g * slotColor.g * pAttachmentColor->g,
			skeletonColor.b * slotColor.b * pAttachmentColor->b,
			skeletonColor.a * slotColor.a * pAttachmentColor->a
		);

		/* Spineから受け取った描画データをSiv3Dの形式に合わせる。 */
		m_buffer2d.vertices.clear();
		for (int ii = 0; ii < pVertices->size(); ii += 2)
		{
			s3d::Vertex2D s3dVertex{};

			s3dVertex.pos.x = (*pVertices)[ii];
			s3dVertex.pos.y = (*pVertices)[ii + 1LL];

			s3dVertex.color.x = tint.r * (isAlphaPremultiplied ? tint.a : 1.f);
			s3dVertex.color.y = tint.g * (isAlphaPremultiplied ? tint.a : 1.f);
			s3dVertex.color.z = tint.b * (isAlphaPremultiplied ? tint.a : 1.f);
			s3dVertex.color.w = tint.a;

			s3dVertex.tex.x = (*pAttachmentUvs)[ii];
			s3dVertex.tex.y = (*pAttachmentUvs)[ii + 1LL];

			m_buffer2d.vertices.push_back(s3dVertex);
		}

		m_buffer2d.indices.clear();
		for (int ii = 0; ii < pIndices->size(); ii +=3)
		{
			s3d::TriangleIndex triangleIndex{};

			triangleIndex.i0 = (*pIndices)[ii];
			triangleIndex.i1 = (*pIndices)[ii + 1LL];
			triangleIndex.i2 = (*pIndices)[ii + 2LL];

			m_buffer2d.indices.push_back(triangleIndex);
		}

		s3d::BlendState s3dBlendState;
		spine::BlendMode spineBlendMode = isBlendModeNormalForced ? spine::BlendMode::BlendMode_Normal : slot.getData().getBlendMode();
		switch (spineBlendMode)
		{
		case spine::BlendMode_Additive:
			s3dBlendState = isAlphaPremultiplied ? Siv3dSpineBlendMode::AddPma : Siv3dSpineBlendMode::Add;
			break;
		case spine::BlendMode_Multiply:
			s3dBlendState = Siv3dSpineBlendMode::Multiply;
			break;
		case spine::BlendMode_Screen:
			s3dBlendState = Siv3dSpineBlendMode::Screen;
			break;
		default:
			s3dBlendState = isAlphaPremultiplied ? Siv3dSpineBlendMode::NormalPma : Siv3dSpineBlendMode::Normal;
			break;
		}

		s3d::ScopedRenderStates2D s3dScopedRenderState2D(s3dBlendState, s3d::SamplerState::ClampLinear);
		pTexture != nullptr ? m_buffer2d.draw(*pTexture) : m_buffer2d.draw();

		m_skeletonClipping.clipEnd(slot);
	}
	m_skeletonClipping.clipEnd();
}

void CS3dSpineDrawable::SetSlotsToLeaveOut(spine::Vector<spine::String>& slotNames)
{
	m_slotsToLeaveOut.clear();
	for (size_t i = 0; i < m_slotsToLeaveOut.size(); ++i)
	{
		m_slotsToLeaveOut.add(slotNames[i].buffer());
	}
}

bool CS3dSpineDrawable::IsSlotToBeLeftOut(const spine::String& slotName)
{
	if (m_pLeaveOutCallback != nullptr)
	{
		return m_pLeaveOutCallback(slotName.buffer(), slotName.length());
	}
	else
	{
		for (size_t i = 0; i < m_slotsToLeaveOut.size(); ++i)
		{
			if (strcmp(slotName.buffer(), m_slotsToLeaveOut[i].buffer()) == 0)return true;
		}
	}

	return false;
}

void CS3dTextureLoader::load(spine::AtlasPage& page, const spine::String& path)
{
	s3d::FilePath filePath = s3d::Unicode::FromUTF8(path.buffer());
	if (filePath.starts_with(U"//"))
	{
		filePath.replace(U"/", U"\\");
	}
	s3d::Texture* pTexture = new s3d::Texture(filePath);
	if (pTexture == nullptr)return;

	if (page.width == 0 || page.height == 0)
	{
		page.width = pTexture->width();
		page.height = pTexture->height();
	}

#ifdef SPINE_4_1_OR_LATER
	page.texture = pTexture;
#else
	page.setRendererObject(pTexture);
#endif
}

void CS3dTextureLoader::unload(void* texture)
{
	delete static_cast<s3d::Texture*>(texture);
}
