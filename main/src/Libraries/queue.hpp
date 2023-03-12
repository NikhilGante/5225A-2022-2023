#pragma once
#include "main.h"
#include "printing.hpp"

//Forward Declare
namespace alert {struct Params; void priority(alert::Params params, std::string fmt, auto... args);}

template <typename O, typename I> concept output_iter = std::input_iterator<I> && std::output_iterator<O, typename std::iterator_traits<I>::value_type>;

template <std::input_or_output_iterator I, std::input_or_output_iterator SIZE>
constexpr std::pair<std::pair<I, I>, std::pair<I, I>> split(I first, I last, std::pair<SIZE, SIZE> size){
  auto offset = std::distance(size.first, size.second);
  I middle = std::clamp(std::next(offset < 0 ? last : first, offset), first, last);
  return {{first, middle}, {middle, last}};
}

template <std::input_iterator I, output_iter<I> O>
constexpr auto copy_pair(std::pair<I, I> in, std::pair<O, O> out){
  auto amount = std::min(std::distance(in.first, in.second), std::distance(out.first, out.second));
  std::copy_n(in.first, amount, out.first);
  return amount;
}

template <typename T,  std::size_t N>
class Queue{
  public:
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
        using value_type        = Queue<T, N>::value_type;
        using pointer           = Queue<T, N>::pointer;
        using reference         = Queue<T, N>::reference;
        using iterator_category = std::random_access_iterator_tag;

      private:
        pointer internal;
        pointer begin, end;
        int cycle;

        constexpr bool same_container(iterator const & rhs) const {return begin == rhs.begin && end == rhs.end;}
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
        friend constexpr iterator operator+(difference_type n, iterator const & rhs) {return rhs+n;}
        constexpr iterator operator-(difference_type n) const {return *this + -n;}
        constexpr iterator& operator++() {return *this += 1;}
        constexpr iterator& operator--() {return *this += -1;}
        constexpr iterator operator++(int) {iterator temp{*this}; ++(*this); return temp;}
        constexpr iterator operator--(int) {iterator temp{*this}; --(*this); return temp;}
        constexpr reference operator*() const {return *internal;}
        constexpr pointer operator->() {return internal;}
        friend constexpr difference_type operator-(iterator const & lhs, iterator const & rhs) {return lhs.same_container(rhs) ? lhs.internal-rhs.internal+(lhs.cycle-rhs.cycle)*(lhs.end-lhs.begin) : std::numeric_limits<difference_type>::max();}

        //== checks for equality on the same cycle. <=> only checks pointer equality
        friend constexpr bool operator==(iterator const & lhs, iterator const & rhs) {return lhs.internal == rhs.internal && lhs.cycle == rhs.cycle;}
        constexpr std::partial_ordering operator<=>(iterator const & rhs) const{
          if(same_container(rhs)) return cycle == rhs.cycle ? internal <=> rhs.internal : cycle <=> rhs.cycle;
          else return std::partial_ordering::unordered;
        }
    };


  private:
    //front_iter points to element about to be popped, back_iter points to location where element will be inserted
    array arr;
    iterator front_iter, back_iter;

    constexpr std::pair<std::pair<pointer, pointer>, std::pair<pointer, pointer>> empty_contiguous_iterators(){ //Returns two ranges corresponding to unfilled part of queue
      if(full()) return {};
      if(begin().internal <= end().internal) return {std::pair{end().internal, arr.end()}, std::pair{arr.begin(), begin().internal}}; //Wraparound
      else return {std::pair{end().internal, begin().internal}, {}}; //Single Range
    }
    constexpr std::pair<std::pair<const_pointer, const_pointer>, std::pair<const_pointer, const_pointer>> full_contiguous_iterators() const { //Returns two ranges corresponding to filled part of queue
      if(empty()) return {};
      if(begin().internal < end().internal) return {std::pair{begin().internal, end().internal}, {}}; //Single Range
      else return {std::pair{begin().internal, arr.end()}, std::pair{arr.begin(), end().internal}}; //Wraparound
    }

    constexpr iterator construct_iterator(pointer pointer) {return {pointer, arr.begin(), arr.end()};}

  public:
    std::string name;
    
  //Constructors
    constexpr Queue(): name{}, arr{}, front_iter{construct_iterator(arr.begin())}, back_iter{front_iter} {}
    Queue(std::string name): name{name}, arr{}, front_iter{construct_iterator(arr.begin())}, back_iter{front_iter} {}

  //Getters
    constexpr size_type size() const {return end()-begin();}
    constexpr size_type capacity() const {return N;}
    constexpr bool full() const {return size() == capacity();}
    constexpr bool empty() const {return size() == 0;}
    constexpr iterator        begin() const {return front_iter;}
    constexpr iterator        end()   const {return back_iter ;}
    constexpr const_reference front() const {return *begin();}
    constexpr const_reference back()  const {return *(end()-1);}
    constexpr const_reference operator[](difference_type n) const {return *(begin() + n);}
    constexpr reference       front(){return *begin();}
    constexpr reference       back() {return *(end()-1);}
    constexpr reference       operator[](difference_type n) {return *(begin() + n);}
    
  //Insert Modifiers
    constexpr void priority_push(const_reference value){
      if(full()) back_iter--;
      *end() = value;
      back_iter++;
    }
    constexpr void push(const_reference value){
      if(full()) return;
      *end() = value;
      back_iter++;
      if(size() >= capacity()) alert::priority(term_colours::ERROR , "%s queue has reached %d%% capacity", name, (100.0*size())/capacity());
      else if(size() >= capacity()*0.8) alert::priority(term_colours::WARNING, "%s queue has reached %d%% capacity", name, (100.0*size())/capacity());
    }
    template <std::input_iterator I> constexpr iterator insert(I first, I last) requires output_iter<iterator, I> {
      auto out = empty_contiguous_iterators();
      auto in  = split(first, last, out.first);
      back_iter += copy_pair(in.first,  out.first );
      back_iter += copy_pair(in.second, out.second);
      if(size() >= capacity()) alert::priority(term_colours::ERROR , "%s queue has reached %d%% capacity", name, (100.0*size())/capacity());
      else if(size() >= capacity()*0.8) alert::priority(term_colours::WARNING, "%s queue has reached %d%% capacity", name, (100.0*size())/capacity());
      return end();
    }
    constexpr iterator insert(auto const & range) {return insert(std::cbegin(range), std::cend(range));}

  //Remove Modifiers
    constexpr void pop() {if(!empty()) front_iter++;}
    constexpr void clear() {front_iter = end();}
    constexpr void output(std::ostream& out) requires std::same_as<T, char>{
      //construct string_view from the contiguous iterators and << that
      auto in = full_contiguous_iterators();
      out.write(in.first .first, std::distance(in.first .first, in.first .second));
      out.write(in.second.first, std::distance(in.second.first, in.second.second));
    }
};