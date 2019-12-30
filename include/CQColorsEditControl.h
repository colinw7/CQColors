#ifndef CQColorsEditControl_H
#define CQColorsEditControl_H

#include <CQColors.h>
#include <CQColorsPalette.h>
#include <QFrame>
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
  void updateDefinedState();

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

#endif
