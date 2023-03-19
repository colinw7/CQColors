#ifndef CQColorsEditModel_H
#define CQColorsEditModel_H

#include <QComboBox>

//! \brief gradient palette model combo
class CQColorsEditModel : public QComboBox {
  Q_OBJECT

 public:
  CQColorsEditModel(QWidget *parent=nullptr);
};

#endif
