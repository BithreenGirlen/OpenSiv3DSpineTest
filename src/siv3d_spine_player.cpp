

#include "siv3d_spine_player.h"

CSiv3dSpinePlayer::CSiv3dSpinePlayer()
{

}

CSiv3dSpinePlayer::~CSiv3dSpinePlayer()
{

}

void CSiv3dSpinePlayer::Redraw()
{
	if (!m_drawables.empty())
	{
		/* 
		 * 描画時の変換とは別に、転送時にsiv3d側で縮小されるので元に戻す。
		 * https://siv3d.github.io/ja-jp/tutorial3/scene/#448-%E3%82%B7%E3%83%BC%E3%83%B3%E6%8B%A1%E5%A4%A7%E7%B8%AE%E5%B0%8F%E3%83%95%E3%82%A3%E3%83%AB%E3%82%BF
		 */
		float fScale = m_fSkeletonScale / m_fDefaultScale;

		float fX = (m_fBaseSize.x * fScale - m_sceneSize.x) / 2;
		float fY = (m_fBaseSize.y * fScale - m_sceneSize.y) / 2;
		const s3d::Mat3x2 matrix = s3d::Mat3x2::Scale(fScale).translated(-fX, -fY);
		const s3d::Transformer2D t(matrix);

		if (!m_bDrawOrderReversed)
		{
			for (size_t i = 0; i < m_drawables.size(); ++i)
			{
				m_drawables[i]->Draw();
			}
		}
		else
		{
			for (long long i = m_drawables.size() - 1; i >= 0; --i)
			{
				m_drawables[i]->Draw();
			}
		}
	}
}

void CSiv3dSpinePlayer::WorkOutDefaultScale()
{
	m_fDefaultScale = 1.f;
	m_fDefaultOffset = s3d::Vector2D<float>();

	/* 描画可能領域に合わせて拡縮調整 */

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
