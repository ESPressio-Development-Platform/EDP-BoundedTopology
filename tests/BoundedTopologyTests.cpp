#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <ESPressio_BoundedTopology.hpp>

namespace {

    /// Semantic index-space tag used by the majority of host tests.
    struct RecordSpace final {
    };


    /// Distinct semantic index-space tag used to prove compile-time separation.
    struct OtherSpace final {
    };


    // Capacity-derived index aliases.

    /// Zero-capacity index used to validate the smallest retained representation.
    using Index0 = ESPressio::BoundedTopology::BoundedIndex<RecordSpace, 0U>;
    /// Eight-entry index used for ordinary one-byte behavior tests.
    using Index8 = ESPressio::BoundedTopology::BoundedIndex<RecordSpace, 8U>;
    /// Largest capacity retaining a one-byte index plus Invalid sentinel.
    using Index255 = ESPressio::BoundedTopology::BoundedIndex<RecordSpace, 255U>;
    /// First capacity requiring a two-byte index representation.
    using Index256 = ESPressio::BoundedTopology::BoundedIndex<RecordSpace, 256U>;
    /// Largest capacity retaining a two-byte index plus Invalid sentinel.
    using Index65535 = ESPressio::BoundedTopology::BoundedIndex<RecordSpace, 65535U>;
    /// First capacity requiring a four-byte index representation.
    using Index65536 = ESPressio::BoundedTopology::BoundedIndex<RecordSpace, 65536U>;

    static_assert(
        sizeof(Index0) == 1U
    );

    static_assert(
        sizeof(Index8) == 1U
    );

    static_assert(
        sizeof(Index255) == 1U
    );

    static_assert(
        sizeof(Index256) == 2U
    );

    static_assert(
        sizeof(Index65535) == 2U
    );

    static_assert(
        sizeof(Index65536) == 4U
    );

    static_assert(
        !std::is_same_v<
            ESPressio::BoundedTopology::BoundedIndex<RecordSpace, 8U>,
            ESPressio::BoundedTopology::BoundedIndex<OtherSpace, 8U>
        >
    );

    static_assert(
        std::is_trivially_copyable_v<Index8>
    );

    // Index-set footprint aliases.

    /// One-member set used to validate one-byte minimum positive storage.
    using Set1 = ESPressio::BoundedTopology::BoundedIndexSet<RecordSpace, 1U>;
    /// Eight-member set used to validate one complete bitmap byte.
    using Set8 = ESPressio::BoundedTopology::BoundedIndexSet<RecordSpace, 8U>;
    /// Nine-member set used to validate expansion into a second bitmap byte.
    using Set9 = ESPressio::BoundedTopology::BoundedIndexSet<RecordSpace, 9U>;
    /// Representative larger set used to validate exact ceil(N/8) storage.
    using Set255 = ESPressio::BoundedTopology::BoundedIndexSet<RecordSpace, 255U>;
    /// Zero-capacity set used to validate empty-state composition.
    using Set0 = ESPressio::BoundedTopology::BoundedIndexSet<RecordSpace, 0U>;

    static_assert(
        sizeof(Set1) == 1U
    );

    static_assert(
        sizeof(Set8) == 1U
    );

    static_assert(
        sizeof(Set9) == 2U
    );

    static_assert(
        sizeof(Set255) == 32U
    );

    /// Composition fixture proving a zero-capacity set need not increase owner size.
    struct ZeroSetHolder final {

        // Composed state.

        /// Empty set permitted to overlap another member under C++20 empty-state composition.
        [[no_unique_address]] Set0 Set;

        /// One-byte owner state establishing the expected complete fixture size.
        std::uint8_t Marker = 0U;

    };

    static_assert(
        sizeof(ZeroSetHolder) == 1U
    );

    // Intrusive-queue footprint aliases.

    /// One-byte-index queue used to validate two-byte endpoint storage.
    using Queue8 = ESPressio::BoundedTopology::IntrusiveQueue<RecordSpace, 8U>;
    /// Two-byte-index queue used to validate four-byte endpoint storage.
    using Queue256 = ESPressio::BoundedTopology::IntrusiveQueue<RecordSpace, 256U>;
    /// Zero-capacity queue used to validate empty-state composition.
    using Queue0 = ESPressio::BoundedTopology::IntrusiveQueue<RecordSpace, 0U>;

    static_assert(
        sizeof(Queue8) == 2U
    );

    static_assert(
        sizeof(Queue256) == 4U
    );

    /// Composition fixture proving a zero-capacity queue need not increase owner size.
    struct ZeroQueueHolder final {

        // Composed state.

        /// Empty queue permitted to overlap another member under C++20 empty-state composition.
        [[no_unique_address]] Queue0 Queue;

        /// One-byte owner state establishing the expected complete fixture size.
        std::uint8_t Marker = 0U;

    };

    static_assert(
        sizeof(ZeroQueueHolder) == 1U
    );


    /// Validates checked construction, Invalid state and failure preservation for bounded indices.
    constexpr bool ValidateIndex() noexcept {
        using Result = ESPressio::BoundedTopology::BoundedIndexCreationResult;

        Index8 index;

        if (index.IsValid()) {
            return false;
        }

        if (
            Index8::TryCreate(
                7U,
                index
            ) != Result::Succeeded
        ) {
            return false;
        }

        if (!index.IsValid() || index.Value() != 7U) {
            return false;
        }

        if (
            Index8::TryCreate(
                8U,
                index
            ) != Result::OutOfRange
        ) {
            return false;
        }

        return index.Value() == 7U;
    }

    static_assert(
        ValidateIndex()
    );


    /// Validates membership mutation, scans, full/empty state and invalid-index rejection.
    constexpr bool ValidateIndexSet() noexcept {
        using Result = ESPressio::BoundedTopology::BoundedIndexSetMutationResult;
        using Set = ESPressio::BoundedTopology::BoundedIndexSet<RecordSpace, 10U>;
        using Index = typename Set::Index;

        Set set;

        if (!set.IsEmpty() || set.Count() != 0U) {
            return false;
        }

        if (
            set.Set(
                Index::FromUnchecked(
                    3U
                )
            ) != Result::Succeeded
        ) {
            return false;
        }

        if (
            set.Set(
                Index::FromUnchecked(
                    9U
                )
            ) != Result::Succeeded
        ) {
            return false;
        }

        if (
            set.Set(
                Index::Invalid()
            ) != Result::InvalidIndex
        ) {
            return false;
        }

        if (set.Count() != 2U) {
            return false;
        }

        if (set.FindFirstSet().Value() != 3U) {
            return false;
        }

        if (set.FindFirstClear().Value() != 0U) {
            return false;
        }

        set.SetAll();

        if (set.Count() != 10U || set.FindFirstClear().IsValid()) {
            return false;
        }

        if (
            set.Clear(
                Index::FromUnchecked(
                    4U
                )
            ) != Result::Succeeded
        ) {
            return false;
        }

        if (set.Count() != 9U || set.FindFirstClear().Value() != 4U) {
            return false;
        }

        set.ClearAll();

        return set.IsEmpty() && !set.FindFirstSet().IsValid();
    }

    static_assert(
        ValidateIndexSet()
    );


    /// Externally owned record fixture satisfying the intrusive queue linkage contract.
    ///
    /// @tparam TIndex Strong bounded index Type used for this record collection.
    template<class TIndex>
    struct QueueRecord final {

        // Intrusive linkage.

        /// Next record in FIFO order, or Invalid when no record follows.
        TIndex Next = TIndex::Invalid();


        // Queue linkage access.

        /// Returns this record's externally owned next-link.
        constexpr TIndex QueueNext() const noexcept {
            return Next;
        }

        /// Replaces this record's externally owned next-link.
        constexpr void SetQueueNext(
            TIndex index
        ) noexcept {
            Next = index;
        }

    };


    /// Validates FIFO append/pop ordering, specific removal and invalid-index rejection.
    void ValidateQueue() {
        using Queue = ESPressio::BoundedTopology::IntrusiveQueue<RecordSpace, 4U>;
        using Index = typename Queue::Index;
        using PushResult = ESPressio::BoundedTopology::IntrusiveQueuePushResult;
        using PopResult = ESPressio::BoundedTopology::IntrusiveQueuePopResult;
        using RemoveResult = ESPressio::BoundedTopology::IntrusiveQueueRemoveResult;

        std::array<QueueRecord<Index>, 4U> records{};
        Queue queue;

        assert(
            queue.IsEmpty()
        );

        assert(
            queue.Push(
                records,
                Index::FromUnchecked(
                    2U
                )
            ) == PushResult::Succeeded
        );

        assert(
            queue.Push(
                records,
                Index::FromUnchecked(
                    0U
                )
            ) == PushResult::Succeeded
        );

        assert(
            queue.Push(
                records,
                Index::FromUnchecked(
                    3U
                )
            ) == PushResult::Succeeded
        );

        assert(
            queue.Head().Value() == 2U
        );

        assert(
            queue.Tail().Value() == 3U
        );

        assert(
            queue.Remove(
                records,
                Index::FromUnchecked(
                    0U
                )
            ) == RemoveResult::Removed
        );

        Index output = Index::Invalid();

        assert(
            queue.Pop(
                records,
                output
            ) == PopResult::Succeeded
        );

        assert(
            output.Value() == 2U
        );

        assert(
            queue.Pop(
                records,
                output
            ) == PopResult::Succeeded
        );

        assert(
            output.Value() == 3U
        );

        assert(
            queue.Pop(
                records,
                output
            ) == PopResult::Empty
        );

        assert(
            queue.IsEmpty()
        );

        assert(
            queue.Push(
                records,
                Index::Invalid()
            ) == PushResult::InvalidIndex
        );

        assert(
            queue.Remove(
                records,
                Index::Invalid()
            ) == RemoveResult::InvalidIndex
        );
    }


    /// Validates stateless behavior for zero-capacity set and queue specializations.
    void ValidateZeroCapacity() {
        using Set = ESPressio::BoundedTopology::BoundedIndexSet<RecordSpace, 0U>;
        using Queue = ESPressio::BoundedTopology::IntrusiveQueue<RecordSpace, 0U>;
        using Index = typename Queue::Index;

        Set set;
        Queue queue;
        std::array<QueueRecord<Index>, 0U> records{};
        Index output = Index::Invalid();

        assert(
            set.IsEmpty()
        );

        assert(
            set.Count() == 0U
        );

        assert(
            !set.FindFirstSet().IsValid()
        );

        assert(
            queue.IsEmpty()
        );

        assert(
            queue.Pop(
                records,
                output
            ) == ESPressio::BoundedTopology::IntrusiveQueuePopResult::Empty
        );
    }

} // anonymous namespace


/// Executes the host behavior suite after all compile-time footprint checks have instantiated.
int main() {
    ValidateQueue();
    ValidateZeroCapacity();

    return 0;
}
