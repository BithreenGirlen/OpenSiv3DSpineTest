
#include "siv3d_main_window.h"

#include "siv3d_spine_player.h"

/* ウィンドウサイズ変更時の追従がうまくいかないので一先ず保留。 */
// #define RESIZE_ACUTUAL_TEST


CSiv3dMainWindow::CSiv3dMainWindow(const char32_t* windowName)
{
	if (windowName != nullptr)
	{
		s3d::Window::SetTitle(windowName);
		s3d::Window::SetStyle(s3d::WindowStyle::Sizable);
#ifdef RESIZE_ACUTUAL_TEST
		s3d::Scene::SetResizeMode(s3d::ResizeMode::Actual);
#endif
	}
}

CSiv3dMainWindow::~CSiv3dMainWindow()
{

}

void CSiv3dMainWindow::Display()
{
	auto selectedAtlas = s3d::Dialog::OpenFile({ { U"Atlas file", { U"atlas" , U"atlas.txt" } } }, U"", U"Select atlas");
	if (!selectedAtlas.has_value())return;

	const s3d::Array<s3d::String> skelCandidates = { U"skel", U"bin", U"json", U"txt" };
	auto selectedSkeleton = s3d::Dialog::OpenFile({ { U"Skeleton file", skelCandidates } }, U"", U"Select skeleton");
	if (!selectedSkeleton.has_value())return;

	const auto IsBinarySkeleton = [&](const s3d::String& str)
		-> bool
		{
			const s3d::Array<s3d::String> binaryCandidates = { U"skel", U"bin" };
			for (const auto& binaryCandidate : binaryCandidates)
			{
				if (str.contains(binaryCandidate))return true;
			}
			return false;
		};
	bool isBinarySkel = IsBinarySkeleton(s3d::FileSystem::FileName(selectedSkeleton.value()));

	std::vector<std::string> atlasPaths;
	std::vector<std::string> skelPaths;

	atlasPaths.push_back(s3d::Unicode::ToUTF8(selectedAtlas.value()));
	skelPaths.push_back(s3d::Unicode::ToUTF8(selectedSkeleton.value()));

	CSiv3dSpinePlayer sS3dSpinePlayer;
	sS3dSpinePlayer.SetSpineFromFile(atlasPaths, skelPaths, isBinarySkel);
	
	const auto ResizeWindow = [&]()
		-> void
		{
			s3d::Vector2D<float> fCanvasSize = sS3dSpinePlayer.GetBaseSize();
			float fScale = sS3dSpinePlayer.GetCanvasScale();

			s3d::int32 iClientWidth = static_cast<s3d::int32>(fCanvasSize.x * fScale);
			s3d::int32 iClientHeight = static_cast<s3d::int32>(fCanvasSize.y * fScale);

			/*
			* 各画面上限
			* Scene (Virtual mode): なし
			* Scene (Actual mode): デスクトップ解像度 + 枠幅
			* Frame: デスクトップ解像度 + 枠幅
			* Virtual: デスクトップ解像度 / DPI
			*/

#ifdef RESIZE_ACUTUAL_TEST
			s3d::Window::ResizeActual(iClientWidth, iClientHeight, s3d::YesNo<s3d::Centering_tag>::No);
#else
			s3d::Window::Resize(iClientWidth, iClientHeight, s3d::YesNo<s3d::Centering_tag>::No);
#endif
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
#ifdef RESIZE_ACUTUAL_TEST
			ResizeWindow();
#endif
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
#ifdef RESIZE_ACUTUAL_TEST
				if (s3d::KeyLControl.pressed())
				{
					sS3dSpinePlayer.RescaleCanvas(s3d::Mouse::Wheel() > 0);
					ResizeWindow();
				}
#endif
			}
		}

		sS3dSpinePlayer.Redraw(static_cast<float>(s3d::Scene::DeltaTime()));
	}
}
