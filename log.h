#pragma once

#include <vector>
#include <string>
#include <sstream>

class log_messages {
  using container = std::vector<std::string>;
public:
  using iterator = container::const_iterator;

  template <class ...Args>
  void append(Args&&... args) {
    std::ostringstream os;
    using List = int[];
    (void) List {
      0, ((void)(os << std::forward<Args>(args)), 0)...
    };
    log_.push_back(os.str());
  }

  iterator begin() const { return log_.begin(); }
  iterator end()   const { return log_.end(); }

private:
  container log_;
};
