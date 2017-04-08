// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <noggit/MapChunk.h>
#include <noggit/Selection.h>
#include <noggit/ui/widget.hpp>

#include <vector>

namespace noggit
{
  namespace ui
  {
    class current_texture;

    class texture_picker : public widget
    {
      Q_OBJECT

    public:
      texture_picker (current_texture*);

      void getTextures(selection_type lSelection);
      void setTexture(size_t id, current_texture*);
      void shiftSelectedTextureLeft();
      void shiftSelectedTextureRight();

    private:
      void update(bool set_changed = true);

      std::vector<current_texture*> _labels;
      std::vector<scoped_blp_texture_reference> _textures;
      MapChunk* _chunk;

    signals:
      void shift_left();
      void shift_right();
    };
  }
}
