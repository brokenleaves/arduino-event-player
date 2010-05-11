/*
 * EventPlayer.cpp
 */
 
#include "WProgram.h"
#include "EventPlayer.h"

Event::Event(EventType type, unsigned long timeMillis) {
	this->type = type;
	this->timeMillis = timeMillis;
}

EventType Event::getType() {
	return type;
}

unsigned long Event::getTime() {
	return timeMillis;
}

void Event::run(unsigned long playerTimeMillis) {
	// empty
}

OnetimeEvent::OnetimeEvent(unsigned long timeMillis) : Event(ONETIME, timeMillis) {
	// empty
}

ContinuousEvent::ContinuousEvent(unsigned long timeMillis, unsigned long lengthMillis) : Event(CONTINUOUS, timeMillis) {
	this->lengthMillis = lengthMillis;
}

void ContinuousEvent::init(unsigned long playerTimeMillis) {
	// empty
}

unsigned long ContinuousEvent::getLength() {
	return lengthMillis;
}

unsigned long ContinuousEvent::getEndTime() {
	return timeMillis + lengthMillis;
}

JumpEvent::JumpEvent(unsigned long timeMillis, short eventIndex, unsigned char loopCount) : Event(JUMP, timeMillis) {
	this->absolute = false;
	this->eventIndex = eventIndex;
	this->loopCount = loopCount;
	this->loopsExecuted = 0;
}

JumpEvent::JumpEvent(unsigned long timeMillis, short eventIndex, unsigned char loopCount, boolean absolute) : Event(JUMP, timeMillis) {
	this->absolute = absolute;
	this->eventIndex = eventIndex;
	this->loopCount = loopCount;
	this->loopsExecuted = 0;
}

boolean JumpEvent::isAbsolute() {
	return absolute;
}

short JumpEvent::getEventIndex() {
	return eventIndex;
}

unsigned char JumpEvent::getLoopCount() {
	return loopCount;
}

unsigned char JumpEvent::getLoopsExecuted() {
	return loopsExecuted;
}

void JumpEvent::setLoopsExecuted(unsigned char loopsExecuted) {
	this->loopsExecuted = loopsExecuted;
}

EventPlayer::EventPlayer(unsigned short maxEventCount, unsigned char maxContinuousEventCount) {
	this->eventCount = maxEventCount;
	this->maxContinuousEventCount = maxContinuousEventCount;
	this->currentEventIndex = 0;
	this->firstFreeEventIndex = 0;
	this->events = new Event*[maxEventCount];
	this->continuousEvents = new ContinuousEvent*[maxContinuousEventCount];
	for (unsigned char i = 0; i < maxContinuousEventCount; i++) {
		this->continuousEvents[i] = NULL;
	}
}

EventPlayer::~EventPlayer() {
	delete[] this->events;
}

void EventPlayer::add(Event *event) {
	this->events[firstFreeEventIndex] = event;
	firstFreeEventIndex++;
}

void EventPlayer::prepare() {
	// TODO: sort array according to event start times
}

void EventPlayer::start(unsigned long timeMicros) {
	this->startTimeMicros = timeMicros;
}

void EventPlayer::run(unsigned long timeMicros) {
	unsigned long playerTimeMicros = timeMicros - startTimeMicros;
	unsigned long playerTimeMillis = playerTimeMicros / 1000;

	if (currentEventIndex < firstFreeEventIndex) {
		Event *currentEvent = events[currentEventIndex];
		while (currentEvent->getTime() <= playerTimeMillis) {
			EventType type = currentEvent->getType();

			if (type == JUMP) {
				JumpEvent *jumpEvent = (JumpEvent *) currentEvent;

				unsigned short newCurrentEventIndex;
				if (jumpEvent->isAbsolute()) {
					newCurrentEventIndex = jumpEvent->getEventIndex();
				} else {
					newCurrentEventIndex = currentEventIndex + jumpEvent->getEventIndex();
				}
				if (newCurrentEventIndex >= firstFreeEventIndex) {
					break;
				}

				unsigned char loopCount = jumpEvent->getLoopCount();
				if (loopCount > 0) {
					unsigned char loopsExecuted = jumpEvent->getLoopsExecuted();
					if (loopsExecuted >= loopCount) {
						continue;
					}
					jumpEvent->setLoopsExecuted(loopsExecuted + 1);
				} else if (loopCount == 0) {
					continue;
				}

				currentEventIndex = newCurrentEventIndex;
				currentEvent = events[currentEventIndex];
				startTimeMicros = startTimeMicros + (jumpEvent->getTime() * 1000);
				playerTimeMicros = timeMicros - startTimeMicros;
				playerTimeMillis = playerTimeMicros / 1000;

				continue;
			} else if (type == CONTINUOUS) {
				// Add to list of continuous events
				for (unsigned char i = 0; i < maxContinuousEventCount; i++) {
					if (continuousEvents[i] == NULL) {
						// A free slot for the continuous event was found
						continuousEvents[i] = (ContinuousEvent *) currentEvent;
						// Initialize event
						continuousEvents[i]->init(playerTimeMillis);
						break;
					}
				}
			} else {
				// Execute all onetime events
				currentEvent->run(playerTimeMillis);
			}
			currentEventIndex++;
			if (currentEventIndex >= firstFreeEventIndex) {
				break;
			}
			currentEvent = events[currentEventIndex];
		}
	}

	// Execute all continuous events
	for (unsigned char i = 0; i < maxContinuousEventCount; i++) {
		ContinuousEvent *event = continuousEvents[i];
		if (event != NULL) {
			long endTimeMillis = event->getEndTime();
			if (endTimeMillis <= playerTimeMillis) {
				// Event is over
				event->run(endTimeMillis);
				continuousEvents[i] = NULL;
			} else {
				event->run(playerTimeMillis);
			}
		}
	}
}
