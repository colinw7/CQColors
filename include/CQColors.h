#ifndef CQColors_H
#define CQColors_H

#include <COrderedMap.h>
#include <QObject>
#include <QColor>
#include <QStringList>
#include <map>
#include <vector>

class CQColorsTheme;
class CQColorsPalette;

#define CQColorsMgrInst CQColorsMgr::instance()

//! \brief manager class for named palettes and named themes
class CQColorsMgr : public QObject {
  Q_OBJECT

 public:
  static CQColorsMgr *instance();

 ~CQColorsMgr();

  //! named palettes
  CQColorsPalette *createPalette(const QString &name);

  void addNamedPalette(const QString &name, CQColorsPalette *palette);

  CQColorsPalette *getNamedPalette(const QString &name) const;
  int getNamedPaletteInd(const QString &name) const;
  CQColorsPalette *getIndPalette(int ind) const;

  void getPaletteNames(QStringList &names) const;

  void resetPalette(const QString &name);

  //---

  //! named themes
  CQColorsTheme *createTheme(const QString &name);

  void addNamedTheme(const QString &name, CQColorsTheme *theme);

  CQColorsTheme *getNamedTheme(const QString &name) const;

  void getThemeNames(QStringList &names) const;

 signals:
  // sent when manager's themes changed (added or content changed)
  void themesChanged();

  // sent when manager's palettes changed (added or content changed)
  void palettesChanged();

  // sent when managed theme changed
  void themeChanged(const QString &name);

  // sent when managed palette changed
  void paletteChanged(const QString &name);

 private:
  CQColorsMgr();

  void init();

 private slots:
  void themeChangedSlot();

  void paletteChangedSlot();

 private:
  struct PaletteData {
    int              ind      { -1 };
    CQColorsPalette *original { nullptr };
    CQColorsPalette *current  { nullptr };
  };

//using ThemeMap      = std::map<QString,CQColorsTheme*>;
  using ThemeMap      = COrderedMap<QString,CQColorsTheme*>;
//using NamedPalettes = std::map<QString,PaletteData>;
  using NamedPalettes = COrderedMap<QString,PaletteData>;

  NamedPalettes namedPalettes_; //!< named palettes
  ThemeMap      themes_;        //!< named themes
};

#endif
