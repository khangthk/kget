/* This file is part of the KDE libraries
    Copyright (C) 2006,2007 Andreas Hartmetz (ahartmetz@gmail.com)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/


#ifndef KEXTENDABLEITEMDELEGATE_H
#define KEXTENDABLEITEMDELEGATE_H

#include <QItemDelegate>
#include <QHash>

/**
This delegate makes it possible to display an arbitrary QWidget ("extender") that spans all columns below a line of items.
The extender will logically belong to a column in the row above it.


It is your responsibility to devise a way to trigger extension and contraction of items, by calling
extendItem() and contractItem(). You can e.g. reimplement itemActivated() and similar functions.
*/


class QAbstractItemView;
class KExtendableItemDelegatePrivate;

class KExtendableItemDelegate : public QItemDelegate {
	Q_OBJECT

public:
	enum auxDataRoles {ShowExtensionIndicatorRole = Qt::UserRole + 200};

	/**
	 * Create a new KExtendableItemDelegate that belongs to @p parent. In contrast to generic
	 * QAbstractItemDelegates, an instance of this class can only ever be the delegate for one
	 * instance of af QAbstractItemView subclass.
	 */
	KExtendableItemDelegate(QAbstractItemView *parent);
	virtual ~KExtendableItemDelegate();

	//reimplemented from QItemDelegate
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	
	/**
	 * Insert the @p extender that logically belongs to @p index into the view.
	 * If you need a parent for the extender at construction time, use the itemview's viewport().
	 * The extender will be reparented and resized to the viewport by this function.
	 */
	void extendItem(QWidget *extender, const QModelIndex &index);

    /**
     * Remove the extender that logically belongs to @p index from the view.
	 */
	void contractItem(const QModelIndex &index);

	/**
	 * Return whether there is an extender that belongs to @p index.
	 */
	bool isExtended(const QModelIndex &index) const;
	
	/**
	 * Reimplement this function to adjust the internal geometry of the extender.
	 * The external geometry of the extender will be set by the delegate.
	 */
	virtual void updateExtenderGeometry(QWidget *extender, const QStyleOptionViewItem &option, const QModelIndex &index) const;

Q_SIGNALS:
	/**
	 * This signal indicates that the item at @p index was extended with @p extender.
	 */
	void extenderCreated(QWidget *extender, const QModelIndex &index);

	/**
	 * This signal indicates that the @p extender belonging to @p index has emitted the destroyed() signal.
	 */
	void extenderDestroyed(QWidget *extender, const QModelIndex &index);

protected:
	/**
	 * Reimplement this function to fine-tune the position of the extender. @p option.rect will be a rectangle
	 * that is as wide as the viewport and as high as the usual item height plus the extender size hint's height.
	 * Its upper left corner will be at the upper left corner of the usual item.
	 * You can place the returned rectangle of this function anywhere inside that area.
	 */
	QRect extenderRect(QWidget *extender, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	//these two must have the same (screen) size!
	void setExtendIcon(const QPixmap &);
	void setContractIcon(const QPixmap &);
	QPixmap extendIcon();
	QPixmap contractIcon();

private Q_SLOTS:
	void extenderDestructionHandler(QObject *destroyed);

private:
	inline QSize maybeExtendedSize(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QModelIndex indexOfExtendedColumnInSameRow(const QModelIndex &index) const;
	inline void scheduleUpdateViewLayout() const;
	
	KExtendableItemDelegatePrivate *const d;
};
#endif // KEXTENDABLEITEMDELEGATE_H
