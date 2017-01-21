// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <string>

#include <noggit/ui/Frame.h>

#include <noggit/ui/Texture.h>
#include <noggit/ui/Text.h>
#include <noggit/ui/ToggleGroup.h>

class UICheckBox : public UIFrame
{
public:
  typedef UICheckBox* Ptr;

protected:
  UITexture::Ptr check;
  UIText::Ptr text;
  bool checked;
  int id;
  void(*clickFunc)(bool, int);

  UIToggleGroup::Ptr mToggleGroup;

public:
  UICheckBox(float, float, const std::string&);
  UICheckBox(float, float, const std::string&, UIToggleGroup *, int);
  UICheckBox(float xPos, float yPos, const std::string& pText, void(*pClickFunc)(bool, int), int pClickFuncParameter);
  void SetToggleGroup(UIToggleGroup *, int);
  void setText(const std::string&);
  void setState(bool);
  bool getState();
  void setClickFunc(void(*f)(bool, int), int);

  UIFrame *processLeftClick(float, float);
};
