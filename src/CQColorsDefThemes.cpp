#include <CQColorsDefThemes.h>
#include <CQColorsTheme.h>
#include <CQColors.h>

class CQColorsThemeDefault : public CQColorsTheme {
 public:
  CQColorsThemeDefault() {
    setDesc("Default");

    //---

    addNamedPalettes();

    moveNamedPalette("set2"    , 0);
    moveNamedPalette("palette1", 1);
    moveNamedPalette("palette2", 2);
  }
};

class CQColorsTheme1 : public CQColorsTheme {
 public:
  CQColorsTheme1() {
    setDesc("Theme 1");

    //---

    addNamedPalettes();

    moveNamedPalette("palette1", 0);
    moveNamedPalette("palette2", 1);
    moveNamedPalette("set2"    , 2);
  }
};

class CQColorsTheme2 : public CQColorsTheme {
 public:
  CQColorsTheme2() {
    setDesc("Theme 2");

    //---

    addNamedPalettes();

    moveNamedPalette("palette2", 0);
    moveNamedPalette("palette1", 1);
    moveNamedPalette("set2"    , 2);
  }
};

//------

void
CQColorsDefThemes::
addThemes(CQColorsMgr *mgr)
{
  mgr->addNamedTheme("default", new CQColorsThemeDefault);
  mgr->addNamedTheme("theme1" , new CQColorsTheme1);
  mgr->addNamedTheme("theme2" , new CQColorsTheme2);
}
