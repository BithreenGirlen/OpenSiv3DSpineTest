

#include "siv3d_spine_player.h"

CSiv3dSpinePlayer::CSiv3dSpinePlayer()
{

}

CSiv3dSpinePlayer::~CSiv3dSpinePlayer()
{

}

void CSiv3dSpinePlayer::Redraw(float fDelta)
{
	if (!m_drawables.empty())
	{
		auto screenSize = s3d::Scene::Size();
		float fX = (m_fBaseSize.x * m_fSkeletonScale - screenSize.x) / 2;
		float fY = (m_fBaseSize.y * m_fSkeletonScale - screenSize.y) / 2;
		const s3d::Mat3x2 matrix = s3d::Mat3x2::Scale(m_fSkeletonScale).translated(-fX, -fY);
		const s3d::Transformer2D t(matrix);

		if (!m_bDrawOrderReversed)
		{
			for (size_t i = 0; i < m_drawables.size(); ++i)
			{
				m_drawables[i]->Update(fDelta);
				m_drawables[i]->Draw();
			}
		}
		else
		{
			for (long long i = m_drawables.size() - 1; i >= 0; --i)
			{
				m_drawables[i]->Update(fDelta);
				m_drawables[i]->Draw();
			}
		}
	}
}

void CSiv3dSpinePlayer::WorkOutDefaultScale()
{
	m_fDefaultScale = 1.f;
	m_fDefaultOffset = s3d::Vector2D<float>();

	/* デスクトップ寸法・描画可能領域に合わせて拡縮調整 */

	int iSkeletonWidth = static_cast<int>(m_fBaseSize.x);
	int iSkeletonHeight = static_cast<int>(m_fBaseSize.y);

	auto monitor = s3d::System::GetCurrentMonitor();
	int iDisplayWidth = monitor.displayRect.w;
	int iDisplayHeight = monitor.displayRect.h;
	if (iSkeletonWidth > iDisplayWidth || iSkeletonHeight > iDisplayHeight)
	{
		float fScaleX = static_cast<float>(iDisplayWidth) / iSkeletonWidth;
		float fScaleY = static_cast<float>(iDisplayHeight) / iSkeletonHeight;

		if (fScaleX > fScaleY)
		{
			m_fDefaultScale = fScaleY;
		}
		else
		{
			m_fDefaultScale = fScaleX;
		}
	}
}
