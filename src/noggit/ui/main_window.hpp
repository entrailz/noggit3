#pragma once

#include <math/vector_3d.hpp>
#include <math/trig.hpp>
#include <noggit/World.h>
#include <noggit/ui/uid_fix_window.hpp>

#include <QtWidgets/QMainWindow>

#include <string>

namespace noggit
{
  namespace ui
  {
    class minimap_widget;

    struct main_window : QMainWindow
    {
      main_window();

      void prompt_exit();

    private:
      void loadMap (int mapID);
      void enterMapAt ( math::vector_3d pos
                      , math::degrees camera_pitch
                      , math::degrees camera_yaw
                      , uid_fix_mode uid_fix = uid_fix_mode::none
                      );

      void createBookmarkList();
      void build_menu();

      struct MapEntry
      {
        int mapID;
        std::string name;
        int areaType;
      };

      struct BookmarkEntry
      {
        int mapID;
        std::string name;
        math::vector_3d pos;
        float camera_yaw;
        float camera_pitch;
      };

      std::vector<MapEntry> mMaps;
      std::vector<BookmarkEntry> mBookmarks;

      minimap_widget* _minimap;
      QWidget* _null_widget;

      std::unique_ptr<World> _world;

      virtual void closeEvent (QCloseEvent*) override;
    };
  }
}
