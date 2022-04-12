#include <CQColorsEditControl.h>
#include <CQColorsEditCanvas.h>
#include <CQColorsEditColorType.h>
#include <CQColorsEditColorModel.h>
#include <CQColorsEditModel.h>
#include <CQColorsEditDefinedColors.h>

#include <CQRealSpin.h>
#include <CQGroupBox.h>
#include <CQLineEdit.h>
#include <CQUtil.h>
#include <CSafeIndex.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QDir>
#include <QCheckBox>
#include <cassert>

CQColorsEditControl::
CQColorsEditControl(CQColorsEditCanvas *canvas) :
 QScrollArea(nullptr), canvas_(canvas)
{
  setObjectName("paletteControl");

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  QFontMetricsF fm(font());

  //---

  auto *frame  = CQUtil::makeWidget<QFrame>("frame");
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(frame, 2, 2);

  setWidget(frame);
  setWidgetResizable(true);

  auto *pal = canvas_->palette();

  //---

  if (! canvas_->isGray()) {
    auto *controlFrame  = CQUtil::makeWidget<QFrame>("controlFrame");
    auto *controlLayout = CQUtil::makeLayout<QHBoxLayout>(controlFrame, 2, 2);

    controlFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    //---

    auto *colorTypeFrame = createColorTypeCombo("Type", &colorType_);

    if (pal)
      colorType_->setType(pal->colorType());

    colorType_->setToolTip("Color Type");

    connect(colorType_, SIGNAL(currentIndexChanged(int)), this, SLOT(colorTypeChanged(int)));

    controlLayout->addWidget(colorTypeFrame);

    //---

    auto *colorModelFrame = createColorModelCombo("Color", &colorModel_);

    if (pal)
      colorModel_->setModel(pal->colorModel());

    colorModel_->setToolTip("Color Model");

    connect(colorModel_, SIGNAL(currentIndexChanged(int)), this, SLOT(colorModelChanged(int)));

    controlLayout->addWidget(colorModelFrame);

    //---

    distinctCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Distinct", "distinct");

    distinctCheck_->setChecked(isDistinct());
    distinctCheck_->setToolTip("Palette uses distinct (non-interpolated) colors");

    connect(distinctCheck_, SIGNAL(stateChanged(int)), this, SLOT(distinctChanged(int)));

    controlLayout->addWidget(distinctCheck_);

    //---

    invertedCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Inverted", "inverted");

    invertedCheck_->setChecked(isInverted());
    invertedCheck_->setToolTip("Invert order of colors");

    connect(invertedCheck_, SIGNAL(stateChanged(int)), this, SLOT(invertedChanged(int)));

    controlLayout->addWidget(invertedCheck_);

    //---

    controlLayout->addStretch(1);

    //---

    layout->addWidget(controlFrame);
  }

  //---

  stack_ = CQUtil::makeWidget<QStackedWidget>("stack");

  //---

  auto *definedFrame       = CQUtil::makeWidget<QFrame>("definedFrame");
  auto *definedFrameLayout = CQUtil::makeLayout<QVBoxLayout>(definedFrame, 2, 2);

  definedFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  definedColors_ = new CQColorsEditDefinedColors;

  connect(definedColors_, SIGNAL(itemSelectionChanged()), this, SLOT(updateDefinedState()));

  if (pal)
    definedColors_->updateColors(pal);

  definedFrameLayout->addWidget(definedColors_);

  connect(definedColors_, SIGNAL(colorsChanged()), this, SLOT(colorsChanged()));

  auto *definedButtonsFrame  = CQUtil::makeWidget<QFrame>("definedButtonsFrame");
  auto *definedButtonsLayout = CQUtil::makeLayout<QHBoxLayout>(definedButtonsFrame, 2, 2);

  addColorButton_    = CQUtil::makeLabelWidget<QPushButton>("Add"   , "add"   );
  removeColorButton_ = CQUtil::makeLabelWidget<QPushButton>("Remove", "remove");
  loadColorsButton_  = CQUtil::makeLabelWidget<QPushButton>("Load"  , "load"  );
  saveColorsButton_  = CQUtil::makeLabelWidget<QPushButton>("Save"  , "save"  );

  addColorButton_   ->setToolTip("Add new color");
  removeColorButton_->setToolTip("Removed selected color");
  loadColorsButton_ ->setToolTip("Load colors from file");
  saveColorsButton_ ->setToolTip("Save colors to file");

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

  auto *modelFrame  = CQUtil::makeWidget<QFrame>("modelFrame");
  auto *modelLayout = CQUtil::makeLayout<QVBoxLayout>(modelFrame, 2, 2);

  //---

  // red, green, blue function combos
  auto *functionGroupBox    = CQUtil::makeLabelWidget<CQGroupBox>("Function", "function");
  auto *functionGroupLayout = CQUtil::makeLayout<QGridLayout>(functionGroupBox, 0, 2);

  //functionGroupBox->setContentsMargins(2, fm.height() + 2, 0, 0);

  modelLayout->addWidget(functionGroupBox);

  createModelCombo(functionGroupLayout, 0, "R", &redModelLabel_  , &redModelCombo_  );
  createModelCombo(functionGroupLayout, 1, "G", &greenModelLabel_, &greenModelCombo_);
  createModelCombo(functionGroupLayout, 2, "B", &blueModelLabel_ , &blueModelCombo_ );

  if (pal) {
    setRedModel  (pal->redModel  ());
    setGreenModel(pal->greenModel());
    setBlueModel (pal->blueModel ());
  }

  redModelCombo_  ->setToolTip("Red function");
  greenModelCombo_->setToolTip("Green function");
  blueModelCombo_ ->setToolTip("Blue function");

  connect(redModelCombo_  , SIGNAL(currentIndexChanged(int)), this, SLOT(modelChanged(int)));
  connect(greenModelCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(modelChanged(int)));
  connect(blueModelCombo_ , SIGNAL(currentIndexChanged(int)), this, SLOT(modelChanged(int)));

  //---

  // red, green, blue negative check boxes
  auto *negateGroupBox    = CQUtil::makeLabelWidget<CQGroupBox>("Negate", "negate");
  auto *negateGroupLayout = CQUtil::makeLayout<QHBoxLayout>(negateGroupBox, 0, 2);

  //negateGroupBox->setContentsMargins(2, fm.height() + 2, 0, 0);

  modelLayout->addWidget(negateGroupBox);

  modelRNegativeCheck_ = CQUtil::makeLabelWidget<QCheckBox>("R", "rnegative");
  modelGNegativeCheck_ = CQUtil::makeLabelWidget<QCheckBox>("G", "gnegative");
  modelBNegativeCheck_ = CQUtil::makeLabelWidget<QCheckBox>("B", "bnegative");

  modelRNegativeCheck_->setToolTip("Negate result of Red function");
  modelGNegativeCheck_->setToolTip("Negate result of Green function");
  modelBNegativeCheck_->setToolTip("Negate result of Blue function");

  connect(modelRNegativeCheck_, SIGNAL(stateChanged(int)), this, SLOT(modelRNegativeChecked(int)));
  connect(modelGNegativeCheck_, SIGNAL(stateChanged(int)), this, SLOT(modelGNegativeChecked(int)));
  connect(modelBNegativeCheck_, SIGNAL(stateChanged(int)), this, SLOT(modelBNegativeChecked(int)));

  negateGroupLayout->addWidget(modelRNegativeCheck_);
  negateGroupLayout->addWidget(modelGNegativeCheck_);
  negateGroupLayout->addWidget(modelBNegativeCheck_);
  negateGroupLayout->addStretch(1);

  //---

  // red, green, blue min/max edits
  auto *rangeGroupBox    = CQUtil::makeLabelWidget<CQGroupBox>("Range", "range");
  auto *rangeGroupLayout = CQUtil::makeLayout<QHBoxLayout>(rangeGroupBox, 0, 2);

  //rangeGroupBox->setContentsMargins(2, fm.height() + 2, 0, 0);

  modelLayout->addWidget(rangeGroupBox);

  auto *rangeGridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

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

  redMin_  ->setToolTip("Minimum Red value");
  redMax_  ->setToolTip("Maximum Red value");
  greenMin_->setToolTip("Minimum Green value");
  greenMax_->setToolTip("Maximum Green value");
  blueMin_ ->setToolTip("Minimum Blue value");
  blueMax_ ->setToolTip("Maximum Blue value");

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

  auto *functionsFrame      = CQUtil::makeWidget<QFrame>("functionsFrame");
  auto *functionsGridLayout = CQUtil::makeLayout<QGridLayout>(functionsFrame, 2, 2);

  createFunctionEdit(functionsGridLayout, 0, "R", &redFunctionLabel_  , &redFunctionEdit_  );
  createFunctionEdit(functionsGridLayout, 1, "G", &greenFunctionLabel_, &greenFunctionEdit_);
  createFunctionEdit(functionsGridLayout, 2, "B", &blueFunctionLabel_ , &blueFunctionEdit_ );

  redFunctionEdit_  ->setToolTip("Red Function Expression");
  greenFunctionEdit_->setToolTip("Green Function Expression");
  blueFunctionEdit_ ->setToolTip("Blue Function Expression");

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

  auto *cubeFrame      = CQUtil::makeWidget<QFrame>("cubeFrame");
  auto *cubeGridLayout = CQUtil::makeLayout<QGridLayout>(cubeFrame, 2, 2);

  createRealEdit(cubeGridLayout, 0, "Start"     , &cubeStart_     );
  createRealEdit(cubeGridLayout, 1, "Cycles"    , &cubeCycles_    );
  createRealEdit(cubeGridLayout, 2, "Saturation", &cubeSaturation_);

  cubeStart_     ->setToolTip("Cube Helix Start Value");
  cubeCycles_    ->setToolTip("Cube Helix Cycles Count");
  cubeSaturation_->setToolTip("Cube Helix Saturation");

  cubeNegativeCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Negative", "negative");

  cubeNegativeCheck_->setToolTip("Negate Colors");

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
//layout->addStretch(1);

  //---

  connect(this, SIGNAL(stateChanged()), canvas, SLOT(update()));

  connect(canvas, SIGNAL(colorsChanged()), this, SLOT(updateState()));

  //---

  updateDefinedState();

  updateColorType ();
  updateColorModel();
}

void
CQColorsEditControl::
updateState()
{
  auto *pal = canvas_->palette();
  if (! pal) return;

  setColorType (pal->colorType ());
  setColorModel(pal->colorModel());

  if (distinctCheck_)
    distinctCheck_->setChecked(pal->isDistinct());

  if (invertedCheck_)
    invertedCheck_->setChecked(pal->isInverted());

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
updateDefinedState()
{
  removeColorButton_->setEnabled(! definedColors_->selectedItems().empty());
}

void
CQColorsEditControl::
colorTypeChanged(int)
{
  assert(colorType_);

  setColorType(colorType_->type());
}

void
CQColorsEditControl::
distinctChanged(int)
{
  assert(distinctCheck_);

  setDistinct(distinctCheck_->isChecked());
}

void
CQColorsEditControl::
invertedChanged(int)
{
  assert(invertedCheck_);

  setInverted(invertedCheck_->isChecked());
}

CQColorsPalette::ColorType
CQColorsEditControl::
colorType() const
{
  auto *pal = canvas_->palette();
  if (! pal) return CQColorsPalette::ColorType::NONE;

  return pal->colorType();
}

void
CQColorsEditControl::
setColorType(CQColorsPalette::ColorType colorType)
{
  auto *pal = canvas_->palette();
  if (! pal) return;

  pal->setColorType(colorType);

  updateColorType();

  emit stateChanged();
}

bool
CQColorsEditControl::
isDistinct() const
{
  auto *pal = canvas_->palette();
  if (! pal) return false;

  return pal->isDistinct();
}

void
CQColorsEditControl::
setDistinct(bool b)
{
  auto *pal = canvas_->palette();
  if (! pal) return;

  pal->setDistinct(b);

  emit stateChanged();
}

bool
CQColorsEditControl::
isInverted() const
{
  auto *pal = canvas_->palette();
  if (! pal) return false;

  return pal->isInverted();
}

void
CQColorsEditControl::
setInverted(bool b)
{
  auto *pal = canvas_->palette();
  if (! pal) return;

  pal->setInverted(b);

  emit stateChanged();
}

void
CQColorsEditControl::
updateColorType()
{
  auto colorType = this->colorType();

  int ind = 0;

  if      (colorType == CQColorsPalette::ColorType::DEFINED  ) ind = 0;
  else if (colorType == CQColorsPalette::ColorType::MODEL    ) ind = 1;
  else if (colorType == CQColorsPalette::ColorType::FUNCTIONS) ind = 2;
  else if (colorType == CQColorsPalette::ColorType::CUBEHELIX) ind = 3;

  if (colorType_)
    colorType_->setCurrentIndex(ind);

  stack_->setCurrentIndex(ind);
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
  auto *pal = canvas_->palette();
  if (! pal) return CQColorsPalette::ColorModel::NONE;

  return pal->colorModel();
}

void
CQColorsEditControl::
setColorModel(CQColorsPalette::ColorModel colorModel)
{
  auto *pal = canvas_->palette();
  if (! pal) return;

  pal->setColorModel(colorModel);

  updateColorModel();

  emit stateChanged();
}

void
CQColorsEditControl::
updateColorModel()
{
  auto colorModel = this->colorModel();

  if      (colorModel == CQColorsPalette::ColorModel::RGB) {
    if (colorModel_)
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
    if (colorModel_)
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
  auto *me = qobject_cast<CQColorsEditModel *>(sender());
  assert(me);

  auto *pal = canvas_->palette();
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
  auto *pal = canvas_->palette();
  if (! pal) return;

  pal->setRedNegative(state);

  emit stateChanged();
}

void
CQColorsEditControl::
modelGNegativeChecked(int state)
{
  auto *pal = canvas_->palette();
  if (! pal) return;

  pal->setGreenNegative(state);

  emit stateChanged();
}

void
CQColorsEditControl::
modelBNegativeChecked(int state)
{
  auto *pal = canvas_->palette();
  if (! pal) return;

  pal->setBlueNegative(state);

  emit stateChanged();
}

void
CQColorsEditControl::
modelRangeValueChanged(double r)
{
  auto *rs = qobject_cast<CQRealSpin *>(sender());
  assert(rs);

  auto *pal = canvas_->palette();
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
  auto *pal = canvas_->palette();
  if (! pal) return;

  pal->setCubeNegative(state);

  emit stateChanged();
}

void
CQColorsEditControl::
colorsChanged()
{
  auto *pal = canvas_->palette();
  if (! pal) return;

  pal->resetDefinedColors();

  for (size_t i = 0; i < definedColors_->numRealColors(); ++i) {
    const auto &realColor = definedColors_->realColor(CUtil::toInt(i));

    double r = realColor.r;

    while (pal->isDefinedColor(r))
      r += 0.01;

    pal->addDefinedColor(r, realColor.c);
  }

  emit stateChanged();
}

void
CQColorsEditControl::
addColorSlot()
{
  auto *pal = canvas_->palette();
  if (! pal) return;

  int row = 0;

  auto selectedItems = definedColors_->selectedItems();

  if (selectedItems.length())
    row = selectedItems[0]->row();

  const auto &colors = pal->definedValueColors();

  double x = 0.5;
  auto   c = QColor(127, 127, 127);

  auto nc = CUtil::toInt(colors.size());

  int row1 = -1;

  if      (row + 1 < nc)
    row1 = row;
  else if (row < nc && row > 0)
    row1 = row - 1;

  if (row1 >= 0) {
    auto p = colors.begin();

    std::advance(p, row1);

    double      x1 = (*p).first;
    const auto &c1 = (*p).second;

    ++p;
    double      x2 = (*p).first;
    const auto &c2 = (*p).second;

    x = (x1 + x2)/2;

    if (pal->colorModel() == CQColorsPalette::ColorModel::HSV)
      c = CQColorsPalette::interpHSV(c1, c2, 0.5);
    else
      c = CQColorsPalette::interpRGB(c1, c2, 0.5);
  }
  else {
    if (nc == 1) {
      auto p = colors.begin();

      double      x1 = (*p).first;
      const auto &c1 = (*p).second;

      if (x1 != 1.0)
        x = (x1 + 1.0)/2;

      auto c2 = QColor(255, 255, 255);

      if (pal->colorModel() == CQColorsPalette::ColorModel::HSV)
        c = CQColorsPalette::interpHSV(c1, c2, 0.5);
      else
        c = CQColorsPalette::interpRGB(c1, c2, 0.5);
    }
  }

  pal->addDefinedColor(x, c);

  definedColors_->updateColors(pal);

  emit stateChanged();
}

void
CQColorsEditControl::
removeColorSlot()
{
  auto *pal = canvas_->palette();
  if (! pal) return;

  auto selectedItems = definedColors_->selectedItems();

  if (! selectedItems.length())
    return;

  int row = selectedItems[0]->row();

  auto colors = pal->definedValueColors();

  CQColorsPalette::ColorMap colors1;

  int i = 0;

  for (const auto &pc : colors) {
    if (i != row)
      colors1[pc.first] = pc.second;

    ++i;
  }

  pal->setDefinedColors(colors1);

  definedColors_->updateColors(pal);

  emit stateChanged();
}

void
CQColorsEditControl::
loadColorsSlot()
{
  auto dir = QDir::current().dirName();

  auto fileName = QFileDialog::getOpenFileName(this, "Open Palette File", dir, "Files (*.pal)");

  if (! fileName.length())
    return;

  readFile(fileName);

  emit stateChanged();
}

void
CQColorsEditControl::
saveColorsSlot()
{
  auto dir = QDir::current().dirName() + "/colors.pal";

  auto fileName = QFileDialog::getSaveFileName(this, "Save Palette File", dir, "Files (*.pal)");

  if (! fileName.length())
    return;

  saveFile(fileName);
}

void
CQColorsEditControl::
readFile(const QString &fileName)
{
  auto *pal = canvas_->palette();
  if (! pal) return;

  pal->readFile(fileName.toStdString());

  definedColors_->updateColors(pal);

  if (distinctCheck_)
    distinctCheck_->setChecked(pal->isDistinct());

  if (invertedCheck_)
    invertedCheck_->setChecked(pal->isInverted());
}

void
CQColorsEditControl::
saveFile(const QString &fileName)
{
  auto *pal = canvas_->palette();
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
  auto *le = qobject_cast<CQLineEdit *>(sender());
  assert(le);

  auto *pal = canvas_->palette();
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
  auto *rs = qobject_cast<CQRealSpin *>(sender());
  assert(rs);

  auto *pal = canvas_->palette();
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
  auto *frame  = CQUtil::makeWidget<QFrame>("frame");
  auto *layout = CQUtil::makeLayout<QHBoxLayout>(frame, 2, 2);

  *type = new CQColorsEditColorType;

  auto *colorLabel = CQUtil::makeLabelWidget<QLabel>(label, "label");

  layout->addWidget(colorLabel);
  layout->addWidget(*type);
  layout->addStretch(1);

  return frame;
}

QFrame *
CQColorsEditControl::
createColorModelCombo(const QString &label, CQColorsEditColorModel **model)
{
  auto *frame  = CQUtil::makeWidget<QFrame>("frame");
  auto *layout = CQUtil::makeLayout<QHBoxLayout>(frame, 2, 2);

  *model = new CQColorsEditColorModel;

  (*model)->setToolTip("Color Model");

  auto *colorLabel = CQUtil::makeLabelWidget<QLabel>(label, "label");

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

  auto *editLabel = CQUtil::makeLabelWidget<QLabel>(label, "label");

  grid->addWidget(editLabel, row, col);
  grid->addWidget(*edit    , row, col + 1);

  if (stretch)
    grid->setColumnStretch(col + 2, true);
}
