// Copyright 2013 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/libplatform/worker-thread.h"

#include "include/v8-platform.h"
#include "src/libplatform/task-queue.h"
#include "src/v8.h"

namespace v8 {
namespace platform {

WorkerThread::WorkerThread(TaskQueue* queue)
    : Thread(Options("V8 WorkerThread")), queue_(queue) {
    v8::V8::LogMessage("WorkerThread: Starting\n");
  Start();
}


WorkerThread::~WorkerThread() {
  v8::V8::LogMessage("WorkerThread: Killing\n");
  Join();
}


void WorkerThread::Run() {
  v8::V8::LogMessage("WorkerThread: Run()\n");
  while (Task* task = queue_->GetNext()) {
    v8::V8::LogMessage("WorkerThread: Running Task\n");
    task->Run();
    v8::V8::LogMessage("WorkerThread: Deleting Task\n");
    delete task;
    v8::V8::LogMessage("WorkerThread: Done Task\n");
  }
}

}  // namespace platform
}  // namespace v8
