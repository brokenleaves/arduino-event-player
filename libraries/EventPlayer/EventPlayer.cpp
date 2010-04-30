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
	this->endTimeMillis = timeMillis + lengthMillis;
}

unsigned long ContinuousEvent::getLength() {
	return lengthMillis;
}

unsigned long ContinuousEvent::getEndTime() {
	return endTimeMillis;
}

JumpEvent::JumpEvent(unsigned long timeMillis, unsigned short eventIndex, unsigned short loopCount) : Event(JUMP, timeMillis) {
	this->eventIndex = eventIndex;
	this->loopCount = loopCount;
	this->loopsExecuted = 0;
}

unsigned short JumpEvent::getEventIndex() {
	return eventIndex;
}

unsigned short JumpEvent::getLoopCount() {
	return loopCount;
}

unsigned short JumpEvent::getLoopsExecuted() {
	return loopsExecuted;
}

void JumpEvent::setLoopsExecuted(unsigned short loopsExecuted) {
	this->loopsExecuted = loopsExecuted;
}

EventPlayer::EventPlayer(unsigned short maxEventCount, unsigned char maxContinuousEventCount) {
	this->eventCount = maxEventCount;
	this->maxContinuousEventCount = maxContinuousEventCount;
	this->currentEventIndex = 0;
	this->firstFreeEventIndex = 0;
	this->events = new Event*[maxEventCount];
	this->continuousEvents = new Event*[maxContinuousEventCount];
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

				unsigned short newCurrentEventIndex = jumpEvent->getEventIndex();
				if (newCurrentEventIndex >= firstFreeEventIndex) {
					break;
				}

				unsigned short loopCount = jumpEvent->getLoopCount();
				if (loopCount > 0) {
					unsigned short loopsExecuted = jumpEvent->getLoopsExecuted();
					if (loopsExecuted >= loopCount) {
						continue;
					}
					jumpEvent->setLoopsExecuted(loopsExecuted + 1);
				} else if (loopCount == 0) {
					continue;
				}

				currentEventIndex = newCurrentEventIndex;
				currentEvent = events[currentEventIndex];
				startTimeMicros = timeMicros - (currentEvent->getTime() * 1000);
				playerTimeMicros = timeMicros - startTimeMicros;
				playerTimeMillis = playerTimeMicros / 1000;

				Serial.print("JUMP: currentEventIndex: ");
				Serial.print(currentEventIndex, DEC);
				Serial.print(" startTimeMicros: ");
				Serial.println(startTimeMicros, DEC);
				continue;
			} else if (type == CONTINUOUS) {
				// Add to list of continuous events
				for (unsigned char i = 0; i < maxContinuousEventCount; i++) {
					if (continuousEvents[i] == NULL) {
						// A free slot for the continuous event was found
						continuousEvents[i] = currentEvent;
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
		ContinuousEvent *event = (ContinuousEvent *) continuousEvents[i];
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
