#ifndef CQColorsEditColorModel_H
#define CQColorsEditColorModel_H

#include <CQColorsPalette.h>
#include <QComboBox>

//! \brief gradient palette color model combo
class CQColorsEditColorModel : public QComboBox {
  Q_OBJECT

 public:
  CQColorsEditColorModel(QWidget *parent=nullptr);

  CQColorsPalette::ColorModel model() const;

  void setModel(const CQColorsPalette::ColorModel &model);
};

#endif
