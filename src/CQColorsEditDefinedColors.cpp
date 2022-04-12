#include <CQColorsEditDefinedColors.h>

#include <CQRealSpin.h>
#include <CQColorChooser.h>
#include <CQUtil.h>
#include <CSafeIndex.h>

#include <QHeaderView>
#include <QItemDelegate>
#include <QPainter>
#include <cassert>

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

  auto *delegate = new CQColorsEditDefinedColorsDelegate(this);

  setItemDelegate(delegate);
  setEditTriggers(QAbstractItemView::AllEditTriggers);

  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);

  auto *header = horizontalHeader();

  header->setStretchLastSection(true);

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

  for (int r = 0; r < CUtil::toInt(numRealColors()); ++r) {
    const auto &realColor = this->realColor(r);

    auto *item1 = new QTableWidgetItem(QString("%1").arg(realColor.r));
    auto *item2 = new QTableWidgetItem(realColor.c.name());

    setItem(r, 0, item1);
    setItem(r, 1, item2);
  }
}

const CQColorsEditDefinedColors::RealColor &
CQColorsEditDefinedColors::
realColor(int r) const
{
  return CUtil::safeIndex(realColors_, r);
}

void
CQColorsEditDefinedColors::
setRealColor(int r, const RealColor &realColor)
{
  CUtil::safeIndex(realColors_, r) = realColor;

  //auto *item1 = new QTableWidgetItem(QString("%1").arg(realColor.r));
  //auto *item2 = new QTableWidgetItem(realColor.c.name());

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
  auto *item = colors_->item(ind.row(), ind.column());
  assert(item);

  const auto &realColor = colors_->realColor(ind.row());

  if       (ind.column() == 0) {
    auto *edit = CQUtil::makeWidget<CQRealSpin>(parent, "spin");

    edit->setValue(realColor.r);

    return edit;
  }
  else if (ind.column() == 1) {
    auto *edit = CQUtil::makeWidget<CQColorChooser>(parent, "color");

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
  auto *item = colors_->item(ind.row(), ind.column());
  assert(item);

  const auto &realColor = colors_->realColor(ind.row());

  if       (ind.column() == 0) {
    auto *edit = qobject_cast<CQRealSpin *>(w);

    edit->setValue(realColor.r);
  }
  else if (ind.column() == 1) {
    auto *edit = qobject_cast<CQColorChooser *>(w);

    const auto &c = realColor.c;

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
  auto *item = colors_->item(ind.row(), ind.column());
  assert(item);

  auto realColor = colors_->realColor(ind.row());

  if       (ind.column() == 0) {
    auto *edit = qobject_cast<CQRealSpin *>(w);

    double r = edit->value();

    model->setData(ind, QVariant(r));

    realColor.r = r;
  }
  else if (ind.column() == 1) {
    auto *edit = qobject_cast<CQColorChooser *>(w);

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
  auto *item = colors_->item(ind.row(), ind.column());
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
  auto *item = colors_->item(ind.row(), ind.column());
  assert(item);

  if (ind.column() == 1) {
    const auto &realColor = colors_->realColor(ind.row());

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

  auto rect = option.rect;

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
