#pragma once

#include <cstddef>
#include <cstdint>

#include "BoundedIndex.hpp"

namespace ESPressio::BoundedTopology {

    /// Describes the outcome of appending one record to an intrusive queue.
    enum class IntrusiveQueuePushResult : std::uint8_t {

        /// The record was appended to FIFO order.
        Succeeded = 0,

        /// The supplied record index is invalid.
        InvalidIndex = 1

    };


    /// Describes the outcome of removing the FIFO head from an intrusive queue.
    enum class IntrusiveQueuePopResult : std::uint8_t {

        /// One record was removed and published to the caller's output.
        Succeeded = 0,

        /// The queue contained no record.
        Empty = 1

    };


    /// Describes the outcome of removing a specific record from an intrusive queue.
    enum class IntrusiveQueueRemoveResult : std::uint8_t {

        /// The requested record was removed.
        Removed = 0,

        /// The requested valid record was not present in the queue.
        NotFound = 1,

        /// The supplied record index is invalid.
        InvalidIndex = 2

    };


    /// Non-owning FIFO topology over externally owned records in one positive bounded index space.
    ///
    /// Records supplied to queue operations must expose QueueNext() returning Index and
    /// SetQueueNext(Index) for their externally owned intrusive linkage.
    ///
    /// @tparam TIndexSpace Compile-time semantic tag shared by queue and record indices.
    /// @tparam TCapacity Number of valid record indices. This primary template represents positive capacity.
    template<class TIndexSpace, std::size_t TCapacity>
    class IntrusiveQueue final {

        static_assert(
            TCapacity > 0U,
            "The positive-capacity IntrusiveQueue specialization requires capacity greater than zero."
        );

        public:

            // Public Type metadata.

            /// Strong bounded index Type used by this FIFO topology.
            using Index = BoundedIndex<TIndexSpace, TCapacity>;

        private:

            // FIFO endpoints.

            /// First queued record, or Invalid when the queue is empty.
            Index _head = Index::Invalid();

            /// Last queued record, or Invalid when the queue is empty.
            Index _tail = Index::Invalid();

        public:

            // Construction.

            /// Creates an empty intrusive FIFO.
            constexpr IntrusiveQueue() noexcept = default;


            // Queue inspection.

            /// Indicates whether the queue currently contains no record.
            constexpr bool IsEmpty() const noexcept {
                return !_head.IsValid();
            }

            /// Returns the current FIFO head, or Invalid when empty.
            constexpr Index Head() const noexcept {
                return _head;
            }

            /// Returns the current FIFO tail, or Invalid when empty.
            constexpr Index Tail() const noexcept {
                return _tail;
            }


            // Queue mutation.

            /// Appends one externally owned record to FIFO order.
            ///
            /// The caller must ensure the record is not already linked into this queue.
            ///
            /// @tparam TRecords Externally owned indexed record collection supporting operator[].
            template<class TRecords>
            constexpr IntrusiveQueuePushResult Push(
                TRecords& records,
                Index recordIndex
            ) noexcept {
                if (!recordIndex.IsValid()) {
                    return IntrusiveQueuePushResult::InvalidIndex;
                }

                records[
                    static_cast<std::size_t>(
                        recordIndex.Value()
                    )
                ].SetQueueNext(
                    Index::Invalid()
                );

                if (!_tail.IsValid()) {
                    _head = recordIndex;
                    _tail = recordIndex;
                    return IntrusiveQueuePushResult::Succeeded;
                }

                records[
                    static_cast<std::size_t>(
                        _tail.Value()
                    )
                ].SetQueueNext(
                    recordIndex
                );

                _tail = recordIndex;

                return IntrusiveQueuePushResult::Succeeded;
            }

            /// Removes the FIFO head and publishes its index to caller-owned output.
            ///
            /// The output remains unchanged when the queue is empty.
            ///
            /// @tparam TRecords Externally owned indexed record collection supporting operator[].
            template<class TRecords>
            constexpr IntrusiveQueuePopResult Pop(
                TRecords& records,
                Index& output
            ) noexcept {
                if (!_head.IsValid()) {
                    return IntrusiveQueuePopResult::Empty;
                }

                const auto removed = _head;
                const auto next = records[
                    static_cast<std::size_t>(
                        removed.Value()
                    )
                ].QueueNext();

                _head = next;

                if (!_head.IsValid()) {
                    _tail = Index::Invalid();
                }

                records[
                    static_cast<std::size_t>(
                        removed.Value()
                    )
                ].SetQueueNext(
                    Index::Invalid()
                );

                output = removed;

                return IntrusiveQueuePopResult::Succeeded;
            }

            /// Removes one specific queued record while preserving the order of all other records.
            ///
            /// Traversal is explicitly bounded by TCapacity so corrupted/cyclic external linkage cannot
            /// turn this operation into an unbounded loop.
            ///
            /// @tparam TRecords Externally owned indexed record collection supporting operator[].
            template<class TRecords>
            constexpr IntrusiveQueueRemoveResult Remove(
                TRecords& records,
                Index recordIndex
            ) noexcept {
                if (!recordIndex.IsValid()) {
                    return IntrusiveQueueRemoveResult::InvalidIndex;
                }

                auto previous = Index::Invalid();
                auto current = _head;

                for (std::size_t visited = 0U; visited < TCapacity && current.IsValid(); ++visited) {
                    if (current == recordIndex) {
                        const auto next = records[
                            static_cast<std::size_t>(
                                current.Value()
                            )
                        ].QueueNext();

                        if (previous.IsValid()) {
                            records[
                                static_cast<std::size_t>(
                                    previous.Value()
                                )
                            ].SetQueueNext(
                                next
                            );
                        } else {
                            _head = next;
                        }

                        if (_tail == current) {
                            _tail = previous;
                        }

                        records[
                            static_cast<std::size_t>(
                                current.Value()
                            )
                        ].SetQueueNext(
                            Index::Invalid()
                        );

                        return IntrusiveQueueRemoveResult::Removed;
                    }

                    previous = current;
                    current = records[
                        static_cast<std::size_t>(
                            current.Value()
                        )
                    ].QueueNext();
                }

                return IntrusiveQueueRemoveResult::NotFound;
            }

    };


    /// Zero-capacity intrusive FIFO topology retaining no endpoint state.
    ///
    /// @tparam TIndexSpace Compile-time semantic tag retained only for Type identity.
    template<class TIndexSpace>
    class IntrusiveQueue<TIndexSpace, 0U> final {

        public:

            // Public Type metadata.

            /// Zero-capacity strong bounded index Type.
            using Index = BoundedIndex<TIndexSpace, 0U>;


            // Construction.

            /// Creates the stateless zero-capacity FIFO.
            constexpr IntrusiveQueue() noexcept = default;


            // Queue inspection.

            /// Reports true because a zero-capacity FIFO can never contain a record.
            constexpr bool IsEmpty() const noexcept {
                return true;
            }

            /// Returns Invalid because a zero-capacity FIFO has no head.
            constexpr Index Head() const noexcept {
                return Index::Invalid();
            }

            /// Returns Invalid because a zero-capacity FIFO has no tail.
            constexpr Index Tail() const noexcept {
                return Index::Invalid();
            }


            // Queue mutation.

            /// Rejects append because no valid record index exists.
            ///
            /// @tparam TRecords Externally owned record collection Type; never accessed for zero capacity.
            template<class TRecords>
            constexpr IntrusiveQueuePushResult Push(
                TRecords&,
                Index
            ) noexcept {
                return IntrusiveQueuePushResult::InvalidIndex;
            }

            /// Reports empty without touching caller-owned records or output.
            ///
            /// @tparam TRecords Externally owned record collection Type; never accessed for zero capacity.
            template<class TRecords>
            constexpr IntrusiveQueuePopResult Pop(
                TRecords&,
                Index&
            ) noexcept {
                return IntrusiveQueuePopResult::Empty;
            }

            /// Rejects removal because no valid record index exists.
            ///
            /// @tparam TRecords Externally owned record collection Type; never accessed for zero capacity.
            template<class TRecords>
            constexpr IntrusiveQueueRemoveResult Remove(
                TRecords&,
                Index
            ) noexcept {
                return IntrusiveQueueRemoveResult::InvalidIndex;
            }

    };

} // ESPressio::BoundedTopology
