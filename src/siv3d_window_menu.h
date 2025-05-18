#ifndef SIV3D_WINDOW_MENU_H_
#define SIV3D_WINDOW_MENU_H_

#ifndef NO_S3D_USING
#define NO_S3D_USING
#endif
#include <Siv3D.hpp>

class CSiv3dWindowMenu
{
public:

	void Initialise(
		s3d::Array<std::pair<s3d::String, s3d::Array<s3d::String>>> menuItems,
		s3d::Array<s3d::Array<std::function<void()>>> menuCallbacks);
	bool HasBeenInitialised() const;

	void SetVisibility(bool isVisible);
	bool IsVisible()const;

	void Update();
	void Draw();

	bool GetLastItemChecked() const;
	void SetLastItemChecked(bool checked);
private:
	std::unique_ptr<s3d::SimpleMenuBar> m_pMenuBar;
	s3d::Array<std::pair<s3d::String, s3d::Array<s3d::String>>> m_menuItems;
	s3d::Array<s3d::Array<std::function<void()>>> m_menuCallbacks;

	bool m_isMenuBarVisible = true;
	s3d::MenuBarItemIndex m_lastItemIndex;
};

#endif // !SIV3D_WINDOW_MENU_H_
