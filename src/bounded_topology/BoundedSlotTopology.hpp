#pragma once

#include <cstddef>
#include <cstdint>

#include "BoundedIndexSet.hpp"

namespace ESPressio::BoundedTopology {

    /// Describes the outcome of acquiring one slot from a bounded slot topology.
    enum class BoundedSlotAcquisitionStatus : std::uint8_t {

        /// One free slot was acquired successfully.
        Succeeded = 0,

        /// Every representable slot is already occupied.
        Full = 1

    };


    /// Describes the outcome of releasing one slot from a bounded slot topology.
    enum class BoundedSlotReleaseResult : std::uint8_t {

        /// The occupied slot was released successfully.
        Succeeded = 0,

        /// The supplied bounded index is invalid for this topology.
        InvalidIndex = 1,

        /// The supplied slot is valid but is not currently occupied.
        NotOccupied = 2

    };


    /// Forward declaration permitting acquisition results to be constructed only by their matching topology.
    ///
    /// @tparam TIndexSpace Compile-time semantic tag distinguishing this slot space from unrelated spaces.
    /// @tparam TCapacity Number of stable slots represented by this topology.
    template<class TIndexSpace, std::size_t TCapacity>
    class BoundedSlotTopology;


    /// Strong structured result returned by bounded slot acquisition.
    ///
    /// @tparam TIndexSpace Compile-time semantic tag shared with the owning bounded slot topology.
    /// @tparam TCapacity Number of stable slots represented by the owning bounded slot topology.
    template<class TIndexSpace, std::size_t TCapacity>
    class BoundedSlotAcquisitionResult final {

        private:

            // Result Type aliases.

            /// Strong slot index Type carried only when acquisition succeeds.
            using IndexType = BoundedIndex<TIndexSpace, TCapacity>;


            // Result state.

            /// Operational outcome of the acquisition attempt.
            BoundedSlotAcquisitionStatus _status;

            /// Acquired slot when successful, or Invalid when no slot was acquired.
            IndexType _index;


            // Controlled result construction.

            /// Creates one internally consistent acquisition result.
            constexpr BoundedSlotAcquisitionResult(
                BoundedSlotAcquisitionStatus status,
                IndexType index
            ) noexcept :
                _status(status),
                _index(index) {
            }

            /// Allows only the matching slot topology to construct acquisition results.
            friend class BoundedSlotTopology<TIndexSpace, TCapacity>;

        public:

            // Public Type metadata.

            /// Strong slot index Type used by this acquisition result.
            using Index = IndexType;


            // Result inspection.

            /// Returns the operational acquisition outcome.
            constexpr BoundedSlotAcquisitionStatus Status() const noexcept {
                return _status;
            }

            /// Indicates whether one slot was acquired successfully.
            constexpr bool IsSucceeded() const noexcept {
                return _status == BoundedSlotAcquisitionStatus::Succeeded;
            }

            /// Returns the acquired slot, or Invalid when acquisition did not succeed.
            constexpr Index AcquiredIndex() const noexcept {
                return _index;
            }

    };


    /// Payload-agnostic stable bounded slot topology over a compile-time finite index space.
    ///
    /// The topology owns occupancy only. Payload storage, payload lifetime, synchronization,
    /// stale-reference protection, generation identity and domain meaning remain external.
    ///
    /// @tparam TIndexSpace Compile-time semantic tag distinguishing this slot space from unrelated spaces.
    /// @tparam TCapacity Number of stable slots represented by this topology.
    template<class TIndexSpace, std::size_t TCapacity>
    class BoundedSlotTopology final {

        private:

            // Internal Type aliases.

            /// Compact one-bit-per-slot occupancy representation.
            using OccupancySet = BoundedIndexSet<TIndexSpace, TCapacity>;


            // Stable slot occupancy state.

            /// Authoritative occupancy bitmap. Empty zero-capacity state may overlap owner storage.
            [[no_unique_address]] OccupancySet _occupied;

        public:

            // Public Type metadata.

            /// Strong stable slot index Type represented by this topology.
            using Index = BoundedIndex<TIndexSpace, TCapacity>;

            /// Structured result returned by Acquire().
            using AcquisitionResult = BoundedSlotAcquisitionResult<TIndexSpace, TCapacity>;

            /// Exact persistent semantic bytes required for occupancy state.
            static constexpr std::size_t StorageBytes = OccupancySet::StorageBytes;


            // Construction.

            /// Creates an empty bounded slot topology.
            constexpr BoundedSlotTopology() noexcept = default;


            // Slot lifecycle.

            /// Acquires the numerically lowest currently free slot.
            ///
            /// The acquired slot remains occupied and stable until explicitly released.
            constexpr AcquisitionResult Acquire() noexcept {
                const auto index = _occupied.FindFirstClear();

                if (!index.IsValid()) {
                    return AcquisitionResult(
                        BoundedSlotAcquisitionStatus::Full,
                        Index::Invalid()
                    );
                }

                _occupied.Set(
                    index
                );

                return AcquisitionResult(
                    BoundedSlotAcquisitionStatus::Succeeded,
                    index
                );
            }

            /// Releases one occupied slot without affecting any externally owned payload.
            constexpr BoundedSlotReleaseResult Release(
                Index index
            ) noexcept {
                if (!index.IsValid()) {
                    return BoundedSlotReleaseResult::InvalidIndex;
                }

                if (!_occupied.IsSet(
                    index
                )) {
                    return BoundedSlotReleaseResult::NotOccupied;
                }

                _occupied.Clear(
                    index
                );

                return BoundedSlotReleaseResult::Succeeded;
            }


            // Occupancy inspection.

            /// Returns the compile-time number of slots represented by this topology.
            static constexpr std::size_t Capacity() noexcept {
                return TCapacity;
            }

            /// Indicates whether one valid slot is currently occupied.
            constexpr bool IsOccupied(
                Index index
            ) const noexcept {
                return _occupied.IsSet(
                    index
                );
            }

            /// Indicates whether no represented slot is currently occupied.
            constexpr bool IsEmpty() const noexcept {
                return _occupied.IsEmpty();
            }

            /// Indicates whether every represented slot is currently occupied.
            constexpr bool IsFull() const noexcept {
                return !_occupied.FindFirstClear().IsValid();
            }

            /// Calculates the number of occupied slots without retaining a cached count.
            constexpr std::size_t Count() const noexcept {
                return _occupied.Count();
            }


            // Stateless occupied-slot traversal.

            /// Returns the numerically lowest occupied slot, or Invalid when none is occupied.
            constexpr Index FindFirstOccupied() const noexcept {
                return _occupied.FindFirstSet();
            }

            /// Returns the next occupied slot with a numeric value greater than the supplied valid index.
            ///
            /// The supplied index does not itself need to be occupied. Invalid input returns Invalid.
            constexpr Index FindNextOccupied(
                Index index
            ) const noexcept {
                if (!index.IsValid()) {
                    return Index::Invalid();
                }

                for (
                    std::size_t indexValue = static_cast<std::size_t>(
                        index.Value()
                    ) + 1U;
                    indexValue < TCapacity;
                    ++indexValue
                ) {
                    const auto candidate = Index::FromUnchecked(
                        indexValue
                    );

                    if (_occupied.IsSet(
                        candidate
                    )) {
                        return candidate;
                    }
                }

                return Index::Invalid();
            }

    };

} // ESPressio::BoundedTopology
