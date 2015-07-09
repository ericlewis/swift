/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/EventViewer/EventDelegate.h>

#include <QDebug>

#include <Swift/Controllers/XMPPEvents/ErrorEvent.h>
#include <Swift/Controllers/XMPPEvents/IncomingFileTransferEvent.h>
#include <Swift/Controllers/XMPPEvents/MUCInviteEvent.h>
#include <Swift/Controllers/XMPPEvents/MessageEvent.h>
#include <Swift/Controllers/XMPPEvents/SubscriptionRequestEvent.h>

namespace Swift {

EventDelegate::EventDelegate() : QStyledItemDelegate(),
	messageDelegate_(QtEvent::SenderRole, Qt::DisplayRole, false),
	subscriptionDelegate_(QtEvent::SenderRole, Qt::DisplayRole, true),
	errorDelegate_(QtEvent::SenderRole, Qt::DisplayRole, true),
	mucInviteDelegate_(QtEvent::SenderRole, Qt::DisplayRole, false),
	incomingFileTransferDelegate_(QtEvent::SenderRole, Qt::DisplayRole, false) {

}

QSize EventDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	QtEvent* item = static_cast<QtEvent*>(index.internalPointer());
	if (!item) {
		return QStyledItemDelegate::sizeHint(option, index);
	}
	switch (getEventType(item->getEvent())) {
		case MessageEventType: return messageDelegate_.sizeHint(option, item);
		case SubscriptionEventType: return subscriptionDelegate_.sizeHint(option, item);
		case ErrorEventType: return errorDelegate_.sizeHint(option, item);
		case MUCInviteEventType: return mucInviteDelegate_.sizeHint(option, item);
		case IncomingFileTransferEventType: return incomingFileTransferDelegate_.sizeHint(option, item);
	}
	assert(false);
	return QSize();
}

void EventDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	QtEvent* item = static_cast<QtEvent*>(index.internalPointer());
	if (!item) {
		QStyledItemDelegate::paint(painter, option, index);
		return;
	}
	switch (getEventType(item->getEvent())) {
		case MessageEventType: messageDelegate_.paint(painter, option, item);break;
		case SubscriptionEventType: subscriptionDelegate_.paint(painter, option, item);break;
		case ErrorEventType: errorDelegate_.paint(painter, option, item);break;
		case MUCInviteEventType: mucInviteDelegate_.paint(painter, option, item);break;
		case IncomingFileTransferEventType: incomingFileTransferDelegate_.paint(painter, option, item);break;
	}
}

EventType EventDelegate::getEventType(boost::shared_ptr<StanzaEvent> event) const {
	boost::shared_ptr<MessageEvent> messageEvent = boost::dynamic_pointer_cast<MessageEvent>(event);
	if (messageEvent) {
		return MessageEventType;
	}
	boost::shared_ptr<SubscriptionRequestEvent> subscriptionEvent = boost::dynamic_pointer_cast<SubscriptionRequestEvent>(event);
	if (subscriptionEvent) {
		return SubscriptionEventType;
	}
	boost::shared_ptr<ErrorEvent> errorEvent = boost::dynamic_pointer_cast<ErrorEvent>(event);
	if (errorEvent) {
		return ErrorEventType;
	}
	boost::shared_ptr<MUCInviteEvent> mucInviteEvent = boost::dynamic_pointer_cast<MUCInviteEvent>(event);
	if (mucInviteEvent) {
		return MUCInviteEventType;
	}
	boost::shared_ptr<IncomingFileTransferEvent> incomingFileTransferEvent = boost::dynamic_pointer_cast<IncomingFileTransferEvent>(event);
	if (incomingFileTransferEvent) {
		return IncomingFileTransferEventType;
	}
	//I don't know what this is.
	assert(false);
	return MessageEventType;
}

}

