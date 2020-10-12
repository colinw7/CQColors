#include <CQColorsTheme.h>
#include <CQColors.h>
#include <CQColorsPalette.h>

CQColorsTheme::
CQColorsTheme()
{
}

void
CQColorsTheme::
addNamedPalettes()
{
  // add 10 palettes by default
  addNamedPalette("default"  );
  addNamedPalette("palette1" );
  addNamedPalette("palette2" );
  addNamedPalette("set1"     );
  addNamedPalette("set2"     );
  addNamedPalette("set3"     );
  addNamedPalette("plasma"   );
  addNamedPalette("moreland" );
  addNamedPalette("distinct1");
  addNamedPalette("distinct2");
  addNamedPalette("red_green");
}

CQColorsTheme::
~CQColorsTheme()
{
}

CQColorsPalette *
CQColorsTheme::
palette(int i) const
{
  int i1 = i % palettes_.size();

  return palettes_[i1];
}

void
CQColorsTheme::
setPalette(int i, CQColorsPalette *palette)
{
  assert(palette);

  // validate destination position
  int n = palettes_.size();
  assert(i >= 0 && i < n);

  palettes_[i] = palette;

  emit themeChanged();
}

void
CQColorsTheme::
setNamedPalettes(const QStringList &names)
{
  palettes_.clear();

  for (int i = 0; i < names.length(); ++i) {
    auto *palette = CQColorsMgrInst->getNamedPalette(names[i]);
    assert(palette);

    palettes_.push_back(palette);
  }

  emit themeChanged();
}

void
CQColorsTheme::
addNamedPalette(const QString &name)
{
  if (hasNamedPalette(name))
    return;

  auto *palette = CQColorsMgrInst->getNamedPalette(name);
  assert(palette);

  palettes_.push_back(palette);

  emit themeChanged();
}

bool
CQColorsTheme::
hasNamedPalette(const QString &name) const
{
  int pos = paletteInd(name);

  return (pos >= 0);
}

void
CQColorsTheme::
removeNamedPalette(const QString &name)
{
  int pos = paletteInd(name);
  if (pos < 0) return;

  int n = palettes_.size();

  for (int i = pos + 1; i < n; ++i)
    palettes_[i - 1] = palettes_[i];

  palettes_.pop_back();

  //---

  emit themeChanged();
}

void
CQColorsTheme::
setNamedPalette(int i, const QString &name)
{
  int pos = paletteInd(name);

  auto *palette = CQColorsMgrInst->getNamedPalette(name);
  assert(palette);

  if (pos < 0)
    setPalette(i, palette);
  else
    moveNamedPalette(name, i);
}

void
CQColorsTheme::
moveNamedPalette(const QString &name, int pos)
{
  // validate destination position
  int n = palettes_.size();
  assert(pos >= 0 && pos < n);

  //---

  // get palette position
  int pos1 = paletteInd(name);
  assert(pos1 >= 0);

  if (pos1 == pos)
    return;

  //---

  // remove palette to move from list
  auto *palette = palettes_[pos1];

  for (int i = pos1 + 1; i < n; ++i)
    palettes_[i - 1] = palettes_[i];

  palettes_[n - 1] = nullptr;

  //---

  for (int i = n - 1; i > pos; --i)
    palettes_[i] = palettes_[i - 1];

  palettes_[pos] = palette;

  //---

  emit themeChanged();
}

int
CQColorsTheme::
paletteInd(const QString &name) const
{
  int n = palettes_.size();

  for (int i = 0; i < n; ++i)
    if (palettes_[i]->name() == name)
      return i;

  return -1;
}

#if 0
void
CQColorsTheme::
shiftPalettes(int n)
{
  Palettes palettes;

  palettes.resize(n);

  for (int i = 0; i < n; ++i)
    palettes[i] = palettes_[i];

  int n1 = palettes_.size() - n;

  for (int i = 0; i < n1; ++i)
    palettes_[i] = palettes_[i + n];

  for (int i = 0; i < n; ++i)
    palettes_[i + n1] = palettes[i];

  emit themeChanged();
}
#endif
