#include <CQColorsEditColorType.h>
#include <CQTclUtil.h>

CQColorsEditColorType::
CQColorsEditColorType(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("colorType");

  addItem("Defined"   , QVariant(static_cast<int>(CQColorsPalette::ColorType::DEFINED  )));
  addItem("Model"     , QVariant(static_cast<int>(CQColorsPalette::ColorType::MODEL    )));
  addItem("Functions" , QVariant(static_cast<int>(CQColorsPalette::ColorType::FUNCTIONS)));
  addItem("Cube Helix", QVariant(static_cast<int>(CQColorsPalette::ColorType::CUBEHELIX)));
}

CQColorsPalette::ColorType
CQColorsEditColorType::
type() const
{
  bool ok;

  long i = CQTclUtil::toInt(itemData(currentIndex()), ok);
  // TODO: assert if bad value

  return static_cast<CQColorsPalette::ColorType>(i);
}

void
CQColorsEditColorType::
setType(const CQColorsPalette::ColorType &type)
{
  int t = static_cast<int>(type);

  for (int i = 0; i < count(); ++i) {
    bool ok;

    long t1 = CQTclUtil::toInt(itemData(i), ok);

    if (ok && t == t1)
      setCurrentIndex(i);
  }
}
