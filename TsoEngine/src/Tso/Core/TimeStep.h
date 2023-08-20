#pragma once

namespace Tso {

	class TimeStep {
	public:
		TimeStep(float time)
			:m_Time(time) {

		}

		float GetSecond() { return m_Time; }
		float GetMillisecond(){return m_Time * 1000.f;}

		operator float() const { return m_Time; }

	private:
		float m_Time;
	};

}