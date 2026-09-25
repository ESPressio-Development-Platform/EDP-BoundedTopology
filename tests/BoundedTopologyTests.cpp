#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <ESPressio_BoundedTopology.hpp>

namespace {

    struct RecordSpace final {
    };

    struct OtherSpace final {
    };

    using Index0 = ESPressio::BoundedTopology::BoundedIndex<RecordSpace, 0U>;
    using Index8 = ESPressio::BoundedTopology::BoundedIndex<RecordSpace, 8U>;
    using Index255 = ESPressio::BoundedTopology::BoundedIndex<RecordSpace, 255U>;
    using Index256 = ESPressio::BoundedTopology::BoundedIndex<RecordSpace, 256U>;
    using Index65535 = ESPressio::BoundedTopology::BoundedIndex<RecordSpace, 65535U>;
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

    using Set1 = ESPressio::BoundedTopology::BoundedIndexSet<RecordSpace, 1U>;
    using Set8 = ESPressio::BoundedTopology::BoundedIndexSet<RecordSpace, 8U>;
    using Set9 = ESPressio::BoundedTopology::BoundedIndexSet<RecordSpace, 9U>;
    using Set255 = ESPressio::BoundedTopology::BoundedIndexSet<RecordSpace, 255U>;
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

    struct ZeroSetHolder final {

        [[no_unique_address]] Set0 Set;

        std::uint8_t Marker = 0U;

    };

    static_assert(
        sizeof(ZeroSetHolder) == 1U
    );

    using Queue8 = ESPressio::BoundedTopology::IntrusiveQueue<RecordSpace, 8U>;
    using Queue256 = ESPressio::BoundedTopology::IntrusiveQueue<RecordSpace, 256U>;
    using Queue0 = ESPressio::BoundedTopology::IntrusiveQueue<RecordSpace, 0U>;

    static_assert(
        sizeof(Queue8) == 2U
    );

    static_assert(
        sizeof(Queue256) == 4U
    );

    struct ZeroQueueHolder final {

        [[no_unique_address]] Queue0 Queue;

        std::uint8_t Marker = 0U;

    };

    static_assert(
        sizeof(ZeroQueueHolder) == 1U
    );


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


    template<class TIndex>
    struct QueueRecord final {

        TIndex Next = TIndex::Invalid();

        constexpr TIndex QueueNext() const noexcept {
            return Next;
        }

        constexpr void SetQueueNext(
            TIndex index
        ) noexcept {
            Next = index;
        }

    };


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


int main() {
    ValidateQueue();
    ValidateZeroCapacity();

    return 0;
}
