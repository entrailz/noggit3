// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <string>

#include <noggit/ui/Button.h>
#include <noggit/ui/Window.h>
#include <noggit/ui/CloseWindow.h>

class UIMapViewGUI;
class UIListView;

class UIZoneIDBrowser : public UIWindow
{
public:
	typedef UIZoneIDBrowser* Ptr;

private:
	void(*changeFunc)(UIFrame *, int);
	UIMapViewGUI *mainGui;
	UIListView* ZoneIdList;
	int mapID;
	unsigned int zoneID;
	int subZoneID;
	int selectedAreaID;
	std::string MapName;
	std::string ZoneName;
	std::string SubZoneName;
	UIButton* backZone;
	UIButton* closeBrowser;
	UIText* ZoneIDPath;

	void buildAreaList();
	void expandList();
	void collapseList();

public:
	UIZoneIDBrowser(int xPos, int yPos, int w, int h, UIMapViewGUI *setGui);
	void setMapID(int id);
	void setZoneID(int id);
	void ButtonMapPressed(int id);
	void refreshMapPath();
	void setChangeFunc(void(*f)(UIFrame *, int));
};
