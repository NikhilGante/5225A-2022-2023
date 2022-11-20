#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <compare>
#include <concepts>
#include <cstring>
#include <exception>
#include <iostream>

template <typename O, typename I> concept output_iter = std::input_iterator<I> && std::output_iterator<O, typename std::iterator_traits<I>::value_type>;

template <std::input_or_output_iterator I, std::input_or_output_iterator SIZE>
constexpr std::pair<std::pair<I, I>, std::pair<I, I>> split(I first, I last, std::pair<SIZE, SIZE> size){
  auto offset = std::distance(size.first, size.second);
  I middle = std::clamp(std::next(offset < 0 ? last : first, offset), first, last);
  return {std::make_pair(first, middle), std::make_pair(middle, last)};
}

template <std::input_iterator I, output_iter<I> O>
constexpr auto copy_pair(std::pair<I, I> in, std::pair<O, O> out){
  auto amount = std::min(std::distance(in.first, in.second), std::distance(out.first, out.second));
  std::copy_n(in.first, amount, out.first);
  return amount;
}

template <typename T,  std::size_t N> requires std::same_as<std::remove_cvref_t<T>, T>
class Queue{
  public:
    class iterator;
    using size_type              = std::size_t;
    using array                  = std::array<T, N>;
    using difference_type        = std::ptrdiff_t;
    using value_type             = T;
    using pointer                = value_type*;
    using reference              = value_type&;
    using const_value_type       = value_type const;
    using const_pointer          = const_value_type*;
    using const_reference        = const_value_type&;

    class iterator{
      friend Queue<T, N>;
      public:
        using difference_type   = Queue<T, N>::difference_type;
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = Queue<T, N>::value_type;
        using pointer           = Queue<T, N>::pointer;
        using reference         = Queue<T, N>::reference;

      public:
        pointer internal;
        pointer begin, end;
        int cycle;

        constexpr bool same_container(iterator const& rhs) const {return begin == rhs.begin && end == rhs.end;}
      public:
        constexpr iterator(pointer ptr, pointer begin, pointer end): internal{ptr}, begin{begin}, end{end}, cycle{0} {};
        constexpr iterator(std::nullptr_t = nullptr): iterator{nullptr, nullptr, nullptr} {};

        constexpr iterator& operator+=(difference_type n) {
          cycle += floor(n / static_cast<double>(N));
          n %= static_cast<std::ptrdiff_t>(N);
          if(n < 0) {n += N;}
          if(n >= end-internal) {internal -= N; cycle++;}
          internal += n;
          return *this;
        }
        constexpr iterator& operator-=(difference_type n) {return *this += -n;}
        constexpr iterator operator+(difference_type n) const {iterator temp{*this}; return temp += n;}
        friend constexpr iterator operator+(difference_type n, iterator const& rhs) {return rhs+n;}
        constexpr iterator operator-(difference_type n) const {return *this + -n;}
        constexpr iterator& operator++() {return *this += 1;}
        constexpr iterator& operator--() {return *this += -1;}
        constexpr iterator operator++(int) {iterator temp{*this}; ++(*this); return temp;}
        constexpr iterator operator--(int) {iterator temp{*this}; --(*this); return temp;}
        constexpr reference operator[](difference_type n) const {return *(*this + n);} //? Needed?
        constexpr reference operator*() const {return *internal;}
        constexpr pointer operator->() {return internal;}
        friend constexpr difference_type operator-(iterator const& lhs, iterator const& rhs) {return lhs.same_container(rhs) ? lhs.internal-rhs.internal+(lhs.cycle-rhs.cycle)*(lhs.end-lhs.begin) : std::numeric_limits<difference_type>::max();}

        //== checks for equality on the same cycle. <=> only checks pointer equality
        friend constexpr bool operator==(iterator const& lhs, iterator const& rhs) {return lhs.internal == rhs.internal && lhs.cycle == rhs.cycle;}
        constexpr std::partial_ordering operator<=>(iterator const& rhs) const{
          if(same_container(rhs)) return cycle == rhs.cycle ? internal <=> rhs.internal : cycle <=> rhs.cycle;
          else return std::partial_ordering::unordered;
        }
    };


  private:
    //front_iter points to element about to be popped, back_iter points to location where element will be inserted
    Mutex mutex;
    iterator front_iter, back_iter;
    array arr;

    constexpr std::pair<std::pair<pointer, pointer>, std::pair<pointer, pointer>> empty_contiguous_iterators(){ //Returns two ranges corresponding to unfilled part of queue
      if(full()) return {};
      if(begin().internal <= end().internal) return std::make_pair(std::pair(end().internal, arr.end()), std::pair(arr.begin(), begin().internal)); //Wraparound
      else return std::make_pair(std::pair(end().internal, begin().internal), std::pair<pointer, pointer>()); //Single Range
    }
    constexpr std::pair<std::pair<pointer, pointer>, std::pair<pointer, pointer>> full_contiguous_iterators(){ //Returns two ranges corresponding to filled part of queue
      if(empty()) return {};
      if(begin().internal < end().internal) return std::make_pair(std::pair(begin().internal, end().internal), std::pair<pointer, pointer>()); //Single Range
      else return std::make_pair(std::pair(begin().internal, arr.end()), std::pair(arr.begin(), end().internal)); //Wraparound
    }

    constexpr iterator construct_iterator(pointer pointer) {return {pointer, arr.begin(), arr.end()};}

  public:

  //Constructors
    constexpr Queue(): arr{} {clear();}

    //Getters
    constexpr size_type size() const {return end()-begin();}
    constexpr size_type capacity() const {return N;}
    constexpr size_type space_left() const {return std::max(static_cast<size_type>(0), capacity()-size());}
    constexpr bool full() const {return size() == capacity();}
    constexpr bool empty() const {return size() == 0;}
    constexpr iterator begin() const {return front_iter;}
    constexpr iterator end() const {return back_iter ;}
    constexpr reference front() {return *begin();}
    constexpr reference back() {return *(end()-1);}
    constexpr const_reference front() const {return *begin();}
    constexpr const_reference back() const {return *(end()-1);}
    
    
    //Insert Modifiers
    constexpr void push(const_reference value){mutex.take(); if(!full()) *back_iter++ = value; mutex.give();}
    constexpr iterator insert(const_reference value){push(value); return end();}
    template <std::input_iterator I> constexpr iterator insert(I first, I last){
      mutex.take();
      auto out = empty_contiguous_iterators();
      auto in = split(first, last, out.first);
      back_iter += copy_pair(in.first,  out.first );
      back_iter += copy_pair(in.second, out.second);
      mutex.give();
      return end();
    }
    constexpr iterator insert(const_pointer pointer, size_type count) {return insert(pointer, pointer+count);}
    constexpr iterator insert(const char* str) requires std::same_as<T, char> {return insert(str, str+strlen(str)+1);}

    //Remove Modifiers
    constexpr void pop() {mutex.take(); if(!empty()) *front_iter++ = value_type{}; mutex.give();}
    constexpr void clear() {mutex.take(); front_iter = back_iter = construct_iterator(arr.begin()); mutex.give();}
    template <output_iter<iterator> O> constexpr O output(O first, O last){
      mutex.take();
      auto in = full_contiguous_iterators();
      auto out = split(first, last, in.first);
      copy_pair(in.first,  out.first );
      copy_pair(in.second, out.second);
      clear();
      mutex.give();
      return out.second.end();
    }
    template <output_iter<iterator> O> constexpr O output(O out) {
      mutex.take();
      auto in = full_contiguous_iterators();
      out = std::copy(in.first .first, in.first .second, out);
      out = std::copy(in.second.first, in.second.second, out);
      clear();
      mutex.give();
      return out;
    }
    constexpr void output(std::ostream& out) requires std::same_as<T, char> {
      mutex.take();
      auto in = full_contiguous_iterators();
      out.write(in.first .first, in.first .second-in.first .first);
      out.write(in.second.first, in.second.second-in.second.first);
      mutex.give();
    }
};

template <typename T,  std::size_t N>
std::ostream& operator<<(std::ostream& os, Queue<T, N>& queue){
  queue.output(os);
  return os;
}