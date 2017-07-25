// Copyright 2013 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/libplatform/worker-thread.h"

#include "include/v8-platform.h"
#include "src/libplatform/task-queue.h"
#include "v8.h"

namespace v8 {
namespace platform {

WorkerThread::WorkerThread(TaskQueue* queue)
    : Thread(Options("V8 WorkerThread")), queue_(queue) {
    v8::V8::LogMessage("Starting Thread");
  Start();
}


WorkerThread::~WorkerThread() {
  v8::V8::LogMessage("Killing Thread");
  Join();
}


void WorkerThread::Run() {
  while (Task* task = queue_->GetNext()) {
    task->Run();
    delete task;
  }
}

}  // namespace platform
}  // namespace v8
