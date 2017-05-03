// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/CursorSwitcher.h>
#include <util/qt/overload.hpp>

#include <qt-color-widgets/color_selector.hpp>

#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

namespace noggit
{
  namespace ui
  {
    cursor_switcher::cursor_switcher(QWidget* parent, math::vector_4d& color, noggit::unsigned_int_property& cursor_type)
      : widget (parent)
    {
      setWindowTitle("Cursor Options");
      setWindowFlags(Qt::Tool);
      new QVBoxLayout (this);

      auto butt_disk (new QRadioButton ("Disk", this));
      auto butt_sphere (new QRadioButton ("Sphere", this));
      auto butt_triangle (new QRadioButton ("Triangle", this));
      auto butt_terrain_disk (new QRadioButton ("Terrain Disk", this));
      auto butt_none (new QRadioButton ("None", this));

      this->layout()->addWidget (butt_disk);
      this->layout()->addWidget (butt_sphere);
      this->layout()->addWidget (butt_triangle);
      this->layout()->addWidget (butt_terrain_disk);
      this->layout()->addWidget (butt_none);

      auto group (new QButtonGroup (this));

      group->addButton (butt_disk, 1);
      group->addButton (butt_sphere, 2);
      group->addButton (butt_triangle, 3);
      group->addButton (butt_terrain_disk, 4);
      group->addButton (butt_none, 0);

      group->button (cursor_type.get())->setChecked (true);

      connect ( group
              , qOverload<int> (&QButtonGroup::buttonClicked)
               , [&] (int id)
                 {
                   QSignalBlocker const blocker(&cursor_type);
                   cursor_type.set(id);
                 }
              );

      connect ( &cursor_type
              , &noggit::unsigned_int_property::changed
              , [group] (unsigned int id)
                {
                  QSignalBlocker const blocker(group);
                  group->button(id)->setChecked (true);
                }
              );

      auto color_picker (new color_widgets::ColorSelector (this));

      color_picker->setDisplayMode (color_widgets::ColorSelector::AllAlpha);

      color_picker->setColor (QColor::fromRgbF (color.x, color.y, color.z, color.w));

      connect ( color_picker, &color_widgets::ColorSelector::colorChanged
              , [&] (QColor new_color)
                {
                  color.x = new_color.redF();
                  color.y = new_color.greenF();
                  color.z = new_color.blueF();
                  color.w = new_color.alphaF();
                }
              );


      this->layout()->addWidget (color_picker);
    }
  }
}
