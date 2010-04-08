/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtStatusWidget.h"

#include <QBoxLayout>
#include <QComboBox>
#include <QLineEdit>


namespace Swift {
QtStatusWidget::QtStatusWidget(QWidget *parent) : QWidget(parent) {
	types_ = new QComboBox(this);
	types_->addItem(QIcon(":/icons/online.png"), "Available", QVariant(StatusShow::Online));
	types_->addItem(QIcon(":/icons/online.png"), "Free For Chat", QVariant(StatusShow::FFC));
	types_->addItem(QIcon(":/icons/away.png"), "Away", QVariant(StatusShow::Away));
	types_->addItem(QIcon(":/icons/away.png"), "Extended Away", QVariant(StatusShow::XA));
	types_->addItem(QIcon(":/icons/dnd.png"), "Do Not Disturb", QVariant(StatusShow::DND));
	types_->addItem(QIcon(":/icons/offline.png"), "Offline", QVariant(StatusShow::None));
	connect(types_, SIGNAL(activated(int)), this, SLOT(handleTypeSelected(int)));
	QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->setSpacing(0);
	mainLayout->addWidget(types_);
}

void QtStatusWidget::handleTypeSelected(int index) {
	Q_UNUSED(index);
	emit onChangeStatusRequest(getSelectedStatusShow());
}

StatusShow::Type QtStatusWidget::getSelectedStatusShow() {
	return (StatusShow::Type)types_->itemData(types_->currentIndex()).toInt();
}

void QtStatusWidget::setStatusType(StatusShow::Type type) {
	for (int i = 0; i < types_->count(); i++) {
		if (types_->itemData(i).toInt() == type) {
			types_->setCurrentIndex(i);
			break;
		}
	}
}

}




