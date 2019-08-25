#include <CQColorsEditColorModel.h>
#include <CQTclUtil.h>

CQColorsEditColorModel::
CQColorsEditColorModel(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("colorModel");

  addItem("RGB", QVariant(static_cast<int>(CQColorsPalette::ColorModel::RGB)));
  addItem("HSV", QVariant(static_cast<int>(CQColorsPalette::ColorModel::HSV)));
}

CQColorsPalette::ColorModel
CQColorsEditColorModel::
model() const
{
  bool ok;

  long i = CQTclUtil::toInt(itemData(currentIndex()), ok);
  // TODO: assert if bad value

  return static_cast<CQColorsPalette::ColorModel>(i);
}

void
CQColorsEditColorModel::
setModel(const CQColorsPalette::ColorModel &model)
{
  int m = static_cast<int>(model);

  for (int i = 0; i < count(); ++i) {
    bool ok;

    long m1 = CQTclUtil::toInt(itemData(i), ok);

    if (ok && m == m1)
      setCurrentIndex(i);
  }
}
