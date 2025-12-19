#include "AppHelper.h"
#include <condition_variable>
#include <cstdlib>
#include <mutex>

static std::condition_variable waitForExitCv;
static std::mutex waitForExitCvM;

void AppHelper::Exit() { waitForExitCv.notify_all(); }

void AppHelper::AwaitExit() {
  std::unique_lock<std::mutex> lk(waitForExitCvM);
  waitForExitCv.wait(lk);
}
