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
  addNamedPalette("green_red");
}

CQColorsTheme::
~CQColorsTheme()
{
}

CQColorsPalette *
CQColorsTheme::
palette(int i) const
{
  assert(i >= 0);

  int i1 = int(size_t(i) % palettes_.size());

  return palettes_[size_t(i1)];
}

void
CQColorsTheme::
setPalette(int i, CQColorsPalette *palette)
{
  assert(palette);

  // validate destination position
  auto n = palettes_.size();
  assert(i >= 0 && size_t(i) < n);

  palettes_[size_t(i)] = palette;

  emit themeChanged();
}

void
CQColorsTheme::
setNamedPalettes(const QStringList &names)
{
  palettes_.clear();

  for (size_t i = 0; i < size_t(names.length()); ++i) {
    auto *palette = CQColorsMgrInst->getNamedPalette(names[int(i)]);
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

  auto n = palettes_.size();

  for (size_t i = size_t(pos + 1); i < n; ++i)
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
  auto n = palettes_.size();
  assert(pos >= 0 && size_t(pos) < n);

  //---

  // get palette position
  int pos1 = paletteInd(name);
  assert(pos1 >= 0);

  if (pos1 == pos)
    return;

  //---

  // remove palette to move from list
  auto *palette = palettes_[size_t(pos1)];

  for (size_t i = size_t(pos1 + 1); i < n; ++i)
    palettes_[i - 1] = palettes_[i];

  palettes_[n - 1] = nullptr;

  //---

  for (size_t i = n - 1; i > size_t(pos); --i)
    palettes_[i] = palettes_[i - 1];

  palettes_[size_t(pos)] = palette;

  //---

  emit themeChanged();
}

int
CQColorsTheme::
paletteInd(const QString &name) const
{
  auto n = palettes_.size();

  for (size_t i = 0; i < n; ++i)
    if (palettes_[i]->name() == name)
      return int(i);

  return -1;
}

#if 0
void
CQColorsTheme::
shiftPalettes(int n)
{
  Palettes palettes;

  palettes.resize(n);

  for (size_t i = 0; i < n; ++i)
    palettes[i] = palettes_[i];

  size_t n1 = palettes_.size() - n;

  for (size_t i = 0; i < n1; ++i)
    palettes_[i] = palettes_[i + n];

  for (size_t i = 0; i < n; ++i)
    palettes_[i + n1] = palettes[i];

  emit themeChanged();
}
#endif
