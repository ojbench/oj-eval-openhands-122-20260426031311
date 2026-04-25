#ifndef MEMO_HPP
#define MEMO_HPP

#include "event.h"
#include <vector>
#include <iostream>

inline std::string CustomNotifyLateEvent::GetNotification(int n) const {
  return NotifyLateEvent::GetNotification(n) + generator_(n);
}

class Memo {
 public:
  // 显示删除默认构造函数
  Memo() = delete;

  // 构造函数，参数duration表示需要模拟第1~duration小时的备忘录
  Memo(int duration) : duration_(duration), current_hour_(0) {
  }

  // 析构函数，需保证没有内存泄漏
  ~Memo() {
  }

  // 向备忘录中加入一项事件。传入一个Event指针，需根据实际派生类类型进行具体操作
  void AddEvent(const Event *event) {
    events_.push_back(event);
  }

  // 模拟时间流逝，进行下一个小时的事件提醒
  void Tick() {
    if (current_hour_ >= duration_) return;
    current_hour_++;
    for (const auto* event : events_) {
      if (event->IsComplete()) continue;
      
      int deadline = event->GetDeadline();
      
      if (const auto* nle = dynamic_cast<const NotifyLateEvent*>(event)) {
        if (current_hour_ == deadline) {
          std::cout << nle->GetNotification(0) << std::endl;
        } else if (current_hour_ > deadline) {
          int freq = nle->GetFrequency();
          if ((current_hour_ - deadline) % freq == 0) {
            std::cout << nle->GetNotification((current_hour_ - deadline) / freq) << std::endl;
          }
        }
      } else if (const auto* nbe = dynamic_cast<const NotifyBeforeEvent*>(event)) {
        if (current_hour_ == deadline - nbe->GetNotifyTime()) {
          std::cout << nbe->GetNotification(0) << std::endl;
        } else if (current_hour_ == deadline) {
          std::cout << nbe->GetNotification(1) << std::endl;
        }
      } else if (const auto* ne = dynamic_cast<const NormalEvent*>(event)) {
        if (current_hour_ == deadline) {
          std::cout << ne->GetNotification(0) << std::endl;
        }
      }
    }
  }

 private:
  int duration_;
  int current_hour_;
  std::vector<const Event*> events_;
};
#endif