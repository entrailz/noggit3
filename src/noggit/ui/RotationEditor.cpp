// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/RotationEditor.h>

#include <noggit/Misc.h>
#include <noggit/ModelInstance.h>
#include <noggit/Selection.h>
#include <noggit/WMOInstance.h>
#include <util/qt/overload.hpp>

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>

namespace noggit
{
  namespace ui
  {
    rotation_editor::rotation_editor()
      : QWidget (nullptr)
      , rotationVect(nullptr)
      , posVect(nullptr)
      , scale(nullptr)
      , _selection(false)
      , _wmoInstance(nullptr)
    {
      setWindowTitle("Rotation Editor");
      setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);

      auto layout (new QFormLayout (this));

      layout->addRow (new QLabel ("Tilt", this));
      layout->addRow ("X", _rotation_x = new QDoubleSpinBox (this));
      layout->addRow ("Z", _rotation_z = new QDoubleSpinBox (this));

      layout->addRow (new QLabel ("Rotation", this));
      layout->addRow ("", _rotation_y = new QDoubleSpinBox (this));

      layout->addRow (new QLabel ("Position", this));
      layout->addRow ("X", _position_x = new QDoubleSpinBox (this));
      layout->addRow ("Z", _position_z = new QDoubleSpinBox (this));
      layout->addRow ("H", _position_y = new QDoubleSpinBox (this));

      layout->addRow (new QLabel ("Scale", this));
      layout->addRow ("", _scale = new QDoubleSpinBox (this));


      _rotation_x->setRange (-180.f, 180.f);
      _rotation_x->setDecimals (3);
      _rotation_x->setWrapping(true);
      _rotation_x->setSingleStep(5.0f);
      _rotation_z->setRange (-180.f, 180.f);
      _rotation_z->setDecimals (3);
      _rotation_z->setWrapping(true);
      _rotation_z->setSingleStep(5.0f);

      _rotation_y->setRange (0.f, 360.f);
      _rotation_y->setDecimals (3);
      _rotation_y->setWrapping(true);
      _rotation_z->setSingleStep(5.0f);

      _position_x->setRange (std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
      _position_x->setDecimals (5);
      _position_z->setRange (std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
      _position_z->setDecimals (5);
      _position_y->setRange (std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
      _position_y->setDecimals (5);

      _scale->setRange (0.01f, 63.0f);
      _scale->setDecimals (2);
      _scale->setSingleStep(0.1f);


      connect ( _rotation_x, qOverload<double> (&QDoubleSpinBox::valueChanged)
              , [&] (double v)
                {
                  rotationVect->x = v;
                  maybe_updateWMO();
                }
              );
      connect ( _rotation_z, qOverload<double> (&QDoubleSpinBox::valueChanged)
              , [&] (double v)
                {
                  rotationVect->z = v;
                  maybe_updateWMO();
                }
              );
      connect ( _rotation_y, qOverload<double> (&QDoubleSpinBox::valueChanged)
              , [&] (double v)
                {
                  rotationVect->y = v;
                  maybe_updateWMO();
                }
              );

      connect ( _position_x, qOverload<double> (&QDoubleSpinBox::valueChanged)
              , [&] (double v)
                {
                  posVect->x = v;
                  maybe_updateWMO();
                }
              );
      connect ( _position_z, qOverload<double> (&QDoubleSpinBox::valueChanged)
              , [&] (double v)
                {
                  posVect->z = v;
                  maybe_updateWMO();
                }
              );
      connect ( _position_y, qOverload<double> (&QDoubleSpinBox::valueChanged)
              , [&] (double v)
                {
                  posVect->y = v;
                  maybe_updateWMO();
                }
              );

      connect ( _scale, qOverload<double> (&QDoubleSpinBox::valueChanged)
              , [&] (double v)
                {
                  *scale = v;
                }
              );
    }

    void rotation_editor::select(selection_type entry)
    {
      _selection = true;

      if (entry.which() == eEntry_Model)
      {
        rotationVect = &(boost::get<selected_model_type> (entry)->dir);
        posVect = &(boost::get<selected_model_type> (entry)->pos);
        scale = &(boost::get<selected_model_type> (entry)->scale);
        _wmoInstance = nullptr;
      }
      else if (entry.which() == eEntry_WMO)
      {
        _wmoInstance = boost::get<selected_wmo_type> (entry);
        rotationVect = &(_wmoInstance->dir);
        posVect = &(_wmoInstance->pos);
      }
      else
      {
        _wmoInstance = nullptr;
        _selection = false;
        rotationVect = nullptr;
        posVect = nullptr;
        scale = nullptr;
      }

      updateValues();
    }

    void rotation_editor::updateValues()
    {
      if (_selection)
      {
        QSignalBlocker const block_rotation_x (_rotation_x);
        QSignalBlocker const block_rotation_y (_rotation_y);
        QSignalBlocker const block_rotation_z (_rotation_z);
        QSignalBlocker const block_position_x (_position_x);
        QSignalBlocker const block_position_y (_position_y);
        QSignalBlocker const block_position_z (_position_z);
        QSignalBlocker const block_scale (_scale);

        _rotation_x->setValue (rotationVect->x);
        _rotation_y->setValue (rotationVect->y);
        _rotation_z->setValue (rotationVect->z);
        _position_x->setValue (posVect->x);
        _position_y->setValue (posVect->y);
        _position_z->setValue (posVect->z);

        _rotation_x->setEnabled (true);
        _rotation_y->setEnabled (true);
        _rotation_z->setEnabled (true);
        _position_x->setEnabled (true);
        _position_y->setEnabled (true);
        _position_z->setEnabled (true);

        if (!_wmoInstance)
        {
          _scale->setValue (*scale);
        }

        _scale->setEnabled (!_wmoInstance);
      }
      else
      {
        _rotation_x->setEnabled (false);
        _rotation_y->setEnabled (false);
        _rotation_z->setEnabled (false);
        _position_x->setEnabled (false);
        _position_y->setEnabled (false);
        _position_z->setEnabled (false);
        _scale->setEnabled (false);
      }
    }

    void rotation_editor::maybe_updateWMO()
    {
      if (_wmoInstance)
      {
        _wmoInstance->recalcExtents();
      }
    }
  }
}
