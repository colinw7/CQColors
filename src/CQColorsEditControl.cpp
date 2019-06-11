#include <CQColorsEditControl.h>
#include <CQColorsEditCanvas.h>
#include <CQRealSpin.h>
#include <CQColorChooser.h>
#include <CQGroupBox.h>
#include <CQLineEdit.h>
#include <CQUtil.h>
#include <CQTclUtil.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QDir>
#include <QHeaderView>
#include <QCheckBox>
#include <QItemDelegate>
#include <QPainter>
#include <cassert>

CQColorsEditControl::
CQColorsEditControl(CQColorsEditCanvas *canvas) :
 QFrame(0), canvas_(canvas)
{
  setObjectName("paletteControl");

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  QFontMetricsF fm(font());

  //---

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  CQColorsPalette *pal = canvas_->palette();

  //---

  QHBoxLayout *controlLayout = CQUtil::makeLayout<QHBoxLayout>(2, 2);

  //---

  QFrame *colorTypeFrame = createColorTypeCombo("Type", &colorType_);

  if (pal)
    colorType_->setType(pal->colorType());

  connect(colorType_, SIGNAL(currentIndexChanged(int)), this, SLOT(colorTypeChanged(int)));

  controlLayout->addWidget(colorTypeFrame);

  //---

  QFrame *colorModelFrame = createColorModelCombo("Color", &colorModel_);

  if (pal)
    colorModel_->setModel(pal->colorModel());

  connect(colorModel_, SIGNAL(currentIndexChanged(int)), this, SLOT(colorModelChanged(int)));

  controlLayout->addWidget(colorModelFrame);

  //---

  distinctCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Distinct", "distinct");

  distinctCheck_->setChecked(isDistinct());

  connect(distinctCheck_, SIGNAL(stateChanged(int)), this, SLOT(distinctChanged(int)));

  controlLayout->addWidget(distinctCheck_);

  //---

  controlLayout->addStretch(1);

  //---

  layout->addLayout(controlLayout);

  //---

  stack_ = CQUtil::makeWidget<QStackedWidget>("stack");

  //---

  QFrame *modelFrame = CQUtil::makeWidget<QFrame>("modelFrame");

  QVBoxLayout *modelLayout = CQUtil::makeLayout<QVBoxLayout>(modelFrame, 2, 2);

  //---

  // red, green, blue function combos
  CQGroupBox *functionGroupBox = CQUtil::makeLabelWidget<CQGroupBox>("Function", "function");

  //functionGroupBox->setContentsMargins(2, fm.height() + 2, 0, 0);

  QGridLayout *functionGroupLayout = CQUtil::makeLayout<QGridLayout>(functionGroupBox, 0, 2);

  modelLayout->addWidget(functionGroupBox);

  createModelCombo(functionGroupLayout, 0, "R", &redModelLabel_  , &redModelCombo_  );
  createModelCombo(functionGroupLayout, 1, "G", &greenModelLabel_, &greenModelCombo_);
  createModelCombo(functionGroupLayout, 2, "B", &blueModelLabel_ , &blueModelCombo_ );

  if (pal) {
    setRedModel  (pal->redModel  ());
    setGreenModel(pal->greenModel());
    setBlueModel (pal->blueModel ());
  }

  connect(redModelCombo_  , SIGNAL(currentIndexChanged(int)), this, SLOT(modelChanged(int)));
  connect(greenModelCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(modelChanged(int)));
  connect(blueModelCombo_ , SIGNAL(currentIndexChanged(int)), this, SLOT(modelChanged(int)));

  //---

  // red, green, blue negative check boxes
  CQGroupBox *negateGroupBox = CQUtil::makeLabelWidget<CQGroupBox>("Negate", "negate");

  //negateGroupBox->setContentsMargins(2, fm.height() + 2, 0, 0);

  QHBoxLayout *negateGroupLayout = CQUtil::makeLayout<QHBoxLayout>(negateGroupBox, 0, 2);

  modelLayout->addWidget(negateGroupBox);

  modelRNegativeCheck_ = CQUtil::makeLabelWidget<QCheckBox>("R", "rnegative");
  modelGNegativeCheck_ = CQUtil::makeLabelWidget<QCheckBox>("G", "gnegative");
  modelBNegativeCheck_ = CQUtil::makeLabelWidget<QCheckBox>("B", "bnegative");

  connect(modelRNegativeCheck_, SIGNAL(stateChanged(int)), this, SLOT(modelRNegativeChecked(int)));
  connect(modelGNegativeCheck_, SIGNAL(stateChanged(int)), this, SLOT(modelGNegativeChecked(int)));
  connect(modelBNegativeCheck_, SIGNAL(stateChanged(int)), this, SLOT(modelBNegativeChecked(int)));

  negateGroupLayout->addWidget(modelRNegativeCheck_);
  negateGroupLayout->addWidget(modelGNegativeCheck_);
  negateGroupLayout->addWidget(modelBNegativeCheck_);
  negateGroupLayout->addStretch(1);

  //---

  // red, green, blue min/max edits
  CQGroupBox *rangeGroupBox = CQUtil::makeLabelWidget<CQGroupBox>("Range", "range");

  //rangeGroupBox->setContentsMargins(2, fm.height() + 2, 0, 0);

  QHBoxLayout *rangeGroupLayout = CQUtil::makeLayout<QHBoxLayout>(rangeGroupBox, 0, 2);

  modelLayout->addWidget(rangeGroupBox);

  QGridLayout *rangeGridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  redMinMaxLabel_   = CQUtil::makeLabelWidget<QLabel>("R", "r");
  greenMinMaxLabel_ = CQUtil::makeLabelWidget<QLabel>("G", "g");
  blueMinMaxLabel_  = CQUtil::makeLabelWidget<QLabel>("B", "b");

  rangeGridLayout->addWidget(redMinMaxLabel_  , 0, 0);
  rangeGridLayout->addWidget(greenMinMaxLabel_, 1, 0);
  rangeGridLayout->addWidget(blueMinMaxLabel_ , 2, 0);

  createRealEdit(rangeGridLayout, 0, 1, false, "Min", &redMin_  );
  createRealEdit(rangeGridLayout, 0, 3, true , "Max", &redMax_  );
  createRealEdit(rangeGridLayout, 1, 1, false, "Min", &greenMin_);
  createRealEdit(rangeGridLayout, 1, 3, true , "Max", &greenMax_);
  createRealEdit(rangeGridLayout, 2, 1, false, "Min", &blueMin_ );
  createRealEdit(rangeGridLayout, 2, 3, true , "Max", &blueMax_ );

  if (pal) {
    redMin_  ->setValue(pal->redMin  ());
    redMax_  ->setValue(pal->redMax  ());
    greenMin_->setValue(pal->greenMin());
    greenMax_->setValue(pal->greenMax());
    blueMin_ ->setValue(pal->blueMin ());
    blueMax_ ->setValue(pal->blueMax ());
  }

  rangeGroupLayout->addLayout(rangeGridLayout);
  rangeGroupLayout->addStretch(1);

  connect(redMin_  , SIGNAL(valueChanged(double)), this, SLOT(modelRangeValueChanged(double)));
  connect(redMax_  , SIGNAL(valueChanged(double)), this, SLOT(modelRangeValueChanged(double)));
  connect(greenMin_, SIGNAL(valueChanged(double)), this, SLOT(modelRangeValueChanged(double)));
  connect(greenMax_, SIGNAL(valueChanged(double)), this, SLOT(modelRangeValueChanged(double)));
  connect(blueMin_ , SIGNAL(valueChanged(double)), this, SLOT(modelRangeValueChanged(double)));
  connect(blueMax_ , SIGNAL(valueChanged(double)), this, SLOT(modelRangeValueChanged(double)));

  //---

  modelLayout->addStretch(1);

  stack_->addWidget(modelFrame);

  //---

  QFrame *definedFrame = CQUtil::makeWidget<QFrame>("definedFrame");

  QVBoxLayout *definedFrameLayout = CQUtil::makeLayout<QVBoxLayout>(definedFrame, 2, 2);

  definedColors_ = new CQColorsEditDefinedColors;

  if (pal)
    definedColors_->updateColors(pal);

  definedFrameLayout->addWidget(definedColors_);

  connect(definedColors_, SIGNAL(colorsChanged()), this, SLOT(colorsChanged()));

  QFrame *definedButtonsFrame = CQUtil::makeWidget<QFrame>("definedButtonsFrame");

  QHBoxLayout *definedButtonsLayout = CQUtil::makeLayout<QHBoxLayout>(definedButtonsFrame, 2, 2);

  addColorButton_    = CQUtil::makeLabelWidget<QPushButton>("Add"   , "add"   );
  removeColorButton_ = CQUtil::makeLabelWidget<QPushButton>("Remove", "remove");
  loadColorsButton_  = CQUtil::makeLabelWidget<QPushButton>("Load"  , "load"  );
  saveColorsButton_  = CQUtil::makeLabelWidget<QPushButton>("Save"  , "save"  );

  definedButtonsLayout->addWidget(addColorButton_);
  definedButtonsLayout->addWidget(removeColorButton_);
  definedButtonsLayout->addStretch(1);
  definedButtonsLayout->addWidget(loadColorsButton_);
  definedButtonsLayout->addWidget(saveColorsButton_);
  definedButtonsLayout->addStretch(3);

  connect(addColorButton_   , SIGNAL(clicked()), this, SLOT(addColorSlot()));
  connect(removeColorButton_, SIGNAL(clicked()), this, SLOT(removeColorSlot()));
  connect(loadColorsButton_ , SIGNAL(clicked()), this, SLOT(loadColorsSlot()));
  connect(saveColorsButton_ , SIGNAL(clicked()), this, SLOT(saveColorsSlot()));

  definedFrameLayout->addWidget(definedButtonsFrame);

  stack_->addWidget(definedFrame);

  //---

  QFrame *functionsFrame = CQUtil::makeWidget<QFrame>("functionsFrame");

  QGridLayout *functionsGridLayout = CQUtil::makeLayout<QGridLayout>(functionsFrame, 2, 2);

  createFunctionEdit(functionsGridLayout, 0, "R", &redFunctionLabel_  , &redFunctionEdit_  );
  createFunctionEdit(functionsGridLayout, 1, "G", &greenFunctionLabel_, &greenFunctionEdit_);
  createFunctionEdit(functionsGridLayout, 2, "B", &blueFunctionLabel_ , &blueFunctionEdit_ );

  functionsGridLayout->setRowStretch(3, 1);

  if (pal) {
    setRedFunction  (pal->redFunction  ());
    setGreenFunction(pal->greenFunction());
    setBlueFunction (pal->blueFunction ());
  }

  connect(redFunctionEdit_  , SIGNAL(editingFinished()), this, SLOT(functionChanged()));
  connect(greenFunctionEdit_, SIGNAL(editingFinished()), this, SLOT(functionChanged()));
  connect(blueFunctionEdit_ , SIGNAL(editingFinished()), this, SLOT(functionChanged()));

  stack_->addWidget(functionsFrame);

  //---

  QFrame *cubeFrame = CQUtil::makeWidget<QFrame>("cubeFrame");

  QGridLayout *cubeGridLayout = CQUtil::makeLayout<QGridLayout>(cubeFrame, 2, 2);

  createRealEdit(cubeGridLayout, 0, "Start"     , &cubeStart_     );
  createRealEdit(cubeGridLayout, 1, "Cycles"    , &cubeCycles_    );
  createRealEdit(cubeGridLayout, 2, "Saturation", &cubeSaturation_);

  cubeNegativeCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Negative", "negative");

  connect(cubeNegativeCheck_, SIGNAL(stateChanged(int)), this, SLOT(cubeNegativeChecked(int)));

  cubeGridLayout->addWidget(cubeNegativeCheck_, 3, 0, 1, 2);

  cubeGridLayout->setRowStretch(4, 1);

  if (pal) {
    setCubeStart     (pal->cbStart     ());
    setCubeCycles    (pal->cbCycles    ());
    setCubeSaturation(pal->cbSaturation());
  }

  connect(cubeStart_     , SIGNAL(valueChanged(double)), this, SLOT(cubeValueChanged(double)));
  connect(cubeCycles_    , SIGNAL(valueChanged(double)), this, SLOT(cubeValueChanged(double)));
  connect(cubeSaturation_, SIGNAL(valueChanged(double)), this, SLOT(cubeValueChanged(double)));

  stack_->addWidget(cubeFrame);

  //---

  layout->addWidget(stack_);
  layout->addStretch(1);

  //---

  connect(this, SIGNAL(stateChanged()), canvas, SLOT(update()));

  connect(canvas, SIGNAL(colorsChanged()), this, SLOT(updateState()));

  //---

  updateColorType ();
  updateColorModel();
}

void
CQColorsEditControl::
updateState()
{
  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return;

  setColorType (pal->colorType ());
  setColorModel(pal->colorModel());

  distinctCheck_->setChecked(pal->isDistinct());

  setRedModel  (pal->redModel  ());
  setGreenModel(pal->greenModel());
  setBlueModel (pal->blueModel ());

  modelRNegativeCheck_->setChecked(pal->isRedNegative  ());
  modelGNegativeCheck_->setChecked(pal->isGreenNegative());
  modelBNegativeCheck_->setChecked(pal->isBlueNegative ());

  redMin_  ->setValue(pal->redMin  ());
  redMax_  ->setValue(pal->redMax  ());
  greenMin_->setValue(pal->greenMin());
  greenMax_->setValue(pal->greenMax());
  blueMin_ ->setValue(pal->blueMin ());
  blueMax_ ->setValue(pal->blueMax ());

  //---

  setCubeStart     (pal->cbStart     ());
  setCubeCycles    (pal->cbCycles    ());
  setCubeSaturation(pal->cbSaturation());

  //---

  definedColors_->updateColors(pal);
}

void
CQColorsEditControl::
colorTypeChanged(int)
{
  setColorType(colorType_->type());
}

void
CQColorsEditControl::
distinctChanged(int)
{
  setDistinct(distinctCheck_->isChecked());
}

CQColorsPalette::ColorType
CQColorsEditControl::
colorType() const
{
  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return CQColorsPalette::ColorType::NONE;

  return pal->colorType();
}

void
CQColorsEditControl::
setColorType(CQColorsPalette::ColorType colorType)
{
  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return;

  pal->setColorType(colorType);

  updateColorType();

  emit stateChanged();
}

bool
CQColorsEditControl::
isDistinct() const
{
  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return false;

  return pal->isDistinct();
}

void
CQColorsEditControl::
setDistinct(bool b)
{
  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return;

  pal->setDistinct(b);

  emit stateChanged();
}

void
CQColorsEditControl::
updateColorType()
{
  CQColorsPalette::ColorType colorType = this->colorType();

  if      (colorType == CQColorsPalette::ColorType::MODEL) {
    colorType_->setCurrentIndex(0);
    stack_    ->setCurrentIndex(0);
  }
  else if (colorType == CQColorsPalette::ColorType::DEFINED) {
    colorType_->setCurrentIndex(1);
    stack_    ->setCurrentIndex(1);
  }
  else if (colorType == CQColorsPalette::ColorType::FUNCTIONS) {
    colorType_->setCurrentIndex(2);
    stack_    ->setCurrentIndex(2);
  }
  else if (colorType == CQColorsPalette::ColorType::CUBEHELIX) {
    colorType_->setCurrentIndex(3);
    stack_    ->setCurrentIndex(3);
  }
}

void
CQColorsEditControl::
colorModelChanged(int)
{
  setColorModel(colorModel_->model());
}

CQColorsPalette::ColorModel
CQColorsEditControl::
colorModel() const
{
  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return CQColorsPalette::ColorModel::NONE;

  return pal->colorModel();
}

void
CQColorsEditControl::
setColorModel(CQColorsPalette::ColorModel colorModel)
{
  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return;

  pal->setColorModel(colorModel);

  updateColorModel();

  emit stateChanged();
}

void
CQColorsEditControl::
updateColorModel()
{
  CQColorsPalette::ColorModel colorModel = this->colorModel();

  if      (colorModel == CQColorsPalette::ColorModel::RGB) {
    colorModel_->setCurrentIndex(0);

    redModelLabel_  ->setText("R");
    greenModelLabel_->setText("G");
    blueModelLabel_ ->setText("B");

    modelRNegativeCheck_->setText("R");
    modelGNegativeCheck_->setText("G");
    modelBNegativeCheck_->setText("B");

    redMinMaxLabel_  ->setText("R");
    greenMinMaxLabel_->setText("G");
    blueMinMaxLabel_ ->setText("B");

    redFunctionLabel_  ->setText("R");
    greenFunctionLabel_->setText("G");
    blueFunctionLabel_ ->setText("B");
  }
  else if (colorModel == CQColorsPalette::ColorModel::HSV) {
    colorModel_->setCurrentIndex(1);

    redModelLabel_  ->setText("H");
    greenModelLabel_->setText("S");
    blueModelLabel_ ->setText("V");

    modelRNegativeCheck_->setText("H");
    modelGNegativeCheck_->setText("S");
    modelBNegativeCheck_->setText("V");

    redMinMaxLabel_  ->setText("H");
    greenMinMaxLabel_->setText("S");
    blueMinMaxLabel_ ->setText("V");

    redFunctionLabel_  ->setText("H");
    greenFunctionLabel_->setText("S");
    blueFunctionLabel_ ->setText("V");
  }
}

int
CQColorsEditControl::
redModel() const
{
  return redModelCombo_->currentIndex();
}

void
CQColorsEditControl::
setRedModel(int model)
{
  redModelCombo_->setCurrentIndex(model);
}

int
CQColorsEditControl::
greenModel() const
{
  return greenModelCombo_->currentIndex();
}

void
CQColorsEditControl::
setGreenModel(int model)
{
  greenModelCombo_->setCurrentIndex(model);
}

int
CQColorsEditControl::
blueModel() const
{
  return blueModelCombo_->currentIndex();
}

void
CQColorsEditControl::
setBlueModel(int model)
{
  blueModelCombo_->setCurrentIndex(model);
}

void
CQColorsEditControl::
modelChanged(int model)
{
  CQColorsEditModel *me = qobject_cast<CQColorsEditModel *>(sender());
  assert(me);

  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return;

  if      (me == redModelCombo_  ) pal->setRedModel  (model);
  else if (me == greenModelCombo_) pal->setGreenModel(model);
  else if (me == blueModelCombo_ ) pal->setBlueModel (model);

  emit stateChanged();
}

void
CQColorsEditControl::
modelRNegativeChecked(int state)
{
  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return;

  pal->setRedNegative(state);

  emit stateChanged();
}

void
CQColorsEditControl::
modelGNegativeChecked(int state)
{
  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return;

  pal->setGreenNegative(state);

  emit stateChanged();
}

void
CQColorsEditControl::
modelBNegativeChecked(int state)
{
  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return;

  pal->setBlueNegative(state);

  emit stateChanged();
}

void
CQColorsEditControl::
modelRangeValueChanged(double r)
{
  CQRealSpin *rs = qobject_cast<CQRealSpin *>(sender());
  assert(rs);

  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return;

  if      (rs == redMin_  ) pal->setRedMin  (r);
  else if (rs == redMax_  ) pal->setRedMax  (r);
  else if (rs == greenMin_) pal->setGreenMin(r);
  else if (rs == greenMax_) pal->setGreenMax(r);
  else if (rs == blueMin_ ) pal->setBlueMin (r);
  else if (rs == blueMax_ ) pal->setBlueMax (r);

  emit stateChanged();
}

void
CQColorsEditControl::
cubeNegativeChecked(int state)
{
  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return;

  pal->setCubeNegative(state);

  emit stateChanged();
}

void
CQColorsEditControl::
colorsChanged()
{
  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return;

  pal->resetDefinedColors();

  for (int i = 0; i < definedColors_->numRealColors(); ++i) {
    const CQColorsEditDefinedColors::RealColor &realColor = definedColors_->realColor(i);

    pal->addDefinedColor(realColor.r, realColor.c);
  }

  emit stateChanged();
}

void
CQColorsEditControl::
addColorSlot()
{
  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return;

  int row = 0;

  QList<QTableWidgetItem *> selectedItems = definedColors_->selectedItems();

  if (selectedItems.length())
    row = selectedItems[0]->row();

  const CQColorsPalette::ColorMap &colors = pal->definedValueColors();

  double x = 0.5;
  QColor c = QColor(127, 127, 127);

  int row1 = -1;

  if      (row + 1 < int(colors.size()))
    row1 = row;
  else if (row < int(colors.size()) && row > 0)
    row1 = row - 1;

  if (row1 >= 0) {
    auto p = colors.begin();

    std::advance(p, row1);

    double        x1 = (*p).first;
    const QColor &c1 = (*p).second;

    ++p;
    double        x2 = (*p).first;
    const QColor &c2 = (*p).second;

    x = (x1 + x2)/2;

    if      (pal->colorModel() == CQColorsPalette::ColorModel::RGB)
      c = CQColorsPalette::interpRGB(c1, c2, 0.5);
    else if (pal->colorModel() == CQColorsPalette::ColorModel::HSV)
      c = CQColorsPalette::interpHSV(c1, c2, 0.5);
    else
      c = CQColorsPalette::interpRGB(c1, c2, 0.5);
  }

  pal->addDefinedColor(x, c);

  definedColors_->updateColors(pal);

  emit stateChanged();
}

void
CQColorsEditControl::
removeColorSlot()
{
  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return;

  QList<QTableWidgetItem *> selectedItems = definedColors_->selectedItems();

  if (! selectedItems.length())
    return;

  int row = selectedItems[0]->row();

  CQColorsPalette::ColorMap colors = pal->definedValueColors();

  CQColorsPalette::ColorMap colors1;

  int nc = colors.size();

  for (int i = 0; i < nc; ++i) {
    if (i != row)
      colors1[i] = colors[i];
  }

  pal->setDefinedColors(colors1);

  definedColors_->updateColors(pal);

  emit stateChanged();
}

void
CQColorsEditControl::
loadColorsSlot()
{
  QString dir = QDir::current().dirName();

  QString fileName = QFileDialog::getOpenFileName(this, "Open Palette File", dir, "Files (*.pal)");

  if (! fileName.length())
    return;

  readFile(fileName);

  emit stateChanged();
}

void
CQColorsEditControl::
saveColorsSlot()
{
  QString dir = QDir::current().dirName() + "/colors.pal";

  QString fileName = QFileDialog::getSaveFileName(this, "Save Palette File", dir, "Files (*.pal)");

  if (! fileName.length())
    return;

  saveFile(fileName);
}

void
CQColorsEditControl::
readFile(const QString &fileName)
{
  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return;

  pal->readFile(fileName.toStdString());

  definedColors_->updateColors(pal);

  distinctCheck_->setChecked(pal->isDistinct());
}

void
CQColorsEditControl::
saveFile(const QString &fileName)
{
  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return;

  pal->saveFile(fileName.toStdString());
}

std::string
CQColorsEditControl::
redFunction() const
{
  return redFunctionEdit_->text().toStdString();
}

void
CQColorsEditControl::
setRedFunction(const std::string &fn)
{
  redFunctionEdit_->setText(fn.c_str());
}

std::string
CQColorsEditControl::
greenFunction() const
{
  return greenFunctionEdit_->text().toStdString();
}

void
CQColorsEditControl::
setGreenFunction(const std::string &fn)
{
  greenFunctionEdit_->setText(fn.c_str());
}

std::string
CQColorsEditControl::
blueFunction() const
{
  return blueFunctionEdit_->text().toStdString();
}

void
CQColorsEditControl::
setBlueFunction(const std::string &fn)
{
  blueFunctionEdit_->setText(fn.c_str());
}

void
CQColorsEditControl::
functionChanged()
{
  CQLineEdit *le = qobject_cast<CQLineEdit *>(sender());
  assert(le);

  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return;

  if      (le == redFunctionEdit_)
    pal->setRedFunction  (le->text().toStdString());
  else if (le == greenFunctionEdit_)
    pal->setGreenFunction(le->text().toStdString());
  else if (le == blueFunctionEdit_)
    pal->setBlueFunction (le->text().toStdString());

  emit stateChanged();
}

double
CQColorsEditControl::
cubeStart() const
{
  return cubeStart_->value();
}

void
CQColorsEditControl::
setCubeStart(double r)
{
  cubeStart_->setValue(r);
}

double
CQColorsEditControl::
cubeCycles() const
{
  return cubeCycles_->value();
}

void
CQColorsEditControl::
setCubeCycles(double r)
{
  cubeCycles_->setValue(r);
}

double
CQColorsEditControl::
cubeSaturation() const
{
  return cubeSaturation_->value();
}

void
CQColorsEditControl::
setCubeSaturation(double r)
{
  cubeSaturation_->setValue(r);
}

void
CQColorsEditControl::
cubeValueChanged(double r)
{
  CQRealSpin *rs = qobject_cast<CQRealSpin *>(sender());
  assert(rs);

  CQColorsPalette *pal = canvas_->palette();
  if (! pal) return;

  if      (rs == cubeStart_)
    pal->setCbStart(r);
  else if (rs == cubeCycles_)
    pal->setCbCycles(r);
  else if (rs == cubeSaturation_)
    pal->setCbSaturation(r);

  emit stateChanged();
}

QFrame *
CQColorsEditControl::
createColorTypeCombo(const QString &label, CQColorsEditColorType **type)
{
  QFrame *frame = CQUtil::makeWidget<QFrame>("frame");

  QHBoxLayout *layout = CQUtil::makeLayout<QHBoxLayout>(frame, 2, 2);

  *type = new CQColorsEditColorType;

  QLabel *colorLabel = CQUtil::makeLabelWidget<QLabel>(label, "labe;");

  layout->addWidget(colorLabel);
  layout->addWidget(*type);
  layout->addStretch(1);

  return frame;
}

QFrame *
CQColorsEditControl::
createColorModelCombo(const QString &label, CQColorsEditColorModel **model)
{
  QFrame *frame = CQUtil::makeWidget<QFrame>("frame");

  QHBoxLayout *layout = CQUtil::makeLayout<QHBoxLayout>(frame, 2, 2);

  *model = new CQColorsEditColorModel;

  QLabel *colorLabel = CQUtil::makeLabelWidget<QLabel>(label, "label");

  layout->addWidget(colorLabel);
  layout->addWidget(*model);
  layout->addStretch(1);

  return frame;
}

void
CQColorsEditControl::
createModelCombo(QGridLayout *grid, int row, const QString &label,
                 QLabel **modelLabel, CQColorsEditModel **modelCombo)
{
  *modelLabel = CQUtil::makeLabelWidget<QLabel>(label, "label");

  *modelCombo = CQUtil::makeWidget<CQColorsEditModel>("combo");

  grid->addWidget(*modelLabel, row, 0);
  grid->addWidget(*modelCombo, row, 1);

  grid->setColumnStretch(2, true);
}

void
CQColorsEditControl::
createFunctionEdit(QGridLayout *grid, int row, const QString &label,
                   QLabel **functionLabel, CQLineEdit **functionEdit)
{
  *functionLabel = CQUtil::makeLabelWidget<QLabel>(label, "label");
  *functionEdit  = CQUtil::makeWidget<CQLineEdit>("edit");

  grid->addWidget(*functionLabel, row, 0);
  grid->addWidget(*functionEdit , row, 1);

  grid->setColumnStretch(2, true);
}

void
CQColorsEditControl::
createRealEdit(QGridLayout *grid, int row, const QString &label, CQRealSpin **edit)
{
  createRealEdit(grid, row, 0, true, label, edit);
}

void
CQColorsEditControl::
createRealEdit(QGridLayout *grid, int row, int col, bool stretch,
               const QString &label, CQRealSpin **edit)
{
  *edit = CQUtil::makeWidget<CQRealSpin>("spin");

  QLabel *editLabel = CQUtil::makeLabelWidget<QLabel>(label, "label");

  grid->addWidget(editLabel, row, col);
  grid->addWidget(*edit    , row, col + 1);

  if (stretch)
    grid->setColumnStretch(col + 2, true);
}

//---

CQColorsEditColorType::
CQColorsEditColorType(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("colorType");

  addItem("Model"     , QVariant(static_cast<int>(CQColorsPalette::ColorType::MODEL    )));
  addItem("Defined"   , QVariant(static_cast<int>(CQColorsPalette::ColorType::DEFINED  )));
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

//---

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

//---

CQColorsEditModel::
CQColorsEditModel(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("model");

  for (int i = 0; i < CQColorsPalette::numModels(); ++i)
    addItem(CQColorsPalette::modelName(i).c_str() + QString(" (%1)").arg(i));
}

//---

class CQColorsEditDefinedColorsDelegate : public QItemDelegate {
 public:
  CQColorsEditDefinedColorsDelegate(CQColorsEditDefinedColors *colors_);

  // Override to create editor
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &ind) const;

  // Override to get content from editor
  void setEditorData(QWidget *editor, const QModelIndex &ind) const;

  // Override to set editor from content
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &ind) const;

  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &ind) const;

  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                            const QModelIndex &ind) const;

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &ind) const;

 private:
  void drawColor(QPainter *painter, const QStyleOptionViewItem &option,
                 const QColor &color, const QModelIndex &index) const;

 private:
  CQColorsEditDefinedColors *colors_;
};

CQColorsEditDefinedColors::
CQColorsEditDefinedColors(QWidget *parent) :
 QTableWidget(parent)
{
  setObjectName("defineColors");

  verticalHeader()->hide();

  CQColorsEditDefinedColorsDelegate *delegate =
    new CQColorsEditDefinedColorsDelegate(this);

  setItemDelegate(delegate);
  setEditTriggers(QAbstractItemView::AllEditTriggers);

  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);

  QHeaderView *header = horizontalHeader();

  header->setStretchLastSection(true) ;

  //setSizeAdjustPolicy(QTableWidget::AdjustToContents);
}

void
CQColorsEditDefinedColors::
updateColors(CQColorsPalette *canvas)
{
  setColumnCount(2);

  setHorizontalHeaderLabels(QStringList() << "X" << "Color");

  setRowCount(canvas->numDefinedColors());

  realColors_.clear();

  for (const auto &c : canvas->definedValueColors())
    realColors_.emplace_back(c.first, c.second.rgba());

  for (int r = 0; r < numRealColors(); ++r) {
    const RealColor &realColor = this->realColor(r);

    QTableWidgetItem *item1 = new QTableWidgetItem(QString("%1").arg(realColor.r));
    QTableWidgetItem *item2 = new QTableWidgetItem(realColor.c.name());

    setItem(r, 0, item1);
    setItem(r, 1, item2);
  }
}

const CQColorsEditDefinedColors::RealColor &
CQColorsEditDefinedColors::
realColor(int r) const
{
  return realColors_[r];
}

void
CQColorsEditDefinedColors::
setRealColor(int r, const RealColor &realColor)
{
  realColors_[r] = realColor;

  //QTableWidgetItem *item1 = new QTableWidgetItem(QString("%1").arg(realColor.r));
  //QTableWidgetItem *item2 = new QTableWidgetItem(realColor.c.name());

  //setItem(r, 0, item1);
  //setItem(r, 1, item2);

  emit colorsChanged();
}

//---

CQColorsEditDefinedColorsDelegate::
CQColorsEditDefinedColorsDelegate(CQColorsEditDefinedColors *colors_) :
 QItemDelegate(colors_), colors_(colors_)
{
}

QWidget *
CQColorsEditDefinedColorsDelegate::
createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &ind) const
{
  QTableWidgetItem *item = colors_->item(ind.row(), ind.column());
  assert(item);

  const CQColorsEditDefinedColors::RealColor &realColor = colors_->realColor(ind.row());

  if       (ind.column() == 0) {
    CQRealSpin *edit = CQUtil::makeWidget<CQRealSpin>(parent, "spin");

    edit->setValue(realColor.r);

    return edit;
  }
  else if (ind.column() == 1) {
    CQColorChooser *edit = CQUtil::makeWidget<CQColorChooser>(parent, "color");

    edit->setStyles(CQColorChooser::Text | CQColorChooser::ColorButton);

    edit->setColor(realColor.c);

    return edit;
  }
  else
    return QItemDelegate::createEditor(parent, option, ind);
}

// model->editor
void
CQColorsEditDefinedColorsDelegate::
setEditorData(QWidget *w, const QModelIndex &ind) const
{
  QTableWidgetItem *item = colors_->item(ind.row(), ind.column());
  assert(item);

  const CQColorsEditDefinedColors::RealColor &realColor = colors_->realColor(ind.row());

  if       (ind.column() == 0) {
    CQRealSpin *edit = qobject_cast<CQRealSpin *>(w);

    edit->setValue(realColor.r);
  }
  else if (ind.column() == 1) {
    CQColorChooser *edit = qobject_cast<CQColorChooser *>(w);

    const QColor &c = realColor.c;

    edit->setColor(c);
  }
  else
    QItemDelegate::setEditorData(w, ind);
}

// editor->model
void
CQColorsEditDefinedColorsDelegate::
setModelData(QWidget *w, QAbstractItemModel *model, const QModelIndex &ind) const
{
  QTableWidgetItem *item = colors_->item(ind.row(), ind.column());
  assert(item);

  CQColorsEditDefinedColors::RealColor realColor = colors_->realColor(ind.row());

  if       (ind.column() == 0) {
    CQRealSpin *edit = qobject_cast<CQRealSpin *>(w);

    double r = edit->value();

    model->setData(ind, QVariant(r));

    realColor.r = r;
  }
  else if (ind.column() == 1) {
    CQColorChooser *edit = qobject_cast<CQColorChooser *>(w);

    realColor.c = edit->color();

    model->setData(ind, QVariant(realColor.c));
  }
  else
    QItemDelegate::setModelData(w, model, ind);

  colors_->setRealColor(ind.row(), realColor);
}

QSize
CQColorsEditDefinedColorsDelegate::
sizeHint(const QStyleOptionViewItem &option, const QModelIndex &ind) const
{
  QTableWidgetItem *item = colors_->item(ind.row(), ind.column());
  assert(item);

  return QItemDelegate::sizeHint(option, ind);
}

void
CQColorsEditDefinedColorsDelegate::
updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                     const QModelIndex &/* ind */) const
{
  editor->setGeometry(option.rect);
}

void
CQColorsEditDefinedColorsDelegate::
paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &ind) const
{
  QTableWidgetItem *item = colors_->item(ind.row(), ind.column());
  assert(item);

  if (ind.column() == 1) {
    const CQColorsEditDefinedColors::RealColor &realColor = colors_->realColor(ind.row());

    drawColor(painter, option, realColor.c, ind);
  }
  else
    QItemDelegate::paint(painter, option, ind);
}

void
CQColorsEditDefinedColorsDelegate::
drawColor(QPainter *painter, const QStyleOptionViewItem &option,
          const QColor &color, const QModelIndex &index) const
{
  QItemDelegate::drawBackground(painter, option, index);

  QRect rect = option.rect;

  rect.setWidth(option.rect.height());

  rect.adjust(0, 1, -3, -2);

  painter->fillRect(rect, QBrush(color));

  painter->setPen(QColor(0,0,0)); // TODO: contrast border

  painter->drawRect(rect);

//QFontMetrics fm(painter->font());

  int x = rect.right() + 2;
//int y = rect.top() + fm.ascent();

  QRect rect1;

  rect1.setCoords(x, option.rect.top(), option.rect.right(), option.rect.bottom());

  QItemDelegate::drawDisplay(painter, option, rect1, color.name());
}
