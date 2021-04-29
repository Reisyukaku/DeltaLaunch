/*
* ΔLaunch
* Copyright (c) 2018-2019, Reisyukaku
* 
* Permission to use, copy, modify, and/or distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
* 
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
* WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
* ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
* OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#pragma once
#include <functional>
#include "../Core/types.h"

class ThreadManager {
	public:
		ThreadManager() {
			m_shouldRun = true;
			m_theadExited = false;
			memset(&t, 0, sizeof(t));
		}

		ThreadManager(std::function<bool()> callback) {
			m_shouldRun = true;
			m_theadExited = false;
			memset(&t, 0, sizeof(t));
			this->callback() = callback;
		}

		virtual ~ThreadManager() {
			if (shouldRun())
				m_shouldRun = false;

			join();
		}

		void join(){
#ifdef  __SWITCH__
			if (!threadFinished())
				threadWaitForExit(&t);
			threadClose(&t);
#endif
		}

		bool start(u32 threadOffset = 1, u8 priority = 0x3B) {
			DEBUG(Debug::Type::THREAD, "Starting thread..\n");
#ifdef __SWITCH__
			memset(&t, 0, sizeof(t));
			if (threadCreate(&t, (void(*)(void*))&__threadEntry, this, NULL, 0x20000, priority, (svcGetCurrentProcessorNumber() + threadOffset) % 4))
				return false;

			if (threadStart(&t))
				return false;
#endif
			return true;
		}

		virtual bool init() { return true; }

		virtual bool step() {
			if (callback()) 
				return callback()();
			return false;	
		}

		virtual void exit() { /*STUB*/ }

		static void __threadEntry(ThreadManager* p) {

			if (!p || !p->init()) {
				if (p)
					p->shouldRun() = false;

				return;
			}

			while (p->shouldRun() && p->step()) {}

			p->exit();
			p->threadFinished() = true;
		}

		bool& shouldRun() { return m_shouldRun;	}
		bool& threadFinished() { return m_theadExited; }
		std::function<bool()>& callback() { return m_callback; }

	protected:
		std::function<bool()> m_callback;
		bool m_shouldRun;
		bool m_theadExited;
		Thread t;
};