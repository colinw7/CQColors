#ifndef CQColorsTheme_H
#define CQColorsTheme_H

#include <QObject>
#include <cassert>

class CQColorsPalette;

//! \brief theme (ordered set of named palettes)
class CQColorsTheme : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString name READ name)
  Q_PROPERTY(QString desc READ desc WRITE setDesc)

 public:
  CQColorsTheme();
 ~CQColorsTheme();

  //! get/set theme name
  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  //! get/set theme description
  const QString &desc() const { return desc_; }
  void setDesc(const QString &s) { desc_ = s; }

  //! number of palettes
  int numPalettes() const { return palettes_.size(); }

  //! get/set nth palette
  CQColorsPalette *palette(int i=0) const;
  void setPalette(int i, CQColorsPalette *palette);

  //! set named palettes
  void setNamedPalettes(const QStringList &names);

  //! add named palette
  void addNamedPalette(const QString &name);

  //! check if has named palette
  bool hasNamedPalette(const QString &name) const;

  //! remove named palette
  void removeNamedPalette(const QString &name);

  //! set nth palette to palette of specified name
  void setNamedPalette(int i, const QString &name);

  //! move palette to new position
  void moveNamedPalette(const QString &name, int pos);

  //! get position of named palette
  int paletteInd(const QString &name) const;

#if 0
  //! shift palettes by n places
  void shiftPalettes(int n);
#endif

 signals:
  void themeChanged();

 protected:
  //! initialize with all named palettes
  void addNamedPalettes();

 protected:
  using Palettes = std::vector<CQColorsPalette*>;

  QString  name_;     //!< theme name
  QString  desc_;     //!< theme description
  Palettes palettes_; //!< theme palette list
};

#endif
