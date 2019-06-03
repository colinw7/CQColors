#ifndef CQColorsEditControl_H
#define CQColorsEditControl_H

#include <CQColors.h>
#include <CQColorsPalette.h>
#include <QFrame>
#include <QComboBox>
#include <QTableWidget>

class CQColorsEditCanvas;
class CQColorsEditColorType;
class CQColorsEditColorModel;
class CQColorsEditModel;
class CQColorsEditDefinedColors;
class CQRealSpin;
class CQLineEdit;

class QStackedWidget;
class QPushButton;
class QCheckBox;
class QLabel;
class QGridLayout;

/*!
 * \brief Controls for colors edit customization
 */
class CQColorsEditControl : public QFrame {
  Q_OBJECT

  Q_PROPERTY(int redModel   READ redModel   WRITE setRedModel  )
  Q_PROPERTY(int greenModel READ greenModel WRITE setGreenModel)
  Q_PROPERTY(int blueModel  READ blueModel  WRITE setBlueModel )

 public:
  CQColorsEditControl(CQColorsEditCanvas *canvas);

  //! get/set color type
  CQColorsPalette::ColorType colorType() const;
  void setColorType(CQColorsPalette::ColorType colorType);

  //! get/set color model
  CQColorsPalette::ColorModel colorModel() const;
  void setColorModel(CQColorsPalette::ColorModel colorModel);

  //! get/set distinct
  bool isDistinct() const;
  void setDistinct(bool b);

  //! get/set red model
  int redModel() const;
  void setRedModel(int mode);

  //! get/set green model
  int greenModel() const;
  void setGreenModel(int mode);

  //! get/set blue model
  int blueModel() const;
  void setBlueModel(int mode);

  //! get/set red function
  std::string redFunction() const;
  void setRedFunction(const std::string &fn);

  //! get/set green function
  std::string greenFunction() const;
  void setGreenFunction(const std::string &fn);

  //! get/set blue function
  std::string blueFunction() const;
  void setBlueFunction(const std::string &fn);

  double cubeStart() const;
  void setCubeStart(double r);

  double cubeCycles() const;
  void setCubeCycles(double r);

  double cubeSaturation() const;
  void setCubeSaturation(double r);

  void readFile(const QString &fileName);
  void saveFile(const QString &fileName);

 signals:
  void stateChanged();

 public slots:
  void updateState();

 private slots:
  void colorTypeChanged(int);
  void colorModelChanged(int);

  void distinctChanged(int);

  void modelChanged(int);

  void modelRNegativeChecked(int state);
  void modelGNegativeChecked(int state);
  void modelBNegativeChecked(int state);

  void modelRangeValueChanged(double v);

  void cubeNegativeChecked(int state);

  void colorsChanged();

  void addColorSlot();
  void removeColorSlot();

  void loadColorsSlot();
  void saveColorsSlot();

  void functionChanged();
  void cubeValueChanged(double r);

 private:
  void updateColorType ();
  void updateColorModel();

  QFrame *createColorTypeCombo(const QString &label, CQColorsEditColorType **type);

  QFrame *createColorModelCombo(const QString &label, CQColorsEditColorModel **model);

  void createModelCombo(QGridLayout *grid, int row, const QString &label,
                        QLabel **modelLabel, CQColorsEditModel **modelCombo);

  void createFunctionEdit(QGridLayout *grid, int row, const QString &label,
                          QLabel **functionLabel, CQLineEdit **functionEdit);

  void createRealEdit(QGridLayout *grid, int row, const QString &label, CQRealSpin **edit);

  void createRealEdit(QGridLayout *grid, int row, int col, bool stretch,
                      const QString &label, CQRealSpin **edit);

 private:
  CQColorsEditCanvas*        canvas_              { nullptr };
  CQColorsEditColorType*     colorType_           { nullptr };
  CQColorsEditColorModel*    colorModel_          { nullptr };
  QCheckBox*                 distinctCheck_       { nullptr };
  QStackedWidget*            stack_               { nullptr };
  QLabel*                    redModelLabel_       { nullptr };
  CQColorsEditModel*         redModelCombo_       { nullptr };
  QLabel*                    greenModelLabel_     { nullptr };
  CQColorsEditModel*         greenModelCombo_     { nullptr };
  QLabel*                    blueModelLabel_      { nullptr };
  CQColorsEditModel*         blueModelCombo_      { nullptr };
  QCheckBox*                 modelRNegativeCheck_ { nullptr };
  QCheckBox*                 modelGNegativeCheck_ { nullptr };
  QCheckBox*                 modelBNegativeCheck_ { nullptr };
  QLabel*                    redMinMaxLabel_      { nullptr };
  QLabel*                    greenMinMaxLabel_    { nullptr };
  QLabel*                    blueMinMaxLabel_     { nullptr };
  CQRealSpin*                redMin_              { nullptr };
  CQRealSpin*                redMax_              { nullptr };
  CQRealSpin*                greenMin_            { nullptr };
  CQRealSpin*                greenMax_            { nullptr };
  CQRealSpin*                blueMin_             { nullptr };
  CQRealSpin*                blueMax_             { nullptr };
  QLabel*                    redFunctionLabel_    { nullptr };
  QLabel*                    greenFunctionLabel_  { nullptr };
  QLabel*                    blueFunctionLabel_   { nullptr };
  CQLineEdit*                redFunctionEdit_     { nullptr };
  CQLineEdit*                greenFunctionEdit_   { nullptr };
  CQLineEdit*                blueFunctionEdit_    { nullptr };
  CQColorsEditDefinedColors* definedColors_       { nullptr };
  QPushButton*               addColorButton_      { nullptr };
  QPushButton*               removeColorButton_   { nullptr };
  QPushButton*               loadColorsButton_    { nullptr };
  QPushButton*               saveColorsButton_    { nullptr };
  CQRealSpin*                cubeStart_           { nullptr };
  CQRealSpin*                cubeCycles_          { nullptr };
  CQRealSpin*                cubeSaturation_      { nullptr };
  QCheckBox*                 cubeNegativeCheck_   { nullptr };
};

//---

//! \brief gradient palette color type combo
class CQColorsEditColorType : public QComboBox {
  Q_OBJECT

 public:
  CQColorsEditColorType(QWidget *parent=0);

  CQColorsPalette::ColorType type() const;

  void setType(const CQColorsPalette::ColorType &type);
};

//---

//! \brief gradient palette color model combo
class CQColorsEditColorModel : public QComboBox {
  Q_OBJECT

 public:
  CQColorsEditColorModel(QWidget *parent=0);

  CQColorsPalette::ColorModel model() const;

  void setModel(const CQColorsPalette::ColorModel &model);
};

//---

//! \brief gradient palette model combo
class CQColorsEditModel : public QComboBox {
  Q_OBJECT

 public:
  CQColorsEditModel(QWidget *parent=0);
};

//---

//! \brief gradient palette defined colors table
class CQColorsEditDefinedColors : public QTableWidget {
  Q_OBJECT

 public:
  struct RealColor {
    RealColor(double r1, const QColor &c1) :
     r(r1), c(c1) {
    }

    double r { 0.0 };
    QColor c { 0, 0, 0 };
  };

  using RealColors = std::vector<RealColor>;

 public:
  CQColorsEditDefinedColors(QWidget *parent=0);

  void updateColors(CQColorsPalette *palette);

  int numRealColors() const { return realColors_.size(); }

  const RealColor &realColor(int i) const;

  void setRealColor(int i, const RealColor &c);

 signals:
  void colorsChanged();

 private:
  RealColors realColors_;
};

#endif
