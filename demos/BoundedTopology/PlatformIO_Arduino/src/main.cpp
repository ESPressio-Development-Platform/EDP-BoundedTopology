#include <Arduino.h>

#include <array>

#include <ESPressio_BoundedTopology.hpp>

namespace Demo {

    /// Semantic index-space tag used by this demonstration.
    struct SlotSpace final {
    };

    /// Four-entry stable slot topology used by this demonstration.
    using Slots = ESPressio::BoundedTopology::BoundedSlotTopology<SlotSpace, 4U>;

    /// Four-entry intrusive FIFO used by this demonstration.
    using Queue = ESPressio::BoundedTopology::IntrusiveQueue<SlotSpace, 4U>;

    /// Strong index Type shared by stable slots, membership and queue topology.
    using Index = typename Slots::Index;

    /// External record whose queue linkage is owned by the record rather than the queue.
    struct Record final {

        // Intrusive linkage.

        /// Next queued record, or Invalid when this record is not linked onward.
        Index Next = Index::Invalid();

        // Queue linkage access.

        /// Returns the next externally owned record in FIFO order.
        constexpr Index QueueNext() const noexcept {
            return Next;
        }

        /// Replaces the externally owned FIFO next-link.
        constexpr void SetQueueNext(
            Index index
        ) noexcept {
            Next = index;
        }

    };


    /// Exercises stable acquisition, compact membership and intrusive FIFO ordering without allocation.
    bool Run() noexcept {
        using Set = ESPressio::BoundedTopology::BoundedIndexSet<SlotSpace, 4U>;

        Slots slots;
        Set active;
        std::array<Record, 4U> records{};
        Queue queue;

        const auto acquisition = slots.Acquire();

        if (!acquisition.IsSucceeded()) {
            return false;
        }

        const auto claimed = acquisition.AcquiredIndex();

        if (
            active.Set(
                claimed
            ) != ESPressio::BoundedTopology::BoundedIndexSetMutationResult::Succeeded
        ) {
            return false;
        }

        if (
            queue.Push(
                records,
                claimed
            ) != ESPressio::BoundedTopology::IntrusiveQueuePushResult::Succeeded
        ) {
            return false;
        }

        Index removed = Index::Invalid();

        if (
            queue.Pop(
                records,
                removed
            ) != ESPressio::BoundedTopology::IntrusiveQueuePopResult::Succeeded
        ) {
            return false;
        }

        if (
            active.Clear(
                removed
            ) != ESPressio::BoundedTopology::BoundedIndexSetMutationResult::Succeeded
        ) {
            return false;
        }

        if (
            slots.Release(
                removed
            ) != ESPressio::BoundedTopology::BoundedSlotReleaseResult::Succeeded
        ) {
            return false;
        }

        return
            removed == claimed &&
            slots.IsEmpty() &&
            active.IsEmpty() &&
            queue.IsEmpty();
    }

} // Demo

/// Runs the bounded-topology demonstration once after serial initialization.
void setup() {
    Serial.begin(
        115200
    );

    const auto passed = Demo::Run();

    Serial.println(
        passed ? "EDP-BoundedTopology demo: PASS" : "EDP-BoundedTopology demo: FAIL"
    );
}


/// Performs no repeated work; the demonstration completes during setup().
void loop() {
}
