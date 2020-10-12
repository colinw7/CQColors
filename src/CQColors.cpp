#include <CQColors.h>
#include <CQColorsTheme.h>
#include <CQColorsPalette.h>
#include <CQColorsDefPalettes.h>
#include <CQColorsDefThemes.h>

CQColorsMgr *
CQColorsMgr::
instance()
{
  static CQColorsMgr *inst;

  if (! inst) {
    inst = new CQColorsMgr;

    inst->init();
  }

  return inst;
}

CQColorsMgr::
CQColorsMgr()
{
}

CQColorsMgr::
~CQColorsMgr()
{
  for (auto &namedPalette : namedPalettes_) {
    PaletteData &paletteData = namedPalette.second;

    delete paletteData.original;
    delete paletteData.current;
  }

  for (auto &nameTheme : themes_)
    delete nameTheme.second;
}

void
CQColorsMgr::
init()
{
  CQColorsDefPalettes::addPalettes(this);

  CQColorsDefThemes::addThemes(this);
}

CQColorsPalette *
CQColorsMgr::
createPalette(const QString &name)
{
  assert(! getNamedPalette(name));

  auto *palette = new CQColorsPalette;

  addNamedPalette(name, palette);

  return palette;
}

void
CQColorsMgr::
addNamedPalette(const QString &name, CQColorsPalette *palette)
{
  auto p = namedPalettes_.find(name);
  assert(p == namedPalettes_.end());

  palette->setName(name);

  PaletteData paletteData;

  paletteData.ind      = namedPalettes_.size();
  paletteData.current  = palette;
  paletteData.original = palette->dup();

  namedPalettes_[name] = paletteData;

  connect(palette, SIGNAL(colorsChanged()), this, SLOT(paletteChangedSlot()));

  emit palettesChanged();
}

CQColorsPalette *
CQColorsMgr::
getNamedPalette(const QString &name) const
{
  auto p = namedPalettes_.find(name);

  if (p == namedPalettes_.end())
    return nullptr;

  const PaletteData &paletteData = (*p).second;

  return paletteData.current;
}

int
CQColorsMgr::
getNamedPaletteInd(const QString &name) const
{
  auto p = namedPalettes_.find(name);

  if (p == namedPalettes_.end())
    return -1;

  const PaletteData &paletteData = (*p).second;

  return paletteData.ind;
}

CQColorsPalette *
CQColorsMgr::
getIndPalette(int ind) const
{
  if (ind < 0) return nullptr;

  auto p = namedPalettes_.begin();

  std::advance(p, ind);

  if (p == namedPalettes_.end())
    return nullptr;

  const PaletteData &paletteData = (*p).second;

  return paletteData.current;
}

void
CQColorsMgr::
getPaletteNames(QStringList &names) const
{
  for (const auto &p : namedPalettes_)
    names.push_back(p.first);
}

void
CQColorsMgr::
resetPalette(const QString &name)
{
  auto p = namedPalettes_.find(name);
  assert(p != namedPalettes_.end());

  PaletteData &paletteData = (*p).second;

  paletteData.current->assign(*paletteData.original);
}

//---

CQColorsTheme *
CQColorsMgr::
createTheme(const QString &name)
{
  assert(! getNamedTheme(name));

  auto *theme = new CQColorsTheme;

  theme->setDesc(name);

  addNamedTheme(name, theme);

  return theme;
}

void
CQColorsMgr::
addNamedTheme(const QString &name, CQColorsTheme *theme)
{
  auto p = themes_.find(name);
  assert(p == themes_.end());

  theme->setName(name);

  themes_[name] = theme;

  connect(theme, SIGNAL(themeChanged()), this, SLOT(themeChangedSlot()));

  emit themesChanged();
}

CQColorsTheme *
CQColorsMgr::
getNamedTheme(const QString &name) const
{
  auto p = themes_.find(name);

  if (p == themes_.end())
    return nullptr;

  return (*p).second;
}

void
CQColorsMgr::
getThemeNames(QStringList &names) const
{
  for (const auto &p : themes_)
    names.push_back(p.first);
}

void
CQColorsMgr::
themeChangedSlot()
{
  auto *theme = qobject_cast<CQColorsTheme *>(sender());

  if (theme)
    emit themeChanged(theme->name());

  emit themesChanged();
}

void
CQColorsMgr::
paletteChangedSlot()
{
  auto *palette = qobject_cast<CQColorsPalette *>(sender());

  if (palette)
    emit paletteChanged(palette->name());

  emit palettesChanged();
}
