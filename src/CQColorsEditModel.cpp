#include <CQColorsEditModel.h>
#include <CQColorsPalette.h>

CQColorsEditModel::
CQColorsEditModel(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("model");

  for (int i = 0; i < CQColorsPalette::numModels(); ++i)
    addItem(CQColorsPalette::modelName(i).c_str() + QString(" (%1)").arg(i));
}
