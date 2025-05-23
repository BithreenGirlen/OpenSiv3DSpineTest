
#include "siv3d_main_window.h"

#include "siv3d_spine_blendmode.h"


CSiv3dMainWindow::CSiv3dMainWindow(const char32_t* windowName)
{
	if (windowName != nullptr)
	{
		s3d::Window::SetTitle(windowName);
	}

	/*
	* 各画面上限
	* Scene (Virtual mode): なし
	* Scene (Actual mode): デスクトップ解像度 + 枠幅
	* Frame: デスクトップ解像度 + 枠幅
	* Virtual: デスクトップ解像度 / DPI
	*/
	s3d::Scene::SetResizeMode(s3d::ResizeMode::Actual);

	InitialiseMenuBar();
}

CSiv3dMainWindow::~CSiv3dMainWindow()
{

}

void CSiv3dMainWindow::Display()
{
	constexpr s3d::int32 kTrackFontSize = 20;
	const s3d::Font trackFont{ kTrackFontSize };

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
				if (s3d::Window::GetStyle() != s3d::WindowStyle::Frameless)
				{
					s3d::Window::SetPos({});
					s3d::Window::SetStyle(s3d::WindowStyle::Frameless);
				}
				else
				{
					s3d::Rect windowRect = s3d::Window::GetState().bounds;
					s3d::Window::SetPos({ windowRect.x, s3d::Max(0, windowRect.y) });
					s3d::Window::SetStyle(s3d::WindowStyle::Fixed);
				}
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
		else if (s3d::KeyA.up())
		{
			m_siv3dSpinePlayer.TogglePma();
		}

		m_siv3dSpinePlayer.Update(static_cast<float>(s3d::Scene::DeltaTime()));

		s3d::int32 menuBarHeight = m_siv3dWindowMenu.IsVisible() ? s3d::SimpleMenuBar::MenuBarHeight : 0;
		if (m_pSpinePlayerTexture.get() != nullptr)
		{
			m_pSpinePlayerTexture->clear(s3d::ColorF(0.f, 0.f));
			{
				const s3d::ScopedRenderTarget2D spinePlayerRenderTarget(*m_pSpinePlayerTexture.get());
				m_siv3dSpinePlayer.Redraw();
			}

			m_pSpinePlayerTexture->draw(0, menuBarHeight);

			if (m_siv3dRecorder.IsUnderRecording())
			{
				s3d::Vector4D<float> animationWatch{};
				m_siv3dSpinePlayer.GetCurrentAnimationTime(&animationWatch.x, &animationWatch.y, &animationWatch.z, &animationWatch.w);
				/* 一周し終わったら書き出し。 */
				if (animationWatch.x > animationWatch.w)
				{
					const char* pzAnimationName = m_siv3dSpinePlayer.GetCurrentAnimationName();
					s3d::String fileName = pzAnimationName == nullptr ? U"output" : s3d::Unicode::FromUTF8(pzAnimationName);
					s3d::FilePath filePath = s3d::FileSystem::ParentPath(s3d::FileSystem::ModulePath()) + fileName;

					m_siv3dRecorder.End(filePath);
				}
				else
				{
					s3d::Graphics2D::Flush();
					m_siv3dRecorder.CommitFrame(*m_pSpinePlayerTexture.get());
				}
			}
		}

		if (m_pSpineTrackTexture.get() != nullptr && !m_isSpineTrackHidden)
		{
			const char* pzAnimationName = m_siv3dSpinePlayer.GetCurrentAnimationName();
			if (pzAnimationName != nullptr)
			{
				m_pSpineTrackTexture->clear(s3d::ColorF(0.f, 0.f));
				{
					const s3d::ScopedRenderTarget2D spinePlayerRenderTarget(*m_pSpineTrackTexture.get());
					const s3d::ScopedRenderStates2D s3dScopedRenderState2D(Siv3dSpineBlendMode::Normal, s3d::SamplerState::ClampLinear);

					/* 毎ループ実行すまじき処理だが、取り敢えず試験用で。 */
					s3d::String animationName = s3d::Unicode::FromUTF8(pzAnimationName);
					s3d::Vector4D<float> animationWatch{};
					m_siv3dSpinePlayer.GetCurrentAnimationTime(&animationWatch.x, &animationWatch.y, &animationWatch.z, &animationWatch.w);

					{
						using namespace s3d;
						const auto& formatted = U"Track: {:.2f}\nLast: {:.2f}"_fmt(animationWatch.x, animationWatch.y);
						trackFont(formatted).draw(s3d::Vec2{ 0, 0 }, s3d::ColorF{ 1.f });
					}

					double trackValue = animationWatch.y;
					double trackMin = animationWatch.z;
					double trackMax = animationWatch.w;
					s3d::SimpleGUI::Slider(animationName, trackValue, trackMin, trackMax, s3d::Vec2{ 0, kTrackFontSize * 3.2 }, animationName.size() * 12.0);
				}

				m_pSpineTrackTexture->draw(0, menuBarHeight);
			}
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
		s3d::String fileName = s3d::Unicode::FromUTF8(m_siv3dSpinePlayer.GetCurrentAnimationName());

		s3d::FilePath filePath = s3d::FileSystem::ParentPath(s3d::FileSystem::ModulePath()) + fileName + U".webp";
		image.saveWebP(filePath);
	}
}

void CSiv3dMainWindow::MenuOnExportAsGif()
{
	if (m_pSpinePlayerTexture.get() != nullptr)
	{
		m_siv3dSpinePlayer.RestartAnimation();
		m_siv3dRecorder.Start(s3d::Size(m_pSpinePlayerTexture->width(), m_pSpinePlayerTexture->height()), CSiv3dRecorder::EOutputType::Gif);
	}
}

void CSiv3dMainWindow::MenuOnExportAsVideo()
{
	if (m_pSpinePlayerTexture.get() != nullptr)
	{
		m_siv3dSpinePlayer.RestartAnimation();
		m_siv3dRecorder.Start(s3d::Size(m_pSpinePlayerTexture->width(), m_pSpinePlayerTexture->height()), CSiv3dRecorder::EOutputType::Video);
	}
}

void CSiv3dMainWindow::MenuOnHideTrack()
{
	bool checked = m_siv3dWindowMenu.GetLastItemChecked();
	m_siv3dWindowMenu.SetLastItemChecked(!checked);
	m_isSpineTrackHidden = !checked;
}

void CSiv3dMainWindow::ResizeWindow()
{
	if (!m_siv3dSpinePlayer.HasSpineBeenLoaded() || m_siv3dRecorder.IsUnderRecording())return;

	s3d::Vector2D<float> fCanvasSize = m_siv3dSpinePlayer.GetBaseSize();
	float fScale = m_siv3dSpinePlayer.GetCanvasScale();

	s3d::int32 iClientWidth = static_cast<s3d::int32>(fCanvasSize.x * fScale);
	s3d::int32 iClientHeight = static_cast<s3d::int32>(fCanvasSize.y * fScale);

	/* メニュー表示時はクライアント領域を高さ分大きく取り、且つ、他の描画対象物の描画開始位置を高さ分下げる。*/
	s3d::int32 menuBarHeight = m_siv3dWindowMenu.IsVisible() ? s3d::SimpleMenuBar::MenuBarHeight : 0;

	/* 書き出しファイルの寸法を解像度内に抑えるため上限を設ける。 */
	const auto monitorInfo = s3d::System::GetCurrentMonitor();
	iClientWidth = s3d::Min(iClientWidth, monitorInfo.displayRect.w);
	iClientHeight = s3d::Min(iClientHeight, monitorInfo.displayRect.h);

	s3d::Window::ResizeActual(iClientWidth, iClientHeight + menuBarHeight, s3d::YesNo<s3d::Centering_tag>::No);

	/* Spine描画先 */
	s3d::Size spinePlayerSize = s3d::Size(iClientWidth, iClientHeight);
	m_pSpinePlayerTexture = std::make_unique<s3d::RenderTexture>(spinePlayerSize);
	m_siv3dSpinePlayer.OnResize(spinePlayerSize);

	/* Spine情報表示先 */
	s3d::Size spineTrackSize = s3d::Size(iClientWidth * 3 / 10, iClientHeight * 3 / 10);
	m_pSpineTrackTexture = std::make_unique<s3d::RenderTexture>(spineTrackSize);
}

void CSiv3dMainWindow::InitialiseMenuBar()
{
	if (!m_siv3dWindowMenu.HasBeenInitialised())
	{
		/* File群 */
		const std::pair<s3d::String, s3d::Array<s3d::String>> fileMenu
		{
			U"File", { U"Open file"}
		};
		const s3d::Array<std::function<void()>> fileMenuCallbacks
		{
			std::bind(&CSiv3dMainWindow::MenuOnOpenFile, this)
		};
		/* Image群 */
		const std::pair<s3d::String, s3d::Array<s3d::String>> imageMenu
		{
			U"Image", { U"Snap as Webp", /*U"Export as GIF",*/ U"Export as video"}
		};
		const s3d::Array<std::function<void()>> imageMenuCallbacks
		{
			std::bind(&CSiv3dMainWindow::MenuOnSnapImage, this),
			//std::bind(&CSiv3dMainWindow::MenuOnExportAsGif, this),
			std::bind(&CSiv3dMainWindow::MenuOnExportAsVideo, this)
		};
		/* Window群 */
		const std::pair<s3d::String, s3d::Array<s3d::String>> windowMenu
		{
			U"Window", { U"Hide track"}
		};
		const s3d::Array<std::function<void()>> windowMenuCallbacks
		{
			std::bind(&CSiv3dMainWindow::MenuOnHideTrack, this)
		};

		const s3d::Array<std::pair<s3d::String, s3d::Array<s3d::String>>> menuItems
		{
			fileMenu, imageMenu, windowMenu
		};
		const s3d::Array<s3d::Array<std::function<void()>>> menuCallbacks
		{
			fileMenuCallbacks, imageMenuCallbacks, windowMenuCallbacks
		};

		m_siv3dWindowMenu.Initialise(menuItems, menuCallbacks);
	}
}
