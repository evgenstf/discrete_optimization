#pragma once

#define LOG(message) do { if (enable_logging) { clog << "[" << class_name_ << "::" << __func__ << "]: " << message << endl; } } while (false);

#ifdef DEBUG
#define DBG(message) do { if (enable_logging) { clog << "<" << class_name_ << "::" << __func__ << ">: " << message << endl; } } while (false);
#else
#define DBG(message) do {} while (false);
#endif

#define ASSERT(condition, message) do { if (!(condition)) { LOG(message); exit(0); } } while (false);
#define MAX_SOLVING_THREAD_COUNT 18
#pragma once

//log.h"
#include <algorithm>
#include <atomic>
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <set>
#include <map>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <tuple>
#include <experimental/optional>
#include <list>
#include <deque>

#define X first
#define Y second
#define PT pair<int, int>
#define mk make_pair
#define ll long long

using namespace std;
using namespace experimental;

string class_name_;
bool enable_logging = true;

#pragma once
//../base/log.h"
#include <utility>

template<typename T>
class Vector {
public:
  Vector(int reserved_size, int init_size = 0):
      reserved_size_(reserved_size + 1), end_(init_size) {
    data_ = new T[reserved_size_];
  }
  ~Vector() { delete[] data_; }

  int size() const { return end_; }

  const T& front() const { return data_[0]; }
  T& front() { return data_[0]; }
  const T& back() const { return data_[end_ - 1]; }
  T& back() { return data_[end_ - 1]; }

  void push_back(const T& item) {
    ASSERT(end_ < reserved_size_ - 1, "no space for push_back")
    data_[end_++] = move(item);
  }
  void pop_back() { end_ -= !!end_; }

  const T* begin() const { return data_; }
  T* begin() { return data_; }
  const T* end() const { return &data_[end_]; }
  T* end() { return &data_[end_]; }

  void swap(Vector<T>& rhs) {
    std::swap(reserved_size_, rhs.reserved_size_);
    std::swap(end_, rhs.end_);
    std::swap(data_, rhs.data_);
  }

  T& operator [](int index) { return data_[index]; }
  const T& operator [](int index) const { return data_[index]; }

  void reset() { end_ = 0; }

private:
  int reserved_size_;
  int end_;
  T* data_;
};
#pragma once
//vector.h"
//../base/log.h"
#include <utility>
#include <cstring>

template<typename T>
class SparceVector {
public:
  SparceVector(int size):
      size_(size), timer_(31), way_(size) {
    time_ = new int[size_];
    memset(time_, 0, sizeof(int) * size_);
    data_ = new T[size_];
  }
  ~SparceVector() {
    delete[] time_;
    delete[] data_;
  }

  void set(int index, T value) {
    data_[index] = move(value);
    if (time_[index] != timer_) {
      time_[index] = timer_;
      way_.push_back(index);
    }
  }

  void erase(int index) {
    if (!exists(index)) {
      return;
    }
    data_[index] = nullptr;
    time_[index] = -1;
  }

  T& get(int index) {
    ASSERT(exists(index), "item does not exist: " << index)
    return data_[index];
  }

  const T& get(int index) const {
    ASSERT(exists(index), "item does not exist: " << index)
    return data_[index];
  }

  bool exists(int index) const { return time_[index] == timer_; }

  const Vector<int>& way() const { return way_; }

  void reset() {
    ++timer_;
    for (auto item : way_) {
      //std::clog << "delete item: " << item << std::endl;
      data_[item] = nullptr;
    }
    way_.reset();
  }

  void reset_prefix(int len) {
    sort(way_.begin(), way_.end(), [](int a, int b) {return a > b; });
    for (int i = 0; i < len && way_.size() > 0; ++i) {
      data_[way_.back()] = nullptr;
      time_[way_.back()] = -1;
      way_.pop_back();
    }
  }

  void sort_way() {
    sort(way_.begin(), way_.end(), [](int a, int b) {return a < b; });
  }

  void update_way() {
    way_.reset();
    for (int i = 0; i < size_; ++i) {
      if (time_[i] == timer_) {
        way_.push_back(i);
      }
    }
  }

private:
  int size_;
  int timer_;
  int* time_;
  T* data_;
  Vector<int> way_;
};
//../base/common.h"
//../base/log.h"
//../fast/vector.h"
//../fast/sparce_vector.h"

template<typename kCostType = int>
class FastKnapsack {
public:
  struct Position;
  struct Pack;

  FastKnapsack(int capacity, int items_count):
      kCapacity(capacity),
      kItemsCount(items_count),
      cells_(capacity + 1),
      next_cells_(capacity + 1),
      last_reset(1000) {
    DBG("init capacity: " << kCapacity)
    reset();
    depth_ = sqrt(kCapacity) * sqrt(kItemsCount) / 12;
  }

  bool add_item(int index);
  void reset();
  void print();

  int capacity() { return kCapacity; }
  void decrease_depth() { depth_ = 5000; }

  const shared_ptr<Pack>& best_pack() const { return cells_.get(best_cell_); }
  kCostType best_cost() { return best_pack()->cost; }

  virtual int free_space() { return kCapacity - best_pack()->next_free_cell; }

protected:
  virtual std::string representation();

private:
  inline virtual optional<Pack> try_emplace(int item_index, shared_ptr<Pack> pack) { return nullopt; }
  virtual void reset_internal() {}

  SparceVector<shared_ptr<Pack>> cells_;
  SparceVector<shared_ptr<Pack>> next_cells_;

  int best_cell_;
  int last_reset;
  int depth_;

  const int kCapacity;
  const int kItemsCount;
  const std::string class_name_ = "FastKnapsack";
};


template<typename kCostType>
bool FastKnapsack<kCostType>::add_item(int index) {
  auto previous_best_pack_cost = best_cost();

  int minx = 1000000000;
  next_cells_.reset();
  const auto& way = cells_.way();
  int cnt = depth_;
  for (int i = way.size() - 1; i >= 0; --i) {
    auto cell = way[i];
    if (!cells_.exists(cell)) {
      continue;
    }
    const auto& pack_ptr = cells_.get(cell);
    const auto& pack = *pack_ptr;
    DBG("process pack: next_free_cell: " << pack.next_free_cell << " cost: " << pack.cost)
    auto next_pack = try_emplace(index, pack_ptr);
    if (next_pack) {
      int next_free_cell = next_pack->next_free_cell;
      kCostType next_cost = next_pack->cost;
      DBG("can emplace: next_free_cell: " << next_free_cell <<
          " cost: " << next_cost << " cnt: " << next_pack->size)
      if (
          !next_cells_.exists(next_free_cell) ||
          next_cost > next_cells_.get(next_free_cell)->cost
      ) {
        if (minx > next_free_cell) {
          minx = next_free_cell;
        }
        next_cells_.set(next_free_cell, make_shared<Pack>(move(*next_pack)));
        --cnt;
        if (cnt == 0) {
          break;
        }
      }
    }
  }

  if (way.size() > 1100000) {
    std::clog << "reset!!!";
    cells_.reset_prefix(way.size() - 500000);
  }

  for (auto cell_to_update : next_cells_.way()) {
    kCostType cost_to_update = next_cells_.get(cell_to_update)->cost;
    if (
        !cells_.exists(cell_to_update) ||
        cost_to_update > cells_.get(cell_to_update)->cost
    ) {
      cells_.set(cell_to_update, move(next_cells_.get(cell_to_update)));
      if (cost_to_update > cells_.get(best_cell_)->cost) {
        best_cell_ = cell_to_update;
      }
    }
  }

  if (last_reset-- == 0) {
    last_reset = 1000;
    cells_.sort_way();
    const auto& way = cells_.way();
    int min_cost = -1;
    int cnt = 0;
    for (auto cell : way) {
      //std::clog << cell << ' ';
      const auto& pack = *cells_.get(cell);
      if (pack.cost > min_cost) {
        min_cost = pack.cost;
        best_cell_ = cell;
        //std::clog << "updated best cost: " << pack.cost << '\n';
      } else {
        cells_.erase(cell);
        ++cnt;
      }
    }
    cells_.update_way();
    //exit(0);
    std::clog << "erased cnt: " << cnt << '\n';
  }


  /*
  if (minx > 5000) {
    for (int i = 0; i < minx / 2; ++i) {
      cells_.erase(i);
    }
  }
  */

  DBG("finished: best cost: " << best_cost() << " free space: " << free_space() <<
      " cells_way_ size: " << cells_.way().size())
  return previous_best_pack_cost != best_cost();
}

template<typename kCostType>
void FastKnapsack<kCostType>::reset() {
  reset_internal();
  cells_.reset();
  best_cell_ = 0;
  cells_.set(0, make_shared<Pack>());
}

template<typename kCostType>
string FastKnapsack<kCostType>::representation() {
  return "kek";
}

template<typename kCostType>
void FastKnapsack<kCostType>::print() {
  std::string repr = representation();
  LOG("==========================")
  LOG("best pack: " << repr)
  LOG("cost: " << best_cost())
  LOG("items: " << best_pack()->size)
  LOG("unused space: " << free_space())
  LOG("==========================")
}

template<typename kCostType>
struct FastKnapsack<kCostType>::Position {
  int l, r;

  Position():
    l(-1), r(-1) {}

  Position(int l, int r):
    l(l), r(r) {}
};

template<typename kCostType>
struct FastKnapsack<kCostType>::Pack {
  kCostType cost;
  int next_free_cell;
  shared_ptr<Pack> previous_pack;
  pair<int, Position> item;
  int size;

  Pack():
    cost(0), next_free_cell(0), previous_pack(nullptr), size(0) {}

  auto create_vector() {
    vector<pair<int, Position>> result;
    result.push_back(item);
    if (previous_pack) {
      auto pack = previous_pack;
      while (pack->previous_pack) {
        result.push_back(pack->item);
        pack = pack->previous_pack;
      }
    }
    reverse(result.begin(), result.end());
    return result;
  }

  void set_parent(const shared_ptr<Pack>& parent) {
    size = parent->size + 1;
    previous_pack = parent;
  }
};

//../../base/common.h"
//../../utils/fast_knapsack.h"

template<typename kCostType = int>
class KnapsackInterface : public FastKnapsack<kCostType> {
public:
  using Position = typename FastKnapsack<kCostType>::Position;
  using Pack = typename FastKnapsack<kCostType>::Pack;
  using Range = Position;

  KnapsackInterface(int capacity, int items_count):
    FastKnapsack<kCostType>(capacity, items_count), capacity_kek(capacity) {
  }

  bool add_item_with_weight_and_cost(int index, const pair<int, int>& rd) {
    index_to_item_[index] = rd;
    return this->add_item(index);
  }

private:
  optional<Pack> try_emplace(int index, shared_ptr<Pack> pack) override {
    const auto& ride = index_to_item_[index];

    int next_free_cell = 0;
    if (pack->size == 0) {
      next_free_cell = ride.second;
    } else {
      next_free_cell += pack->next_free_cell + ride.second;
    }
    DBG("next_free_cell: " << next_free_cell)

    int cost = ride.first;

    if (next_free_cell > capacity_kek) {
      return nullopt;
    }


    Pack new_pack;
    new_pack.cost = pack->cost + cost;
    new_pack.next_free_cell = next_free_cell;
    new_pack.size = pack->size + 1;
    new_pack.previous_pack = pack;
    new_pack.item = mk(index, Position(pack->next_free_cell, next_free_cell));
    return new_pack;
  }

  void reset_internal() override {
    index_to_item_.clear();
  }

  unordered_map<int, pair<int, int>> index_to_item_;

  int capacity_kek;

  const string class_name_ = "KnapsackInterface";
};
////ride_knapsack.h"
//fast_ride_knapsack.h"
#include <ctime>


std::clock_t start;

double get_duration() {
  return ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
}

int main() {
  start = std::clock();

  int n, w;
  cin >> n >> w;
  vector<pair<pair<int, int>, int>> items(n);
  int ind = 0;
  for (auto& item : items) {
    cin >> item.first.first >> item.first.second;
    item.second = ind++;
  }
  sort(items.begin(), items.end(), [](const pair<pair<int, int>, int>& l, const pair<pair<int, int>, int>& r) {
      return  double(l.first.second) /  double(l.first.first * 1 + 1) <  double(r.first.second) /  double(r.first.first * 1 + 1); 
  });

  vector<int> greed_way;
  int greed_ans = 0;
  int grees_cap = w;
  for (const auto& item : items) {
    if (grees_cap >= item.first.second) {
      grees_cap -= item.first.second;
      greed_ans += item.first.first;
      greed_way.push_back(item.second);
    }
  }
  std::clog << "greed calculated: " << get_duration() << '\n';

  KnapsackInterface<int> knapsack(w, n);
  auto previous_duration = get_duration();
  for (int i = 0; i < items.size(); ++i) {
    if (i > 0 && i % 50 == 0) {
      auto duration = get_duration();
      std::clog << "i: " << i << " best_cost: " << knapsack.best_cost() << " duration: " <<
          duration << std::endl;
      auto diff = duration - previous_duration;
      if (duration + diff > 9.5) {
        std::clog << "stop: duration: " << duration << " diff: " << diff << '\n';
        break;
      }
      if (w > 4000000) {
        if (duration + diff > 9.0) {
          std::clog << "stop: duration: " << duration << " diff: " << diff << '\n';
          break;
        }
      }
      if (w >= 6000000) {
        if (duration + diff > 8.0) {
          std::clog << "stop: duration: " << duration << " diff: " << diff << '\n';
          break;
        }
      }


      if (duration > 4.0) {
        knapsack.decrease_depth();
      }
      previous_duration = duration;
    }
    knapsack.add_item_with_weight_and_cost(items[i].second, items[i].first);
  }

  if (knapsack.best_cost() > greed_ans) {
    std::cout << knapsack.best_cost() << std::endl;
    for (const auto& item : knapsack.best_pack()->create_vector()) {
      std::cout << item.first + 1 << ' ';
    }
    std::clog << "knapsack_cost: " << knapsack.best_cost() << std::endl; std::clog << "greed_cost: " << greed_ans << std::endl;
    std::clog << "free_space: " << knapsack.free_space() << std::endl;
  } else {
    std::clog << "greed better than knapsack!!!!\n";
    std::clog << "knapsack_cost: " << knapsack.best_cost() << std::endl;
    std::clog << "greed_cost: " << greed_ans << std::endl;
    std::cout << greed_ans << std::endl;
    for (const auto& item : greed_way) {
      std::cout << item + 1 << ' ';
    }
  }
  //std::clog << "size: " << knapsack.best_pack().items.size() << std::endl;
}
