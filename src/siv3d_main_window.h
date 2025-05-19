#ifndef SIV3D_MAIN_WINDOW_H_
#define SIV3D_MAIN_WINDOW_H_

#include "siv3d_spine_player.h"
#include "siv3d_window_menu.h"

class CSiv3dMainWindow
{
public:
	CSiv3dMainWindow(const char32_t *windowName = nullptr);
	~CSiv3dMainWindow();

	void Display();

private:
	CSiv3dSpinePlayer m_siv3dSpinePlayer;
	std::unique_ptr<s3d::RenderTexture> m_pSpinePlayerTexture;
	std::unique_ptr<s3d::RenderTexture> m_pSpineTrackTexture;

	bool m_isSpineTrackHidden = false;

	CSiv3dWindowMenu m_siv3dWindowMenu;

	void MenuOnOpenFile();

	void MenuOnSnapImage();

	void MenuOnHideTrack();

	void ResizeWindow();

	void InitialiseMenuBar();
};

#endif // !SIV3D_MAIN_WINDOW_H_
