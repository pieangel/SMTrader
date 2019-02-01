/// @file BlockingCollection.h
/// Copyright (c) Code Ex Machina, LLC. All rights reserved.
/// License Agreement: http://codemachina.io/pages/eula-blockingcollection

#ifndef BlockingCollection_h
#define BlockingCollection_h

#include <condition_variable>
#include <unordered_set>
#include <deque>
#include <thread>
#include <chrono>
#include <vector>

/// @namespace code_machina
/// @brief Contains the BlockingCollection<T> and PriorityBlockingCollection<T> classes.
namespace code_machina {

    /// @class BlockingCollectionConditionVariable
    /// BlockingCollectionConditionVariable wraps the std::condition_variable. It implements support
    /// for attaching and detaching workers to the condition variable. And serves as a base class
    /// to the NotFullConditionVariable and NotEmptyConditionVariable classes.
    /// @tparam ThreadContainerPolicy The type of thread container.
    /// @see NotEmptyConditionVariable
    /// @see NotFullConditionVariable
    template<class ThreadContainerPolicy> class BlockingCollectionConditionVariable
    {
    public:
        
        /// Initializes a new instance of the BlockingCollectionConditionVariable class without an upper-bound.
        BlockingCollectionConditionVariable()
        : total_threads_(0), active_threads_(0), bounded_capacity_(SIZE_MAX), item_count_(0) {
            
        }
        
        ~BlockingCollectionConditionVariable() {
            
        }
        
        /// Gets the number of threads attached to this condition variable.
        /// @return The number of threads attached to this condition variable.
        /// @see Attach
        size_t Total() {
            return total_threads_;
        }
        
        /// Gets the number of active threads for this condition variable.
        /// Active threads are threads that are currently NOT waiting on this condition variable.
        /// @return The number of active threads.
        size_t Active() {
            return active_threads_;
        }
        
        /// Gets the bounded capacity of this condition variable instance.
        /// @return The bounded capacity of this condition variable.
        size_t BoundedCapacity() {
            return bounded_capacity_;
        }
        
        /// Sets the bounded capacity of this condition variable instance.
        void BoundedCapacity(size_t capacity) {
            bounded_capacity_ = capacity;
        }
        
        /// Gets the number of items contained in this condition variable instance.
        /// @return The number of items
        size_t Count() {
            return item_count_;
        }
        
        /// Set the number of items contained in this condition variable instance.
        void Count(size_t count) {
            item_count_ = count;
        }
        
        /// Registers the calling thread with this condition variable.
        /// If the calling thread is already registered then this method has no effect.
        /// @see Detach
        void Attach()
        {
            if (container_.Add()) {
                
                IncrementTotal();
                IncrementActive();
            }
        }
        
        /// Unregisters the calling thread from this condition variable.
        /// If the calling thread was not previously registered then this method has no effect.
        /// @see Attach
        void Detach()
        {
            if (container_.Remove()) {
                DecrementTotal();
                DecrementActive();
            }
            
            if (total_threads_ > 0 && active_threads_ == 0) {
                
                IncrementActive();
                condition_var_.notify_one();
            }
        }
        
    protected:
        
        /// Sets the number of active threads for this condition variable.
        /// @param active The number of active threads.
        void Active(size_t active) {
            active_threads_ = active > total_threads_ ? total_threads_ : active;
        }
        
        /// Increments the total thread count for this condition variable by 1.
        void IncrementTotal() {
            total_threads_ += 1;
        }
        
        /// Decrements the total thread count for this condition variable by 1.
        void DecrementTotal() {
            if(total_threads_ > 0)
                total_threads_ -= 1;
            else
                total_threads_ = 0;
        }
        
        /// Increments the active thread count for this condition variable by 1.
        void IncrementActive() {
            if(++active_threads_ > total_threads_)
                active_threads_ = total_threads_;
        }
        
        /// Decrements the active thread count for this condition variable by 1.
        void DecrementActive() {
            if(active_threads_ > 0)
                active_threads_ -= 1;
            else
                active_threads_ = 0;
        }
        
        /// Returns a reference to the underlying std::condition_variable.
        std::condition_variable& Condition() {
            return condition_var_;
        }
        
    private:
        // "BlockingCollectionConditionVariable" objects cannot be copied or assigned
        BlockingCollectionConditionVariable(const BlockingCollectionConditionVariable&);
        BlockingCollectionConditionVariable& operator=(const BlockingCollectionConditionVariable&);
        
        size_t total_threads_;
        size_t active_threads_;
        size_t bounded_capacity_;
        size_t item_count_;
        
        std::condition_variable condition_var_;
        ThreadContainerPolicy container_;
    };

    /// @class NotEmptyConditionVariable
    /// NotEmptyConditionVariable is a specialized BlockingCollectionConditionVariable that wraps the
    /// std::condition_variable.
    /// It handles the "not empty" condition of the BlockingCollection. It allows threads to wait
    /// on a empty BlockingCollection. That is, wait until the BlockingCollection becomes "not empty".
    ///
    /// NotEmptyConditionVariable is optimized for this purpose.
    ///
    /// Optimized means that this condition variable will not signal the underlying std::condition_variable
    /// for every item that is added to the BlockingCollection. Rather it will only issue a signal if
    /// there are no active workers (i.e. all threads are waiting on empty BlockingCollection). Or when
    /// the BlockingCollection's item count starts to grow beyond a threshold level.
    ///
    /// This approach minimizes condition variable sleeps, wakes and lock contention. Which in turn,
    /// improves performance and makes it more predictable.
    /// @tparam ThreadContainerPolicy The type of thread container.
    /// @see BlockingCollectionConditionVariable
    /// @see NotFullConditionVariable
    template<class ThreadContainerPolicy>
    class NotEmptyConditionVariable : public BlockingCollectionConditionVariable<ThreadContainerPolicy>
    {
    public:
        using base = BlockingCollectionConditionVariable<ThreadContainerPolicy>;
        
        /// Initializes a new instance of the NotEmptyConditionVariable<T> class
        NotEmptyConditionVariable()
        : base() {
            
        }
        
        ~NotEmptyConditionVariable() {
        
        }
        
        /// Wakes up a thread waiting on this condition variable.
        void Signal() {
            
            size_t total = base::Total();
            
            if (total == 0) // no threads attached to this condition variable!
                return;
            
            const size_t kItemsPerConsumer = 16;
            
            size_t active = base::Active();
            
            // issue a signal only when there are no active workers, or
            // when the count starts to grow beyond a threshold level
            
            if (active == 0 || (active < total && base::Count() / active > kItemsPerConsumer))
            {
                base::IncrementActive();
                
                base::Condition().notify_one();
            }
        }
        
        /// Wakes up all threads waiting on this condition variable.
        void Broadcast() {
            
            size_t total = base::Total();
            
            if (total == 0) // no threads attached to this condition variable!
                return;
            
            base::Active(total);
            
            base::Condition().notify_all();
        }
        
        /// Waits indefinitely for this condition variable to become signaled.
        /// @param lock An object of type std::unique_lock which must be locked by the current thread.
        void Wait(std::unique_lock<std::mutex> &lock) {
            
            base::DecrementActive();
            
            base::Condition().wait(lock);
        }
        
        /// Waits up to specified duration for this condition variable to become signaled.
        /// @param lock An object of type std::unique_lock which must be locked by the current thread.
        /// @param rel_time An object of type std::chrono::duration representing the maximum time to spend waiting.
        template<class Rep, class Period> bool Wait(std::unique_lock<std::mutex> &lock, const std::chrono::duration<Rep, Period>& rel_time) {
            
            base::DecrementActive();
            
            bool timed_out = std::cv_status::timeout == base::Condition().wait_for(lock, rel_time);
            
            if (timed_out) {
                base::IncrementActive();
            }
            
            return timed_out;
        }

    private:
        // "NotEmptyConditionVariable" objects cannot be copied or assigned
        NotEmptyConditionVariable(const NotEmptyConditionVariable&);			// no implementation
        NotEmptyConditionVariable& operator=(const NotEmptyConditionVariable&);	// no implementation
    };

    /// @class NotFullConditionVariable
    /// NotFullConditionVariable is a specialized BlockingCollectionConditionVariable that wraps the
    /// std::condition_variable.
    /// It handles the "not full" condition of the BlockingCollection. It allows threads to wait
    /// on a full BlockingCollection. That is, wait until the BlockingCollection becomes "not full".
    ///
    /// NotFullConditionVariable is optimized for this purpose.
    ///
    /// Optimized means that this condition variable will not signal the underlying std::condition_variable
    /// for every item that is taken from the BlockingCollection. Rather it will only issue a signal if
    /// there are no active workers (i.e. all threads are waiting on a full BlockingCollection). Or when
    /// the BlockingCollection's available capacity starts to grow beyond a threshold level.
    ///
    /// This approach minimizes condition variable sleeps, wakes and lock contention. Which in turn,
    /// improves performance and makes it more predictable.
    /// @tparam ThreadContainerPolicy The type of thread container.
    /// @see BlockingCollectionConditionVariable
    /// @see NotEmptyConditionVariable
    template<class ThreadContainerPolicy>
    class NotFullConditionVariable : public BlockingCollectionConditionVariable<ThreadContainerPolicy>
    {
    public:
        using base = BlockingCollectionConditionVariable<ThreadContainerPolicy>;
        
        /// Initializes a new instance of the NotFullConditionVariable<T> class
        NotFullConditionVariable()
        : base() {
            
        }
        
        ~NotFullConditionVariable() {
            
        }
        
        /// Wakes up a thread waiting on this condition variable.
        void Signal() {
            
            size_t total = base::Total();
            
            if (total == 0)  // no threads attached to this condition variable!
                return;
            
            const size_t kItemsPerProducer = 16;
            
            size_t active = base::Active();
            
            // issue a signal only when there are no active workers, or
            // when the available capacity starts to grow beyond a threshold level
            
            if (active == 0 ||
                (active < total &&
                 (base::BoundedCapacity() - base::Count()) / active > kItemsPerProducer))
            {
                base::IncrementActive();
                base::Condition().notify_one();
            }
        }
        
        /// Wakes up all threads waiting on this condition variable.
        void Broadcast() {
            
            size_t total = base::Total();
            
            if (total == 0) // no threads attached to this condition variable!
                return;
            
            base::Active(total);
            
            base::Condition().notify_all();
        }
        
        /// Waits indefinitely for this condition variable to become signaled.
        /// @param lock An object of type std::unique_lock which must be locked by the current thread.
        void Wait(std::unique_lock<std::mutex> &lock) {
            
            base::DecrementActive();
            
            base::Condition().wait(lock);
        }
        
        /// Waits up to specified duration for this condition variable to become signaled.
        /// @param lock An object of type std::unique_lock which must be locked by the current thread.
        /// @param rel_time An object of type std::chrono::duration representing the maximum time to spend waiting.
        template<class Rep, class Period> bool Wait(std::unique_lock<std::mutex> &lock, const std::chrono::duration<Rep, Period>& rel_time) {
            
            base::DecrementActive();
            
            bool timed_out = std::cv_status::timeout == base::Condition().wait_for(lock, rel_time);
            
            if (timed_out) {
                base::IncrementActive();
            }
            
            return timed_out;
        }
        
    private:
        // "NotFullConditionVariable" objects cannot be copied or assigned
        NotFullConditionVariable(const NotFullConditionVariable&);
        NotFullConditionVariable& operator=(const NotFullConditionVariable&);
    };
    
    /// @cond HIDDEN_SYMBOLS
    
    /// @class ConditionVariableGenerator
    /// Generates the "not full" and "not empty" condition variables for
    /// the specified ThreadContainerPolicy.
    /// @tparam ThreadContainerPolicy The thread container policy to use when
    /// generating the condition variables.
    template<typename ThreadContainerPolicy> class ConditionVariableGenerator {
    public:
        typedef NotFullConditionVariable<ThreadContainerPolicy> not_full_type;
        typedef NotEmptyConditionVariable<ThreadContainerPolicy> not_empty_type;
    };
    
    template <class T>
    struct ThreadContainerTraits;
    
    template <>
    struct ThreadContainerTraits<std::thread::id>
    {
        static std::thread::id Self()
        {
            return std::this_thread::get_id();
        }
    };
    
    /// @class ThreadContainer
    /// This class adds and removes the specified thread type from the container.
    /// @tparam T The thread type.
    template<class T> class ThreadContainer {
    public:
        ThreadContainer() {
            
        }
        
        /// Adds the calling thread to the container.
        /// @returns True if the calling thread was added to container. Otherwise false.
        bool Add() {
            
            T id = traits::Self();
            
            typename std::unordered_set<T>::iterator itr = threadid_.find(id);
            
            if (itr != threadid_.end()) {
                return false;
            }
            
            threadid_.insert(id);
            return true;
        }
        
        /// Removes the calling thread from the container.
        /// @returns True if the calling thread was removed from container. Otherwise false.
        bool Remove() {
            if (threadid_.erase(traits::Self()) > 0) {
                return true;
            }
            return false;
        }
    private:
        std::unordered_set<T> threadid_;
        
        typedef ThreadContainerTraits<T> traits;
    };
    
    /// @class StdConditionVariableGenerator
    typedef ConditionVariableGenerator<ThreadContainer<std::thread::id>> StdConditionVariableGenerator;
    
    template <class T>
    struct ContainerTraits;

    struct QueueType {};
    
    template <>
    struct ContainerTraits<QueueType>
    {
        static bool IsQueue() {
            return true;
        }
    };
    
    struct StackType {};
    
    template <>
    struct ContainerTraits<StackType>
    {
        static bool IsQueue() {
            return false;
        }
    };
    /// @endcond

    /// @class Container
    /// Represents a first in-first out (FIFO) or a last in-first out (LIFO) collection depending on
    /// the ContainerType template parameter value.
    /// Implements the implicitly defined IProducerConsumerCollection<T> policy.
    /// @tparam T The type of items in the container.
    /// @tparam ContainerType The type of container (i.e. Queue or Stack).
    template<class T, class ContainerType>
    class Container{
    public:
        typedef std::deque<T>                            container_type;
        typedef typename container_type::value_type      value_type;
        typedef typename container_type::size_type       size_type;
        
        /// Initializes a new instance of the Container<T> class.
        Container() {
            
        }
        
        /// Gets the number of elements contained in the collection.
        /// @returns The number of elements contained in the collection.
        size_type Count() {
            return container_.size();
        }
        
        /// Attempts to add an object to the collection.
        /// @param new_item The object to add to the collection.
        /// @returns True if the object was added successfully; otherwise, false.
        bool TryAdd(const value_type item) {
            container_.push_back(item);
            return true;
        }
        
        /// Attempts to remove and return an object from the collection.
        /// @param [out] item When this method returns, if the object was removed and returned successfully, item contains
        /// the removed object. If no object was available to be removed, the value is unspecified.
        /// @returns True if an object was removed and returned successfully; otherwise, false.
        bool TryTake(value_type &item) {
            if (container_.empty())
                return false;
            if (traits::IsQueue()) {
                item = container_.front();
                container_.pop_front();
            } else {
                item = container_.back();
                container_.pop_back();
            }
            return true;
        }
        
        /// Copies the elements contained in the collection to a new array.
        /// The method provides a snapshot of the underlying collection. It is possible for other threads to add or remove
        /// items immediately after the array is made.
        /// @returns A new array containing the elements copied from the collection.
        std::vector<value_type> ToArray() {
            return std::vector<value_type>(container_.begin(), container_.end());
        }
    private:
        container_type container_;
        
        typedef ContainerTraits<ContainerType> traits;
    };
    
    /// @class Queue
    /// A type alias for Container<T, QueueType> - a first in-first out (FIFO) container.
    /// @tparam T The type of items in the container.
    template<class T>
    using Queue = Container<T, QueueType>;
    
    /// @class Stack
    /// A type alias for Container<T, StackType> - a last in-first out (LIFO) container.
    /// @tparam T The type of items in the container.
    template<class T>
    using Stack = Container<T, StackType>;

    /// @enum BlockingCollectionState
    /// The BlockCollection states.
    enum class BlockingCollectionState
    {
        Activated = 1, ///< BlockingCollection is active and processing normally.
        Deactivated = 2, ///< BlockingCollection is deactivated; no add or take operations allowed.
        Pulsed = 3 ///< BlockingCollection was pulsed; add and take may proceed normally.
    };

    /// @enum BlockingCollectionStatus
    /// The BlockCollection status codes.
    /// These are the status codes returned by all of BlockingCollection's Add and Take operations.
    enum class BlockingCollectionStatus {
        Ok = 0, ///< Operation succeeded
        AddingCompleted = -1, ///< Operation failed due to CompleteAdding() having been invoked
        TimedOut = -2, ///< Operation failed due to time out
        NotActivated = -3, ///< Operation failed due to BlockingCollection not being activated
        Completed = -4, ///< Operation failed due to BlockingCollection being completed
        InvalidIterators = -5, ///< Operation failed due to invalid iterators
        CompleteAddingConcurrent = -6, ///< Operation failed due to concurrent Add and CompleteAdding
        InvalidDuration = -7, ///< Operation failed due to invalid timeout duration parameter value
        InternalError = -8 ///< Operation failed due to BlockingCollection container error
    };

    /// @class BlockingCollection
    /// Provides blocking and bounding capabilities for collections that implement the
    /// implicitly defined IProducerConsumerCollection<T> container policy.
    /// @code{.cpp}
    /// template<class T> class IProducerConsumerCollection {
    /// public:
    ///     IProducerConsumerCollection() {
    ///     }
    ///
    ///     size_t Count() {
    ///         return 0;
    ///     }
    ///
    ///     bool TryAdd(const T item) {
    ///         return false;
    ///     }
    ///
    ///     bool TryTake(T &item) {
    ///         return false;
    ///     }
    ///
    ///     std::vector<T> ToArray() {
    ///         return std::vector<T>();
    ///     }
    ///
    /// private:
    /// };
    /// @endcode
    /// BlockingCollection<T> is a thread safe collection class that provides the following:
    /// - Implementation of Producer/Consumer pattern; BlockingCollection<T> is a wrapper for the
    /// implicitly defined IProducerConsumerCollection<T> container policy.
    /// - Pluggable container support (Queue, PriorityQueue & Stack included).
    /// - Concurrent addition and removal of items from multiple threads with the Add and Take methods.
    /// - Batching support for addition of multiple items at once.
    /// - A bounded collection that blocks Add and Take operations when the collection is full or empty.
    /// - Add and Take method timeout support.
    /// - Minimizes sleeps, wake ups and lock contention by managing an active subset of producer and consumer
    /// threads.
    /// - Range-based loop support.
    ///
    /// BlockingCollection<T> is used as a wrapper for an IProducerConsumerCollection<T> container policy,
    /// and allows removal attempts from the collection to block until data is available to be removed.
    /// Similarly, you can create a
    /// BlockingCollection<T> to enforce an upper bound on the number of data elements allowed in the
    /// IProducerConsumerCollection<T>; addition attempts to the collection may then block until space is available to
    /// store the added items. In this manner, BlockingCollection<T> is similar to a traditional blocking queue data
    /// structure, except that the underlying data storage mechanism is abstracted away as an
    /// IProducerConsumerCollection<T> container policy.
    ///
    /// BlockingCollection<T> supports bounding and blocking. Bounding means that you can set the maximum capacity
    /// of the collection. Bounding is important in certain scenarios because it enables you to control the maximum
    /// size of the collection in memory, and it prevents the producing threads from moving too far ahead of the
    /// consuming threads. Multiple threads or tasks can add items to the collection concurrently, and if the
    /// collection reaches its specified maximum capacity, the producing threads will block until an item is
    /// removed. Multiple consumers can remove items concurrently, and if the collection becomes empty, the
    /// consuming threads will block until a producer adds an item. A producing thread can call the
    /// CompleteAdding method to indicate that no more items will be added. Consumers monitor the IsCompleted
    /// property to know when the collection is empty and no more items will be added.
    /// @code{.cpp}
    /// // creates BlockingCollection<int> with bounded capacity of 100 ints.
    ///
    /// BlockingCollection<int> collection(100);
    /// @endcode
    /// When you create a BlockingCollection<T> object, you can specify not only the bounded capacity but also
    /// the type of collection to use. For example, you could specify a Queue<T> object for first in, first out
    /// (FIFO) behavior, or a Stack<T> object for last in, first out (LIFO) behavior. You can use any collection class
    /// that implements the IProducerConsumerCollection<T> container policy.
    ///
    /// At moment there are three supported IProducerConsumerCollection<T> container policies:
    /// - Queue<T>
    /// - Stack<T>
    /// - PriorityQueue<T>
    ///
    /// The default collection type for BlockingCollection<T> is Queue<T>.
    /// @code{.cpp}
    /// BlockingCollection<int> collection; // defaults to Queue<T>
    ///
    /// BlockingCollection<int, Queue> queue;
    ///
    /// BlockingCollection<int, Stack> stack;
    ///
    /// BlockingCollection<int, PriorityQueue> priority_queue;
    ///
    /// // There are also type alias available....
    ///
    /// BlockingQueue<int> blocking_queue;
    ///
    /// BlockingStack<int> blocking_stack;
    ///
    /// BlockingPriorityQueue<int> blocking_priority_queue;
    /// @endcode
    /// BlockingCollection<T> maintains a subset of active threads that are currently adding and taking items.
    /// This is important because it allows BlockingCollection<T> not to have to issue a signal each time an item
    /// is added or taken. Instead it issues a signal only when an item is added or taken and there are no
    /// active consumers, or when the container's item count starts to grow beyond a threshold level. Or the
    /// container's available capacity starts to grow beyond a threshold level. This approach greatly improves
    /// performance and makes it more predictable.
    ///
    /// In order for BlockingCollection<T> to maintain a subset of active threads all threads that will be adding or
    /// taking items from the BlockingCollection<T> must register with it. The BlockingCollectio guard classes
    /// (i.e. ProducerGuard<T> and ConsumerGuard<T> can be used for this purpose. Both guard classes will detach
    /// the thread from the BlockingCollection<T> when the thread terminates. As well as in exception scenarios.
    /// @code{.cpp}
    /// BlockingCollection<int> collection;
    ///
    /// std::thread consumer_thread([&collection]() {
    ///
    ///     ConsumerGuard<BlockingCollection<int>> guard(collection);
    ///
    ///     int item;
    ///
    ///     for(int i = 0; i < 10; i++)
    ///         collection.Take(item);
    /// });
    /// @endcode
    /// @code{.cpp}
    /// BlockingCollection<int> collection;
    ///
    /// std::thread producer_thread([&collection]() {
    ///
    ///     ProducerGuard<BlockingCollection<int>> guard(collection);
    ///
    ///     for(int i = 0; i < 10; i++)
    ///         collection.Take(i+1);
    /// });
    /// @endcode
    /// @see ProducerGuard
    /// @see ConsumerGuard
    /// @version 1.0.0
    template <class T, template<class> class ContainerPolicy = Queue,
    class ConditionVariableGenerator = StdConditionVariableGenerator>
    class BlockingCollection
    {
    public:
        typedef ContainerPolicy<T>                       container_type;
        typedef typename container_type::value_type      value_type;
        typedef typename container_type::size_type       size_type;
        
        /// Initializes a new instance of the BlockingCollection<T> class without an upper-bound.
        BlockingCollection()
        : BlockingCollection(SIZE_MAX) {
        }
        
        /// Initializes a new instance of the BlockingCollection<T> class with the specified upper-bound.
        /// @param capacity The bounded size of the collection.
        BlockingCollection(size_t capacity)
        : state_(BlockingCollectionState::Activated), bounded_capacity_(capacity), is_adding_completed_(false)
        {
            not_empty_condition_var_.BoundedCapacity(capacity);
            not_full_condition_var_.BoundedCapacity(capacity);
        }
        
        ~BlockingCollection() {
        }
        
        /// Gets the bounded capacity of this BlockingCollection<T> instance.
        /// @return The bounded capacity of the collection.
        size_t BoundedCapacity() {
            std::lock_guard<std::mutex> lock(mutex_);
            return bounded_capacity_;
        }

        /// Gets the current state of this BlockingCollection<T> instance.
        /// @return The current state of the collection.
        /// @see BlockingCollectionState
        BlockingCollectionState State() {
            std::lock_guard<std::mutex> lock(mutex_);
            return state_;
        }

        /// Gets whether this BlockingCollection<T> instance is full.
        /// @return True if the collection is full; otherwise false.
        bool IsFull() {
            std::lock_guard<std::mutex> lock(mutex_);
            return IsFull_i();
        }

        /// Gets whether this BlockingCollection<T> instance is empty.
        /// @return True if the collection is empty; otherwise false.
        bool IsEmpty() {
            std::lock_guard<std::mutex> lock(mutex_);
            return IsEmpty_i();
        }

        /// Gets the number of items contained in the BlockingCollection<T> instance.
        /// If any method in BlockingCollection is executing while the Count property is being accessd, the return value
        /// is approximate. Count may reflect a number that is either greater than or less than the actual number of
        /// items in the BlockingCollection.
        /// @return The number of item in the collection.
        size_t Count() {
            std::lock_guard<std::mutex> lock(mutex_);
            return container_.Count();
        }

        /// Gets whether this BlockingCollection<T> instance has been deactivated.
        /// @return True is this collection has been deactivated. Otherwise false.
        bool IsDeactivated() {
            std::lock_guard<std::mutex> lock(mutex_);
            return state_ == BlockingCollectionState::Deactivated;
        }

        /// Gets whether this BlockingCollection<T> instance has been marked as complete for adding and is empty.
        /// @return True if this collection has been marked as complete for adding and is empty. Otherwise false.
        bool IsCompleted() {
            std::lock_guard<std::mutex> lock(mutex_);
            return IsCompleted_i();
        }

        /// Gets whether this BlockingCollection<T> instance has been marked as complete for adding.
        /// @return True if this collection has been marked as complete for adding. Otherwise false.
        bool IsAddingCompleted() {
            std::lock_guard<std::mutex> lock(mutex_);
            return IsAddingCompleted_i();
        }

        /// Pulses this BlockingCollection<T> instance to wake up any waiting threads.
        /// Changes the collection's state to Pulsed. Future Add and Take operations proceed
        /// as in the Activated state.
        /// @return The BlockingCollection's state before this call.
        /// @see BlockingCollectionState
        BlockingCollectionState Pulse() {
            std::lock_guard<std::mutex> lock(mutex_);
            return Deactivate_i(true);
        }

        /// Deactivate this BlockingCollection<T> instance and wakeup all threads waiting
        /// on the collection so they can continue. No items are removed from the collection,
        /// however. Any other operations called until the collection is
        /// activated again will immediately return BlockingCollectionStatus::NotActivated.
        /// @return The BlockingCollection's state before this call.
        /// @see BlockingCollectionState
        /// @see BlockingCollectionStatus
        BlockingCollectionState Deactivate() {
            std::lock_guard<std::mutex> lock(mutex_);
            return Deactivate_i(false);
        }

        /// Reactivate this BlockingCollection<T> instance so that threads can Add and Take
        /// items again.
        /// @return The BlockingCollection's state before this call.
        /// @see BlockingCollectionState
        BlockingCollectionState Activate() {
            std::lock_guard<std::mutex> lock(mutex_);
            return Activate_i();
        }

        /// Releases all items from this BlockingCollection<T> instance
        /// but does not mark it deactivated.
        /// @return The number of items flushed.
        size_t Flush() {
            std::lock_guard<std::mutex> lock(mutex_);

            size_t itemsFlushed = container_.Count();

            T item;
            
            while (container_.Count() > 0) {
                container_.TryTake(item);
            }

            not_empty_condition_var_.Count(0);
            not_full_condition_var_.Count(0);

            return itemsFlushed;
        }

        /// Marks the BlockingCollection<T> instances as not accepting any more additions.
        /// After a collection has been marked as complete for adding, adding to the collection
        /// is not permitted and attempts to remove from the collection will not wait when the collection is empty.
        void CompleteAdding() {
            std::lock_guard<std::mutex> lock(mutex_);

            if (is_adding_completed_)
                return;

            is_adding_completed_ = true;

            not_empty_condition_var_.Broadcast();
            not_full_condition_var_.Broadcast();
        }
        
        /// Gets the number of consumer threads that are actively taking items
        /// from this BlockingCollection<T> instance.
        /// @return The number of active consumer threads.
        /// @see AttachConsumer
        size_t ActiveConsumers() {
            std::lock_guard<std::mutex> lock(mutex_);
            return not_empty_condition_var_.Active();
        }
        
        /// Gets the number of producer threads that are actively adding items
        /// to this BlockingCollection<T> instance.
        /// @return The number of active producer threads.
        /// @see AttachProducer
        size_t ActiveProducers() {
            std::lock_guard<std::mutex> lock(mutex_);
            return not_full_condition_var_.Active();
        }
        
        /// Gets the total number of consumer threads that can take items
        /// from this BlockingCollection<T> instance.
        /// @return The total number of consumer threads.
        /// @see AttachConsumer
        size_t TotalConsumers() {
            std::lock_guard<std::mutex> lock(mutex_);
            return not_empty_condition_var_.Total();
        }
        
        /// Gets the total number of producer threads that can add items
        /// to this BlockingCollection<T> instance.
        /// @return The total number of producer threads.
        /// @see AttachProducer
        size_t TotalProducers() {
            std::lock_guard<std::mutex> lock(mutex_);
            return not_full_condition_var_.Total();
        }
        
        /// Registers a consumer thread with this BlockingCollection<T> instance.
        /// @see TotalConsumers
        void AttachConsumer() {        
            std::lock_guard<std::mutex> lock(mutex_);        
            not_empty_condition_var_.Attach();
        }
        
        /// Unregisters a consumer thread with this BlockingCollection<T> instance.
        /// @see TotalConsumers
        void DetachConsumer() {        
            std::lock_guard<std::mutex> lock(mutex_);        
            not_empty_condition_var_.Detach();
        }

        /// Registers a producer thread with this BlockingCollection<T> instance.
        /// @see TotalProducers
        void AttachProducer() {
            std::lock_guard<std::mutex> lock(mutex_);
            not_full_condition_var_.Attach();
        }

        /// Unregisters a producer thread with this BlockingCollection<T> instance.
        /// @see TotalProducers
        void DetachProducer() {
            std::lock_guard<std::mutex> lock(mutex_);
            not_full_condition_var_.Detach();
        }    
        
        /// Adds the item to the BlockingCollection<T>.
        /// If a bounded capacity was specified when this instance of BlockingCollection<T> was initialized,
        /// a call to Add may block until space is available to store the provided item.
        /// @param item The item to be added to the collection. The value can be a null reference.
        void Add(const T item) {
            TryAdd(item, std::chrono::milliseconds(-1));
        }
        
        /// Tries to add the specified item to the BlockingCollection<T>.
        /// If the collection is a bounded collection, and is full, this method immediately
        /// returns without adding the item.
        /// @param item The item to be added to the collection.
        /// @return A BlockCollectionStatus code.
        /// @see BlockingCollectionStatus
        BlockingCollectionStatus TryAdd(const T item) {
            return TryAdd(item, std::chrono::milliseconds(0));
        }

        /// Tries to add the specified item to the BlockingCollection<T> within the specified time period.
        /// @param item The item to be added to the collection.
        /// @param rel_time An object of type std::chrono::duration representing the maximum time to spend waiting.
        /// @return A BlockCollectionStatus code.
        /// @see BlockingCollectionStatus
        /// @see http://en.cppreference.com/w/cpp/chrono/duration
        template<class Rep, class Period> BlockingCollectionStatus TryAdd(const T item, const std::chrono::duration<Rep, Period>& rel_time) {
            
            if (rel_time < std::chrono::duration<Rep, Period>(-1))
                return BlockingCollectionStatus::InvalidDuration;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                
                BlockingCollectionStatus status = WaitNotFullCondition(lock, rel_time);
                
                if (BlockingCollectionStatus::Ok != status)
                    return status;
                
                container_.TryAdd(item);
                
                Signal(container_.Count(), false);
            }
            
            return BlockingCollectionStatus::Ok;
        }

        /// Removes an item from the BlockingCollection<T>.
        /// A call to Take may block until an item is available to be removed.
        /// @param[out] item The item removed from the collection.
        void Take(T &item) {
            TryTake(item, std::chrono::milliseconds(-1));
        }
        
        /// Tries to remove an item from the BlockingCollection<T>.
        /// If the collection is empty, this method immediately returns without taking an item.
        /// @param[out] item The item removed from the collection.
        /// @return A BlockCollectionStatus code.
        /// @see BlockingCollectionStatus
        BlockingCollectionStatus TryTake(T &item) {
            return TryTake(item, std::chrono::milliseconds(0));
        }
        
        /// Tries to remove an item from the BlockingCollection<T> in the specified time period.
        /// @param[out] item The item removed from the collection.
        /// @param rel_time An object of type std::chrono::duration representing the maximum time to spend waiting.
        /// @return A BlockCollectionStatus code.
        /// @see BlockingCollectionStatus
        /// @see http://en.cppreference.com/w/cpp/chrono/duration
        template<class Rep, class Period> BlockingCollectionStatus TryTake(T &item, const std::chrono::duration<Rep, Period>& rel_time) {
            
            if (rel_time < std::chrono::duration<Rep, Period>(-1))
                return BlockingCollectionStatus::InvalidDuration;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                
                BlockingCollectionStatus status = WaitNotEmptyCondition(lock, rel_time);
                
                if (BlockingCollectionStatus::Ok != status)
                    return status;
                
                if(!container_.TryTake(item))
                   return BlockingCollectionStatus::InternalError;
                
                Signal(container_.Count(), true);
            }
            
            return BlockingCollectionStatus::Ok;
        }

        /// Adds the items from range [first, last] to the BlockingCollection<T>.
        /// If a bounded capacity was specified when this instance of BlockingCollection<T> was initialized,
        /// a call to Add may block until space is available to store the provided items.
        /// @param first The start range of elements to insert.
        /// @param last The end range of elements to insert.
        template <typename Iterator> void Add(Iterator first, Iterator last) {
            TryAdd(first, last, std::chrono::milliseconds(-1));
        }
        
        /// Tries to add the items from range [first, last] to the BlockingCollection<T>.
        /// If the collection is a bounded collection, and is full, this method immediately
        /// returns without adding the items.
        /// @param first The start range of elements to insert.
        /// @param last The end range of elements to insert.
        /// @return A BlockCollectionStatus code.
        /// @see BlockingCollectionStatus
        template <typename Iterator> BlockingCollectionStatus TryAdd(Iterator first, Iterator last) {
            return TryAdd(first, last, std::chrono::milliseconds(0));
        }
        
        /// Tries to add the specified items from the range [first, last] to the BlockingCollection<T> within
        /// the specified time period.
        /// @param first The start range of elements to insert.
        /// @param last The end range of elements to insert.
        /// @param rel_time An object of type std::chrono::duration representing the maximum time to spend waiting.
        /// @return A BlockCollectionStatus code.
        /// @see BlockingCollectionStatus
        /// @see http://en.cppreference.com/w/cpp/chrono/durations
        template <typename Iterator, class Rep, class Period> BlockingCollectionStatus TryAdd(Iterator first, Iterator last, const std::chrono::duration<Rep, Period>& rel_time){
            
            if (rel_time < std::chrono::duration<Rep, Period>(-1))
                return BlockingCollectionStatus::InvalidDuration;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                
                BlockingCollectionStatus status = WaitNotFullCondition(lock, rel_time);
                
                if (BlockingCollectionStatus::Ok != status)
                    return status;
                
                if (first == last)
                    return BlockingCollectionStatus::InvalidIterators;
                
                for (; first != last; ++first) {
                    container_.TryAdd((*first));
                }
                
                Signal(container_.Count(), false);
            }
            
            return BlockingCollectionStatus::Ok;
        }
        
        /// Copies the items from the BlockingCollection<T> instance into a new array.
        ///
        /// The copied elements are not removed from the collection.
        ///
        /// If any method in BlockingCollection is executing while the ToArray method
        /// executes, the return value is approximate. ToArray  may include items that
        /// have already been removed, or exclude items that have already been inserted.
        /// @return An array containing copies of the elements of the collection.
        std::vector<T> ToArray() {
            std::unique_lock<std::mutex> lock(mutex_);
            return container_.ToArray();
        }

    private:
        
        class Iterator {
        public:
            Iterator(BlockingCollection<T, ContainerPolicy, ConditionVariableGenerator> &collection)
            : collection_(collection), status_(BlockingCollectionStatus::Ok), item_not_initialized_(true) {
            }
            
            T& operator*() {
                return item_;
            }
            
            Iterator& operator++() {
                status_ = collection_.TryTake(item_, std::chrono::milliseconds(-1));
                return *this;
            }
            
            bool operator!=(const Iterator& it)  {
                
                if (item_not_initialized_) {
                    item_not_initialized_ = false;
                    status_ = collection_.TryTake(item_, std::chrono::milliseconds(-1));
                }
                
                return !(status_ != BlockingCollectionStatus::Ok);
            }
        private:
            // "Iterator" objects cannot be copied or assigned
            Iterator(const BlockingCollection&);			// no implementation
            Iterator& operator=(const Iterator&);	// no implementation

            BlockingCollection<T, ContainerPolicy, ConditionVariableGenerator> &collection_;
            BlockingCollectionStatus status_;
            bool item_not_initialized_;
            T item_;
        };
     
    public:
        
        Iterator begin() { return { *this }; }
        Iterator end()   { return { *this }; }
        
    protected:
        
        // the member functions below assume lock is held!
        
        /// The implementation for the Deactivate method.
        /// This method is not thread safe.
        /// @see Deactivate
        BlockingCollectionState Deactivate_i (bool pulse)
        {
            BlockingCollectionState previous_state = state_;
            
            if (previous_state != BlockingCollectionState::Deactivated) {
                if(pulse)
                    state_ = BlockingCollectionState::Pulsed;
                else
                    state_ = BlockingCollectionState::Deactivated;
                
                not_empty_condition_var_.Broadcast();
                not_full_condition_var_.Broadcast();
            }
            
            return previous_state;
        }
        
        /// The implementation for the Activate method.
        /// This method is not thread safe.
        /// @see Activate
        BlockingCollectionState Activate_i () {
            BlockingCollectionState previous_state = state_;
            
            state_ = BlockingCollectionState::Activated;
            
            return previous_state;
        }
        
        /// The implementation for the IsFull method.
        /// This method is not thread safe.
        /// @see IsFull
        bool IsFull_i() {
            return bounded_capacity_ != SIZE_MAX && container_.Count() >= bounded_capacity_;
        }
        
        /// The implementation for the IsEmpty method.
        /// This method is not thread safe.
        /// @see IsEmpty
        bool IsEmpty_i() {
            return container_.Count() == 0;
        }
        
        /// The implementation for the IsCompleted method.
        /// This method is not thread safe.
        /// @see IsCompleted
        bool IsCompleted_i() {
            return is_adding_completed_ && IsEmpty_i();
        }

        /// The implementation for the IsAddingCompleted method.
        /// This method is not thread safe.
        /// @see IsAddingCompleted
        bool IsAddingCompleted_i() {
            return is_adding_completed_;
        }

        /// Wraps the condition variable Signal methods.
        /// This method updates the Count property on both
        /// condition variables before invoking the Signal
        /// method on the specified condition variable.
        void Signal(size_t itemCount, bool signal_not_full) {
            
            not_empty_condition_var_.Count(itemCount);
            not_full_condition_var_.Count(itemCount);
            
            if (signal_not_full) {
                // signal only if capacity is bounded
                if (bounded_capacity_ != SIZE_MAX) {
                    not_full_condition_var_.Signal();
                }
            } else {
                not_empty_condition_var_.Signal();
            }
        }
        
        /// The method waits on the "not full" condition variable whenever the collection
        /// becomes full.
        /// It atomically releases lock, blocks the current executing thread, and adds it to the
        /// list of threads waiting on the "not full" condition variable. The thread will be unblocked
        /// when notify_all() or notify_one() is executed, or when the relative timeout rel_time expires.
        /// It may also be unblocked spuriously. When unblocked, regardless of the reason, lock is reacquired
        /// and WaitNotFullCondition() exits.
        /// @param lock An object of type std::unique_lock which must be locked by the current thread.
        /// @param rel_time An object of type std::chrono::duration representing the maximum time to spend waiting.
        /// @return A BlockCollectionStatus code.
        /// @see BlockingCollectionStatus
        /// @see http://en.cppreference.com/w/cpp/chrono/duration
        template<class Rep, class Period> BlockingCollectionStatus WaitNotFullCondition(std::unique_lock<std::mutex> &lock, const std::chrono::duration<Rep, Period>& rel_time) {
            
            if (state_ == BlockingCollectionState::Deactivated)
                return BlockingCollectionStatus::NotActivated;

            if (IsAddingCompleted_i())
                return BlockingCollectionStatus::AddingCompleted;
            
            BlockingCollectionStatus status = BlockingCollectionStatus::Ok;
            
            while (IsFull_i()) {
                if (rel_time == rel_time.zero()) {
                    status = BlockingCollectionStatus::TimedOut;
                    break;
                }
                
                if (IsAddingCompleted_i()) {
                    status = BlockingCollectionStatus::AddingCompleted;
                    break;
                }
                
                not_full_condition_var_.Count(container_.Count());
                
                if (rel_time < rel_time.zero()) {
                    not_full_condition_var_.Wait(lock);
                } else {
                    bool timedout = not_full_condition_var_.Wait(lock, rel_time);
                    
                    if (timedout) {
                        status = BlockingCollectionStatus::TimedOut;
                        break;
                    }
                }
                
                // Add/TryAdd methods and CompleteAdding should not
                // be called concurrently - invalid operation
                
                if (IsAddingCompleted_i()) {
                    status = BlockingCollectionStatus::CompleteAddingConcurrent;
                    break;
                }
                
                if (state_ != BlockingCollectionState::Activated) {
                    status = BlockingCollectionStatus::NotActivated;
                    break;
                }
            }
            
            return status;
        }

        /// The method waits on the "not empty" condition variable whenever the collection
        /// becomes empty.
        /// It atomically releases lock, blocks the current executing thread, and adds it to the
        /// list of threads waiting on the "not empty" condition variable. The thread will be unblocked
        /// when notify_all() or notify_one() is executed, or when the relative timeout rel_time expires.
        /// It may also be unblocked spuriously. When unblocked, regardless of the reason, lock is reacquired
        /// and WaitNotEmptyCondition() exits.
        /// @param lock An object of type std::unique_lock which must be locked by the current thread.
        /// @param rel_time An object of type std::chrono::duration representing the maximum time to spend waiting.
        /// @return A BlockCollectionStatus code.
        /// @see BlockingCollectionStatus
        /// @see http://en.cppreference.com/w/cpp/chrono/duration
        template<class Rep, class Period> BlockingCollectionStatus WaitNotEmptyCondition(std::unique_lock<std::mutex> &lock, const std::chrono::duration<Rep, Period>& rel_time) {
            
            if (state_ == BlockingCollectionState::Deactivated)
                return BlockingCollectionStatus::NotActivated;
            
            if (IsCompleted_i())
                return BlockingCollectionStatus::Completed;
     
            BlockingCollectionStatus status = BlockingCollectionStatus::Ok;
            
            while (IsEmpty_i()) {
                if (rel_time == rel_time.zero()) {
                    status = BlockingCollectionStatus::TimedOut;
                    break;
                }
                
                if (IsAddingCompleted_i()) {
                    status = BlockingCollectionStatus::AddingCompleted;
                    break;
                }
                
                not_empty_condition_var_.Count(container_.Count());
                
                if (rel_time < rel_time.zero()) {
                    not_empty_condition_var_.Wait(lock);
                } else {
                    bool timedout = not_empty_condition_var_.Wait(lock, rel_time);
                    
                    if (timedout) {
                        status = BlockingCollectionStatus::TimedOut;
                        break;
                    }
                }
                
                if (state_ != BlockingCollectionState::Activated) {
                    status = BlockingCollectionStatus::NotActivated;
                    break;
                }
            }
            
            return status;
        }

    private:
        // "BlockingCollection" objects cannot be copied or assigned
        BlockingCollection(const BlockingCollection&);			// no implementation
        BlockingCollection& operator=(const BlockingCollection&);	// no implementation
     
        BlockingCollectionState state_;
        
        size_t bounded_capacity_;
        bool is_adding_completed_;
        
        typename ConditionVariableGenerator::not_empty_type not_empty_condition_var_;
        typename ConditionVariableGenerator::not_full_type not_full_condition_var_;

    protected:
        std::mutex mutex_; ///< Synchronizes access to the BlockCollection.
        container_type container_; ///< The underlying container (e.g. Queue, Stack).
    };
    
    /// @class PriorityContainer
    /// Represents a priority based collection. Items with the highest priority will be at the head of the collection.
    /// Implements the implicitly defined IProducerConsumerCollection<T> policy.
    /// @tparam T The type of items in the collection.
    /// @tparam ComparerPolicy The type of comparer to use when comparing items.
    template<class T, template<class> class ComparerPolicy>
    class PriorityContainer{
    public:
        typedef std::deque<T>                            container_type;
        typedef typename container_type::size_type       size_type;
        typedef typename container_type::value_type      value_type;
        typedef ComparerPolicy<T>                        value_comparer;
        
        /// Initializes a new instance of the PriorityContainer<T> class.
        PriorityContainer() {
            
        }
        
        /// Gets the number of elements contained in the collection.
        /// @returns The number of elements contained in the collection.
        size_type Count() {
            return container_.size();
        }
        
        /// Attempts to add an object to the collection according to the item's priority.
        /// @param new_item The object to add to the collection.
        /// @returns True if the object was added successfully; otherwise, false.
        bool TryAdd(const value_type new_item) {
            if (container_.empty()) {
                container_.push_front(new_item);
            } else {
                
                // search from back to front (i.e. from the lowest priority to the highest priority) for
                // item with a priority greater than or equal to new_item's priority
                
                typename container_type::reverse_iterator itr = std::find_if(container_.rbegin(), container_.rend(), [&new_item, this](value_type &item) {
                    return this->comparer_.Compare(item, new_item) >= 0;
                });
                
                if (itr == container_.rend()) {
                    // if at end then new_item's priority is now the highest
                    container_.push_front(new_item);
                } else if (itr == container_.rbegin()) {
                    // if at start then new_item's priority is now the lowest
                    container_.push_back(new_item);
                } else {
                    // insert the new item behind the item of greater or
                    // equal priority. This ensures that FIFO order is
                    // maintained when items of the same priority are
                    // inserted consecutively.
                    container_.insert(itr.base(), new_item);
                }
            }

            return true;
        }
        
        /// Attempts to remove and return the highest priority object from the collection.
        /// @param [out] item When this method returns, if the object was removed and returned successfully, item contains
        /// the removed object. If no object was available to be removed, the value is unspecified.
        /// @returns True if an object was removed and returned successfully; otherwise, false.
        bool TryTake(value_type &item) {
            if (container_.empty())
                return false;
            item = container_.front();
            container_.pop_front();
            return true;
        }
        
        /// Attempts to remove and return the lowest priority object from the collection.
        /// @param [out] item When this method returns, if the object was removed and returned successfully, item contains
        /// the removed object. If no object was available to be removed, the value is unspecified.
        /// @returns True if an object was removed and returned successfully; otherwise, false.
        bool TryTakePrio(value_type &item) {
            if (container_.empty())
                return false;
            
            bool init_current_priority = true;
            value_type* current_priority;
            
            typename container_type::reverse_iterator itr = std::find_if_not(container_.rbegin(), container_.rend(), [&current_priority, &init_current_priority, this](value_type &item) -> bool {
                
                // Find the first version of the earliest item (i.e.,
                // preserve FIFO order for items at the same priority).
                
                if(init_current_priority) {
                    current_priority = &item;
                    init_current_priority = false;
                    return true;
                }
                bool continue_search = this->comparer_.Compare(item, *current_priority) <= 0;
                if(continue_search) {
                    current_priority = &item;
                }
                return continue_search;
            });
            
            if (itr == container_.rend()) {
                item = container_.front();
                container_.pop_front();
            } else {
                typename container_type::iterator base = itr.base();
                item = (*base);
                container_.erase(base);
            }

            return true;
        }
        
        /// Copies the elements contained in the collection to a new array.
        /// The method provides a snapshot of the underlying collection. It is possible for other threads to add or remove
        /// items immediately after the array is made.
        /// @returns A new array containing the elements copied from the collection.
        std::vector<value_type> ToArray() {
            return std::vector<value_type>(container_.begin(), container_.end());
        }
    private:
        container_type container_;
        value_comparer comparer_;
    };
    
    /// @class Comparer
    /// This is the default PriorityContainer comparer.
    /// It expects that the objects being compared have overloaded < and > operators.
    /// @tparam T The type of objects to compare.
    template<class T> class Comparer
    {
    public:
        /// Initializes a new instance of the Comparer<T> class.
        Comparer() {
            
        }
        
        /// Compares two objects and returns a value indicating whether one is less than, equal to, or greater than the
        /// other.
        /// Implement this method to provide a customized sort order comparison for type T.
        /// @param x The first object to compare.
        /// @param y The second object to compare.
        /// @return A signed integer that indicates the relative values of x and y, as shown in the following table.
        ///
        /// Value             | Meaning
        /// ------------------|---------------------
        /// Less than zero    | x is less than y.
        /// Zero              | x equals y.
        /// Greater than zero | x is greater than y.
        ///
        int Compare(const T &x, const T &y) {
            
            if (x < y)
                return -1;
            else if (x > y)
                return 1;
            else
                return 0;
        }
    };

    /// @class PriorityQueue
    /// A type alias for PriorityContainer<T, Comparer> - a priority-based container.
    /// @tparam T The type of items in the container.
    template <typename T>
    using PriorityQueue = PriorityContainer<T, Comparer>;
    
    /// @class PriorityBlockingCollection
    /// PriorityBlockingCollection offers the same functionality found in BlockingCollection<T, PriorityQueue>.
    /// The Add/TryAdd methods add items to collection based on the their priority - (0 is lowest priority).
    /// FIFO order is maintained when items of the same priority are added consecutively. And the Take/TryTake methods
    /// return the highest priority items in FIFO order.
    ///
    /// In addition, PriorityBlockingCollection adds additional methods (i.e. TakePrio/TryTakePrio) for taking
    /// the lowest priority items.
    ///
    /// PriorityBlockingCollection's default priority comparer expects that the objects being compared have
    /// overloaded < and > operators. If this is not the case then you can provide your own comparer implementation
    /// like so.
    /// @code{.cpp}
    /// class PriorityItem {
    /// public:
    ///     PriorityItem(int priority) : priority_(priority) {}
    ///
    ///     int Priority() const {
    ///         return priority_;
    ///     }
    ///
    /// private:
    ///     int priority_;
    /// };
    ///
    /// template<class T>
    /// class CustomComparer {
    /// public:
    ///     CustomComparer() {
    ///     }
    ///
    ///     int Compare(const PriorityItem &item, const PriorityItem &new_item) {
    ///         if (item->Priority() < new_item->Priority())
    ///             return -1;
    ///         else if (item->Priority() > new_item->Priority())
    ///             return 1;
    ///         else
    ///             return 0;
    ///     }
    /// };
    ///
    /// template<typename T>
    /// using CustomPriorityQueue = PriorityContainer<T, CustomComparer>;
    ///
    /// PriorityBlockingCollection<PriorityItem, CustomPriorityQueue> collection;
    ///
    /// @endcode
    /// @tparam T The type of objects in the PriorityBlockingCollection.
    /// @tparam ContainerPolicy The priority-based container.
    /// @tparam ConditionVariableGenerator The condition variable generator.
    /// @see BlockingCollection
    /// @see Comparer
    /// @version 1.0.0
    template<class T, template<class> class ContainerPolicy = PriorityQueue,
    class ConditionVariableGenerator = StdConditionVariableGenerator>
    class PriorityBlockingCollection : public BlockingCollection<T, ContainerPolicy, ConditionVariableGenerator>
    {
    public:
        using base = BlockingCollection<T, ContainerPolicy, ConditionVariableGenerator>;

        /// Initializes a new instance of the PriorityBlockingCollection<T> class without an upper-bound.
        PriorityBlockingCollection()
        : base() {
        }
        
        /// Initializes a new instance of the PriorityBlockingCollection<T> class with the specified upper-bound.
        /// @param capacity The bounded size of the collection.
        PriorityBlockingCollection(size_t capacity)
        : base(capacity) {
        }
        
        /// Removes the lowest priority item from the PriorityBlockingCollection<T>.
        /// A call to TakePrio may block until an item is available to be removed.
        /// @param[out] item The lowest priority item removed from the collection.
        void TakePrio(T &item) {
            TryTakePrio(item, std::chrono::milliseconds(-1));
        }
        
        /// Tries to remove the lowest priority item from the PriorityBlockingCollection<T>.
        /// If the collection is empty, this method immediately returns immediately without taking an item.
        /// @param[out] item The lowest priority item removed from the collection.
        /// @return A BlockCollectionStatus code.
        /// @see BlockingCollectionStatus
        BlockingCollectionStatus TryTakePrio(T &item) {
            return TryTakePrio(item, std::chrono::milliseconds(0));
        }
        
        /// Tries to remove the lowest priority item from the PriorityBlockingCollection<T> in the specified time period.
        /// @param[out] item The lowest priority item removed from the collection.
        /// @param rel_time An object of type std::chrono::duration representing the maximum time to spend waiting.
        /// @return A BlockCollectionStatus code.
        /// @see BlockingCollectionStatus
        /// @see http://en.cppreference.com/w/cpp/chrono/duration
        template<class Rep, class Period> BlockingCollectionStatus TryTakePrio(T &item, const std::chrono::duration<Rep, Period>& rel_time) {
            
            if(rel_time < std::chrono::duration<Rep, Period>(-1))
                return BlockingCollectionStatus::InvalidDuration;
            {
                std::unique_lock<std::mutex> lock(this->mutex_);
                
                BlockingCollectionStatus status = base::WaitNotEmptyCondition(lock, rel_time);
                
                if (BlockingCollectionStatus::Ok != status)
                    return status;
                
                if (!this->container_.TryTakePrio(item))
                    return BlockingCollectionStatus::InternalError;
                
                base::Signal(this->container_.Count(), true);
            }
            
            return BlockingCollectionStatus::Ok;
        }

    private:
        // "PriorityBlockingCollection" objects cannot be copied or assigned
        PriorityBlockingCollection(const PriorityBlockingCollection&);			// no implementation
        PriorityBlockingCollection& operator=(const PriorityBlockingCollection&);	// no implementation
    };
    
    /// @cond HIDDEN_SYMBOLS
    template <class T>
    struct GuardTraits;
    
    struct ProducerType {};

    template <>
    struct GuardTraits<ProducerType>
    {
        static bool IsProducer() {
            return true;
        }
    };
    
    struct ConsumerType {};
    
    template <>
    struct GuardTraits<ConsumerType>
    {
        static bool IsProducer() {
            return false;
        }
    };
    /// @endcond
    
    /// @class Guard
    /// Implements a strictly scope-based BlockingCollection wrapper.
    /// The class Guard is a BlockingCollection wrapper that provides a convenient RAII-style
    /// mechanism for attaching the current thread as a producer or consumer to the BlockingCollection for the
    /// duration of the scoped block.
    ///
    /// When a Guard object is created, it attaches the current thread as a producer or consumer of the
    /// BlockingCollection it is given. When control leaves the scope in which the Guard object
    /// was created, the Guard is destructed and the current thread is detached from the BlockingCollection.
    ///
    /// The Guard class makes it simple for threads to register as producer or consumers with the BlockingCollection<T>
    /// instance. Plus it ensures the thread will be detached from the BlockingCollection<T> in an
    /// exception scenario.
    ///
    /// The Guard class is non-copyable.
    /// @tparam BlockingCollection The type of BlockingCollection to guard.
    /// @tparam GuardType The type of Guard to create (i.e. ProducerType or ConsumerType).
    /// @see ProducerGuard
    /// @see ConsumerGuard
    /// @see http://en.wikipedia.com/wiki/Resource_Acquisition_Is_Initialization
    template<class BlockingCollection, class GuardType> class Guard {
    public:
        typedef BlockingCollection collection_type;
        
        explicit Guard(collection_type &collection)
        : collection_(collection) {
            if (traits::IsProducer())
                collection_.AttachProducer();
            else
                collection_.AttachConsumer();
        }
        
        ~Guard() {
            if (traits::IsProducer())
                collection_.DetachProducer();
            else
                collection_.DetachConsumer();
        }
        
    private:
        Guard(Guard const&); // = delete;
        Guard& operator=(Guard const&); // = delete;
        
        typedef GuardTraits<GuardType> traits;
        
        collection_type &collection_;
    };
    
    /// @class ProducerGuard
    /// A type alias for Guard<T, ProducerType> - a RAII-style mechanism for attaching the current thread as a producer.
    /// ProducerGuard will call BlockingCollection<T>'s AttachProducer() on construction. And
    /// DetachProducer() on destruction.
    /// @code{.cpp}
    /// BlockingCollection<int> collection;
    ///
    /// std::thread producer_thread([&collection]() {
    ///
    ///     int item = 10;
    ///
    ///     ProducerGuard<BlockingCollection<int>> guard(collection);
    ///
    ///     collection.Add(item);
    /// });
    /// @endcode
    /// @tparam T The BlockingCollection<T> to guard.
    /// @see Guard
    /// @see http://en.wikipedia.com/wiki/Resource_Acquisition_Is_Initialization
    template<class T>
    using ProducerGuard = Guard<T, ProducerType>;
    
    /// @class ConsumerGuard
    /// A type alias for Guard<T, ConsumerType> - a RAII-style mechanism for attaching the current thread as a consumer.
    /// ConsumerGuard will call BlockingCollection<T>'s AttachConsumer() on construction. And
    /// DetachConsumer() on destruction.
    /// @code{.cpp}
    /// BlockingCollection<int> collection;
    ///
    /// std::thread consumer_thread([&collection]() {
    ///
    ///     int item;
    ///
    ///     ConsumerGuard<BlockingCollection<int>> guard(collection);
    ///
    ///     collection.Take(item);
    /// });
    /// @endcode
    /// @tparam T The BlockingCollection<T> to guard.
    /// @see Guard
    /// @see http://en.wikipedia.com/wiki/Resource_Acquisition_Is_Initialization
    template<class T>
    using ConsumerGuard = Guard<T, ConsumerType>;
    
    /// @class BlockingStack
    /// A type alias for BlockingCollection<T, Stack> - a last in-first out (LIFO) BlockingCollection.
    template<class T>
    using BlockingStack = BlockingCollection<T, Stack>;
    
    /// @class BlockingQueue
    /// A type alias for BlockingCollection<T, Queue> - a first in-first out (FIFO) BlockingCollection.
    template<class T>
    using BlockingQueue = BlockingCollection<T, Queue>;
    
    /// @class BlockingPriorityQueue
    /// A type alias for BlockingCollection<T, PriorityQueue> - a priority-based BlockingCollection.
    template<class T>
    using BlockingPriorityQueue = BlockingCollection<T, PriorityQueue>;
    
    /** \example producer_consumer.cpp
     * This is an example of how to use the BlockingCollection class in a multi producer consumer scenario.
     *  This examples spins up the specified number of producer and consumer threads. It then goes about
     * processing the specified number of items. And finally outputs the total processing time.
     */
}

#endif /* BlockingCollection_h */
