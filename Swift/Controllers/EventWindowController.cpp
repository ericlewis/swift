/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/EventWindowController.h"

#include <boost/bind.hpp>

namespace Swift {

EventWindowController::EventWindowController(EventController* eventController, EventWindowFactory* windowFactory) {
	eventController_ = eventController;
	windowFactory_ = windowFactory;
	window_ = windowFactory_->createEventWindow();
	eventAddedConnection_ = eventController_->onEventQueueEventAdded.connect(boost::bind(&EventWindowController::handleEventQueueEventAdded, this, _1));
}

EventWindowController::~EventWindowController() {
	delete window_;
}

void EventWindowController::handleEventQueueEventAdded(boost::shared_ptr<StanzaEvent> event) {
	event->onConclusion.connect(boost::bind(&EventWindowController::handleEventConcluded, this, event));
	window_->addEvent(event, true);
}

void EventWindowController::handleEventConcluded(boost::shared_ptr<StanzaEvent> event) {
	window_->removeEvent(event);
	window_->addEvent(event, false);
}

}
