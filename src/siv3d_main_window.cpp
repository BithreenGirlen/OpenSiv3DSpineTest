
#include "siv3d_main_window.h"


CSiv3dMainWindow::CSiv3dMainWindow(const char32_t* windowName)
{
	if (windowName != nullptr)
	{
		s3d::Window::SetTitle(windowName);
		s3d::Window::SetStyle(s3d::WindowStyle::Sizable);

		s3d::Scene::SetResizeMode(s3d::ResizeMode::Actual);
	}

	InitialiseMenuBar();
}

CSiv3dMainWindow::~CSiv3dMainWindow()
{

}

void CSiv3dMainWindow::Display()
{
	while (s3d::System::Update())
	{
		m_siv3dWindowMenu.Update();

		if (s3d::MouseL.pressed())
		{
			if (s3d::KeyLControl.pressed() && s3d::Window::GetStyle() == s3d::WindowStyle::Frameless)
			{
				/* 枠無しウィンドウ移動 */
				s3d::Point mouseDelta = s3d::Cursor::ScreenDelta();
				s3d::Rect windowRect = s3d::Window::GetState().bounds;

				s3d::Point windowPosToBe = s3d::Point{ windowRect.x + mouseDelta.x, windowRect.y + mouseDelta.y };
				s3d::Window::SetPos(windowPosToBe);
			}
			else
			{
				/* 視点移動 */
				s3d::Point mouseDelta = s3d::Cursor::Delta();
				m_siv3dSpinePlayer.MoveViewPoint(-mouseDelta.x, -mouseDelta.y);
			}
		}
		else if (s3d::MouseL.up() && s3d::MouseR.pressed())
		{
			/* 動作移行 */
			m_siv3dSpinePlayer.ShiftAnimation();
		}
		else if (s3d::MouseM.up())
		{
			if (s3d::MouseR.pressed())
			{
				/* 枠表示・消去 */
				bool hasFrame = s3d::Window::GetStyle() != s3d::WindowStyle::Frameless;
				if (!hasFrame)
				{
					s3d::Rect windowRect = s3d::Window::GetState().bounds;
					windowRect.y = s3d::Max(0, windowRect.y);

					s3d::Window::SetPos({ windowRect.x, windowRect.y });
				}
				else
				{
					s3d::Window::SetPos({});
				}

				s3d::Window::SetStyle(hasFrame ? s3d::WindowStyle::Frameless : s3d::WindowStyle::Sizable);
			}
			else
			{
				/* 視点・速度・尺度初期化 */
				m_siv3dSpinePlayer.ResetScale();
				ResizeWindow();
			}

		}

		if (s3d::Mouse::Wheel())
		{
			if (s3d::MouseL.pressed())
			{
				/* 加減速 */
				m_siv3dSpinePlayer.RescaleTime(s3d::Mouse::Wheel() > 0);
			}
			else if (!s3d::MouseR.pressed())
			{
				/* 拡縮 */
				m_siv3dSpinePlayer.RescaleSkeleton(s3d::Mouse::Wheel() > 0);
				if (!s3d::KeyLControl.pressed())
				{
					m_siv3dSpinePlayer.RescaleCanvas(s3d::Mouse::Wheel() > 0);
					ResizeWindow();
				}
			}
		}

		if (s3d::KeyM.up())
		{
			/* メニュー消去・表示。 */
			m_siv3dWindowMenu.SetVisibility(!m_siv3dWindowMenu.IsVisible());
			ResizeWindow();
		}

		m_siv3dSpinePlayer.Update(static_cast<float>(s3d::Scene::DeltaTime()));

		if (m_pSpinePlayerTexture.get() != nullptr)
		{
			m_pSpinePlayerTexture->clear(s3d::ColorF(0.f, 0.f));
			{
				const s3d::ScopedRenderTarget2D spinePlayerRenderTarget(*m_pSpinePlayerTexture.get());
				m_siv3dSpinePlayer.Redraw();
			}

			s3d::int32 menuBarHeight = m_siv3dWindowMenu.IsVisible() ? s3d::SimpleMenuBar::MenuBarHeight : 0;
			m_pSpinePlayerTexture->draw(0, menuBarHeight);
		}
		m_siv3dWindowMenu.Draw();
	}
}

void CSiv3dMainWindow::MenuOnOpenFile()
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

	m_siv3dSpinePlayer.LoadSpineFromFile(atlasPaths, skelPaths, isBinarySkel);

	ResizeWindow();
}

void CSiv3dMainWindow::MenuOnSnapImage()
{
	if (m_pSpinePlayerTexture.get() != nullptr)
	{
		s3d::Image image;
		m_pSpinePlayerTexture->readAsImage(image);
		s3d::String fileName = s3d::Unicode::FromUTF8(m_siv3dSpinePlayer.GetCurrentAnimationNameWithTrackTime());

		s3d::FilePath filePath = s3d::FileSystem::ParentPath(s3d::FileSystem::ModulePath()) + fileName + U".webp";
		image.saveWebP(filePath);
	}
}

void CSiv3dMainWindow::ResizeWindow()
{
	s3d::Vector2D<float> fCanvasSize = m_siv3dSpinePlayer.GetBaseSize();
	float fScale = m_siv3dSpinePlayer.GetCanvasScale();

	s3d::int32 iClientWidth = static_cast<s3d::int32>(fCanvasSize.x * fScale);
	s3d::int32 iClientHeight = static_cast<s3d::int32>(fCanvasSize.y * fScale);

	/*
	* 各画面上限
	* Scene (Virtual mode): なし
	* Scene (Actual mode): デスクトップ解像度 + 枠幅
	* Frame: デスクトップ解像度 + 枠幅
	* Virtual: デスクトップ解像度 / DPI
	*/

	/* メニュー表示時はクライアント領域を高さ分大きく取り、且つ、他の描画対象物の描画開始位置を高さ分下げる。*/
	s3d::int32 menuBarHeight = m_siv3dWindowMenu.IsVisible() ? s3d::SimpleMenuBar::MenuBarHeight : 0;
	s3d::Window::ResizeActual(iClientWidth, iClientHeight + menuBarHeight, s3d::YesNo<s3d::Centering_tag>::No);
	
	s3d::Size spinePlayerSize = s3d::Size(iClientWidth, iClientHeight);
	m_pSpinePlayerTexture = std::make_unique<s3d::RenderTexture>(spinePlayerSize);
	m_siv3dSpinePlayer.OnResize(spinePlayerSize);
}

void CSiv3dMainWindow::InitialiseMenuBar()
{
	if (!m_siv3dWindowMenu.HasBeenInitialised())
	{
		const std::pair<s3d::String, s3d::Array<s3d::String>> fileMenu
		{
			U"File", { U"Open file"}
		};
		const s3d::Array<std::function<void()>> fileMenuCallbacks
		{
			std::bind(&CSiv3dMainWindow::MenuOnOpenFile, this)
		};

		const std::pair<s3d::String, s3d::Array<s3d::String>> imageMenu
		{
			U"Image", { U"Snap as webp"}
		};
		const s3d::Array<std::function<void()>> imageMenuCallbacks
		{
			std::bind(&CSiv3dMainWindow::MenuOnSnapImage, this)
		};

		const s3d::Array<std::pair<s3d::String, s3d::Array<s3d::String>>> menuItems
		{
			fileMenu, imageMenu
		};

		const s3d::Array<s3d::Array<std::function<void()>>> menuCallbacks
		{
			fileMenuCallbacks, imageMenuCallbacks
		};

		m_siv3dWindowMenu.Initialise(menuItems, menuCallbacks);
	}
}
