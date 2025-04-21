
#include <Siv3D.hpp>

#include "siv3d_main_window.h"

#include "siv3d_spine_player.h"


CSiv3dMainWindow::CSiv3dMainWindow(const char32_t* windowName)
{
	if (windowName != nullptr)
	{
		s3d::Window::SetTitle(windowName);
		s3d::Window::SetStyle(s3d::WindowStyle::Sizable);
	}
}

CSiv3dMainWindow::~CSiv3dMainWindow()
{

}

void CSiv3dMainWindow::Display()
{
	auto selectedAtlas = s3d::Dialog::OpenFile({ { U"Atlas file", { U"atlas" , U"atlas.txt" } } }, U"", U"Select atlas");
	if (!selectedAtlas.has_value())return;

	auto selectedSkeleton = s3d::Dialog::OpenFile({ { U"Skeleton file", { U"skel" , U"skel.txt", U"bin" } } }, U"", U"Select skeleton");
	if (!selectedSkeleton.has_value())return;

	std::vector<std::string> atlasPaths;
	std::vector<std::string> skelPaths;

	atlasPaths.push_back(s3d::Unicode::ToUTF8(selectedAtlas.value()));
	skelPaths.push_back(s3d::Unicode::ToUTF8(selectedSkeleton.value()));

	CSiv3dSpinePlayer sS3dSpinePlayer;
	sS3dSpinePlayer.SetSpineFromFile(atlasPaths, skelPaths, true);
	
	const auto ResizeWindow = [&]()
		-> void
		{
			s3d::Vector2D<float> fCanvasSize = sS3dSpinePlayer.GetBaseSize();
			float fScale = sS3dSpinePlayer.GetCanvasScale();
			s3d::Window::Resize(static_cast<s3d::int32>(fCanvasSize.x * fScale), static_cast<s3d::int32>(fCanvasSize.y * fScale), s3d::YesNo<s3d::Centering_tag>::Yes);
		};

	ResizeWindow();

	s3d::Point mouseStartPos{};
	while (s3d::System::Update())
	{
		if (s3d::MouseL.down())
		{
			mouseStartPos = s3d::Cursor::Pos();
		}
		else if (s3d::MouseL.pressed())
		{
			/* 位置移動 */
			s3d::Point mouseCurrentPos = s3d::Cursor::Pos();
			s3d::Point dif = mouseStartPos - mouseCurrentPos;

			sS3dSpinePlayer.MoveViewPoint(dif.x, dif.y);
			mouseStartPos = mouseCurrentPos;
		}
		else if (s3d::MouseL.up() && s3d::MouseR.pressed())
		{
			/* 動作移行 */
			sS3dSpinePlayer.ShiftAnimation();
		}
		else if (s3d::MouseM.up())
		{
			/* 位置・速度・尺度初期化 */
			sS3dSpinePlayer.ResetScale();
		}

		if (s3d::Mouse::Wheel())
		{
			if (s3d::MouseL.pressed())
			{
				/* 加減速 */
				sS3dSpinePlayer.RescaleTime(s3d::Mouse::Wheel() > 0);
			}
			else if(!s3d::MouseR.pressed())
			{
				/* 拡縮 */
				sS3dSpinePlayer.RescaleSkeleton(s3d::Mouse::Wheel() > 0);
			}
		}

		sS3dSpinePlayer.Redraw(static_cast<float>(s3d::Scene::DeltaTime()));
	}
}
