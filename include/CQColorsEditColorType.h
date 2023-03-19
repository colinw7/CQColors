#ifndef CQColorsEditColorType_H
#define CQColorsEditColorType_H

#include <CQColorsPalette.h>
#include <QComboBox>

//! \brief gradient palette color type combo
class CQColorsEditColorType : public QComboBox {
  Q_OBJECT

 public:
  CQColorsEditColorType(QWidget *parent=nullptr);

  CQColorsPalette::ColorType type() const;

  void setType(const CQColorsPalette::ColorType &type);
};

#endif
