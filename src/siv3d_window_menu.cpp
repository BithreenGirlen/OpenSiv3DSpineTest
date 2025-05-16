

#include "siv3d_window_menu.h"

void CSiv3dWindowMenu::Initialise(
	s3d::Array<std::pair<s3d::String, s3d::Array<s3d::String>>> menuItems,
	s3d::Array<s3d::Array<std::function<void()>>> menuCallbacks)
{
	m_menuItems = menuItems;
	m_menuCallbacks = menuCallbacks;
	m_pMenuBar = std::make_unique<s3d::SimpleMenuBar>(m_menuItems);
}

bool CSiv3dWindowMenu::HasBeenInitialised() const
{
	return !m_menuItems.empty() && !m_menuCallbacks.empty() && m_pMenuBar.get() != nullptr;
}

void CSiv3dWindowMenu::SetVisibility(bool isVisible)
{
	m_isMenuBarVisible = isVisible;
}

bool CSiv3dWindowMenu::IsVisible() const
{
	return m_isMenuBarVisible;
}

void CSiv3dWindowMenu::Update()
{
	if (m_pMenuBar.get() == nullptr)return;

	if (const auto& menuBarItem = m_pMenuBar->update())
	{
		for (size_t menuIndex = 0; menuIndex < m_menuItems.size(); ++menuIndex)
		{
			const auto& items = m_menuItems[menuIndex].second;
			for (size_t itemIndex = 0; itemIndex < items.size(); ++itemIndex)
			{
				if (menuBarItem == s3d::MenuBarItemIndex{ menuIndex, itemIndex })
				{
					if (menuIndex < m_menuCallbacks.size() && itemIndex < m_menuCallbacks[menuIndex].size())
					{
						if (m_menuCallbacks[menuIndex][itemIndex] != nullptr)
						{
							m_menuCallbacks[menuIndex][itemIndex]();
						}
					}
				}
			}
		}
	}
}

void CSiv3dWindowMenu::Draw()
{
	if (m_pMenuBar.get() != nullptr && m_isMenuBarVisible)
	{
		m_pMenuBar->draw();
	}
}
