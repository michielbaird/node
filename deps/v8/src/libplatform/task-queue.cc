// Copyright 2013 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/libplatform/task-queue.h"

#include "src/base/logging.h"
#include "src/v8.h"

namespace v8 {
namespace platform {

TaskQueue::TaskQueue() : process_queue_semaphore_(0), terminated_(false), purging_(false) {}


TaskQueue::~TaskQueue() {
  base::LockGuard<base::Mutex> guard(&lock_);
  DCHECK(terminated_);
  DCHECK(task_queue_.empty());
}


void TaskQueue::Append(Task* task) {
  v8::V8::LogMessage("TaskQueue:GetNext() appending\n");
  base::LockGuard<base::Mutex> guard(&lock_);
  DCHECK(!terminated_);
  task_queue_.push(task);
  process_queue_semaphore_.Signal();
  v8::V8::LogMessage("TaskQueue:GetNext() done appending\n");
}


Task* TaskQueue::GetNext() {
  for (;;) {
    {
      v8::V8::LogMessage("TaskQueue:GetNext() try getting\n");
      base::LockGuard<base::Mutex> guard(&lock_);
      v8::V8::LogMessage("TaskQueue:GetNext() in lock\n");
      if (purging_) {
        v8::V8::LogMessage("TaskQueue:GetNext() purging\n");
        process_queue_semaphore_.Signal();
        v8::V8::LogMessage("TaskQueue:GetNext() out of lock\n");
        return NULL;
      }
      if (!task_queue_.empty()) {
        v8::V8::LogMessage("TaskQueue:GetNext() giving\n");
        Task* result = task_queue_.front();
        task_queue_.pop();
        v8::V8::LogMessage("TaskQueue:GetNext() out of lock\n");
        return result;
      }
      if (terminated_) {
        v8::V8::LogMessage("TaskQueue:GetNext() terminated\n");
        process_queue_semaphore_.Signal();
        v8::V8::LogMessage("TaskQueue:GetNext() out of lock\n");
        return NULL;
      }

      v8::V8::LogMessage("TaskQueue:GetNext() out of lock\n");
    }
    v8::V8::LogMessage("TaskQueue:GetNext() starting waiting\n");
    process_queue_semaphore_.Wait();
    v8::V8::LogMessage("TaskQueue:GetNext() done waiting\n");
  }
}

void TaskQueue::PurgeWorkers() {
  base::LockGuard<base::Mutex> guard(&lock_);
  purging_ = true;
  process_queue_semaphore_.Signal();
}

void TaskQueue::StopPurgeWorkers() {
  base::LockGuard<base::Mutex> guard(&lock_);
  purging_ = false;
}

void TaskQueue::Terminate() {
  base::LockGuard<base::Mutex> guard(&lock_);
  DCHECK(!terminated_);
  terminated_ = true;
  process_queue_semaphore_.Signal();
}

}  // namespace platform
}  // namespace v8
