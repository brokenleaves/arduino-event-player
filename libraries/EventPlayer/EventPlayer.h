#ifndef EventPlayer_h
#define EventPlayer_h

#include "WProgram.h"

enum EventType {
	ONETIME, CONTINUOUS, JUMP
};

class Event {
	public:
		EventType getType();
		unsigned long getTime();
		virtual void run(unsigned long playerTimeMillis);
	protected:
		Event(EventType type, unsigned long timeMillis);
		unsigned long timeMillis;
	private:
		EventType type;
};

class OnetimeEvent : public Event {
	public:
		OnetimeEvent(unsigned long timeMillis);
};

class ContinuousEvent : public Event {
	public:
		ContinuousEvent(unsigned long timeMillis, unsigned long lengthMillis);
		unsigned long getLength();
		unsigned long getEndTime();
		virtual void init(unsigned long playerTimeMillis);
	protected:
		unsigned long lengthMillis;
};

class JumpEvent : public Event {
	public:
		JumpEvent(unsigned long timeMillis, unsigned short eventIndex, unsigned char loopCount);
		unsigned short getEventIndex();
		unsigned char getLoopCount();
		unsigned char getLoopsExecuted();
		void setLoopsExecuted(unsigned char loopsExecuted);
	protected:
		unsigned short eventIndex;
		unsigned char loopCount;
		unsigned char loopsExecuted;
};

class EventPlayer {
	public:
		EventPlayer(unsigned short maxEventCount, unsigned char maxContinuousEventCount);
		~EventPlayer();
		void add(Event *event);
		void prepare();
		void start(unsigned long timeMicros);
		void run(unsigned long timeMicros);
	private:
		unsigned short eventCount;
		unsigned char maxContinuousEventCount;
		unsigned short firstFreeEventIndex;
		unsigned short currentEventIndex;
		unsigned long startTimeMicros;
		Event **events;
		ContinuousEvent **continuousEvents;
};

#endif
