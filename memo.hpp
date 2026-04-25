#ifndef MEMO_HPP
#define MEMO_HPP

#include "event.h"
#include <vector>
#include <iostream>
#include <map>
#include <algorithm>
#include <string>
#include <utility>

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
    int idx = events_.size();
    events_.push_back(event);
    
    int deadline = event->GetDeadline();
    if (const auto* nbe = dynamic_cast<const NotifyBeforeEvent*>(event)) {
      schedule(deadline - nbe->GetNotifyTime(), idx, 0);
      schedule(deadline, idx, 1);
    } else if (const auto* nle = dynamic_cast<const NotifyLateEvent*>(event)) {
      schedule(deadline, idx, 0);
    } else if (const auto* ne = dynamic_cast<const NormalEvent*>(event)) {
      schedule(deadline, idx, 0);
    }
  }

  // 模拟时间流逝，进行下一个小时的事件提醒
  void Tick() {
    if (current_hour_ >= duration_) return;
    current_hour_++;
    
    auto it = scheduled_.find(current_hour_);
    if (it != scheduled_.end()) {
      auto notifications = it->second;
      // Sort by event index to maintain order of addition
      std::sort(notifications.begin(), notifications.end());
      
      for (const auto& p : notifications) {
        int idx = p.first;
        int n = p.second;
        const Event* event = events_[idx];
        
        if (!event->IsComplete()) {
          std::cout << event->GetNotification(n) << std::endl;
          
          // If it's a NotifyLateEvent, schedule the next one
          if (const auto* nle = dynamic_cast<const NotifyLateEvent*>(event)) {
            schedule(current_hour_ + nle->GetFrequency(), idx, n + 1);
          }
        }
      }
      scheduled_.erase(it);
    }
  }

 private:
  void schedule(int hour, int event_idx, int n) {
    if (hour > current_hour_ && hour <= duration_) {
      scheduled_[hour].push_back({event_idx, n});
    }
  }

  int duration_;
  int current_hour_;
  std::vector<const Event*> events_;
  std::map<int, std::vector<std::pair<int, int>>> scheduled_;
};
#endif