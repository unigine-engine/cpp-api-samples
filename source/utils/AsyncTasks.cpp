/* Copyright (C) 2005-2023, UNIGINE. All rights reserved.
 *
 * This file is a part of the UNIGINE 2 SDK.
 *
 * Your use and / or redistribution of this software in source and / or
 * binary form, with or without modification, is subject to: (i) your
 * ongoing acceptance of and compliance with the terms and conditions of
 * the UNIGINE License Agreement; and (ii) your inclusion of this notice
 * in any version of this software that you use or redistribute.
 * A copy of the UNIGINE License Agreement is available by contacting
 * UNIGINE. at http://unigine.com/
 */

#include "AsyncTasks.h"

using namespace Unigine;


ThreadQueue &ThreadQueue::get()
{
	static ThreadQueue instance;
	return instance;
}

void ThreadQueue::add(AsyncTaskBase *task)
{
	ScopedLock lock(mutex);
	queue.append(task);
}

void ThreadQueue::remove(AsyncTaskBase *task)
{
	ScopedLock lock(mutex);
	queue.removeOne(task);
}

void ThreadQueue::process()
{
	while (isRunning())
	{
		AsyncTaskBase *task = nullptr;
		{
			ScopedLock lock(mutex);
			if (queue.size())
				task = queue.takeFirst();
		}
		if (task)
			task->process();
		Thread::switchThread();
	}
}

AsyncTaskBase::AsyncTaskBase()
{
}

AsyncTaskBase::~AsyncTaskBase()
{
	ThreadQueue::get().remove(this);
}
