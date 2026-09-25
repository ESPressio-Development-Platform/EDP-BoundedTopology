#pragma once

#include <cstddef>
#include <cstdint>

#include "BoundedIndex.hpp"

namespace ESPressio::BoundedTopology {

    /// Describes the outcome of changing membership in a bounded index set.
    enum class BoundedIndexSetMutationResult : std::uint8_t {

        /// The requested membership state was established.
        Succeeded = 0,

        /// The supplied bounded index is invalid for this set.
        InvalidIndex = 1

    };


    /// Compact one-bit-per-index membership state for a positive compile-time bounded index space.
    ///
    /// @tparam TIndexSpace Compile-time semantic tag shared with the represented bounded indices.
    /// @tparam TCapacity Number of representable indices. This primary template represents positive capacity.
    template<class TIndexSpace, std::size_t TCapacity>
    class BoundedIndexSet final {

        static_assert(
            TCapacity > 0U,
            "The positive-capacity BoundedIndexSet specialization requires capacity greater than zero."
        );

        private:

            // Internal Type and storage metadata.

            /// Strong index Type represented internally by this set.
            using IndexType = BoundedIndex<TIndexSpace, TCapacity>;

            /// Exact retained byte count required for one bit per represented index.
            static constexpr std::size_t StorageByteCount = (TCapacity + 7U) / 8U;


            // Compact membership state.

            /// One bit per bounded index. A set bit means that index belongs to this set.
            std::uint8_t _bytes[StorageByteCount] = {};


            // Bit addressing.

            /// Returns the backing-byte ordinal for one known-valid index.
            static constexpr std::size_t ByteIndex(
                IndexType index
            ) noexcept {
                return static_cast<std::size_t>(
                    index.Value()
                ) / 8U;
            }

            /// Returns the one-bit mask for one known-valid index.
            static constexpr std::uint8_t BitMask(
                IndexType index
            ) noexcept {
                return static_cast<std::uint8_t>(
                    1U << (
                        static_cast<std::size_t>(
                            index.Value()
                        ) % 8U
                    )
                );
            }

            /// Returns the mask containing only valid index bits for one backing byte.
            static constexpr std::uint8_t ValidMask(
                std::size_t byteIndex
            ) noexcept {
                const auto firstIndex = byteIndex * 8U;
                const auto remaining = TCapacity - firstIndex;

                if (remaining >= 8U) {
                    return 0xFFU;
                }

                return static_cast<std::uint8_t>(
                    (
                        static_cast<std::uint16_t>(1U) << remaining
                    ) - 1U
                );
            }

        public:

            // Public Type metadata.

            /// Strong index Type represented by this set.
            using Index = IndexType;

            /// Exact retained bytes required for one bit per represented index.
            static constexpr std::size_t StorageBytes = StorageByteCount;


            // Construction.

            /// Creates an empty bounded index set.
            constexpr BoundedIndexSet() noexcept = default;


            // Membership mutation.

            /// Adds one valid bounded index to the set.
            constexpr BoundedIndexSetMutationResult Set(
                Index index
            ) noexcept {
                if (!index.IsValid()) {
                    return BoundedIndexSetMutationResult::InvalidIndex;
                }

                const auto byteIndex = ByteIndex(
                    index
                );

                _bytes[byteIndex] = static_cast<std::uint8_t>(
                    _bytes[byteIndex] |
                    BitMask(
                        index
                    )
                );

                return BoundedIndexSetMutationResult::Succeeded;
            }

            /// Removes one valid bounded index from the set.
            constexpr BoundedIndexSetMutationResult Clear(
                Index index
            ) noexcept {
                if (!index.IsValid()) {
                    return BoundedIndexSetMutationResult::InvalidIndex;
                }

                const auto byteIndex = ByteIndex(
                    index
                );

                _bytes[byteIndex] = static_cast<std::uint8_t>(
                    _bytes[byteIndex] &
                    static_cast<std::uint8_t>(
                        ~BitMask(
                            index
                        )
                    )
                );

                return BoundedIndexSetMutationResult::Succeeded;
            }

            /// Removes every represented index from the set.
            constexpr void ClearAll() noexcept {
                for (std::size_t byteIndex = 0U; byteIndex < StorageBytes; ++byteIndex) {
                    _bytes[byteIndex] = 0U;
                }
            }

            /// Adds every represented index to the set while leaving non-represented tail bits clear.
            constexpr void SetAll() noexcept {
                for (std::size_t byteIndex = 0U; byteIndex < StorageBytes; ++byteIndex) {
                    _bytes[byteIndex] = ValidMask(
                        byteIndex
                    );
                }
            }


            // Membership inspection.

            /// Indicates whether one valid bounded index currently belongs to the set.
            constexpr bool IsSet(
                Index index
            ) const noexcept {
                if (!index.IsValid()) {
                    return false;
                }

                return (
                    _bytes[
                        ByteIndex(
                            index
                        )
                    ] &
                    BitMask(
                        index
                    )
                ) != 0U;
            }

            /// Indicates whether the set currently contains no represented index.
            constexpr bool IsEmpty() const noexcept {
                for (std::size_t byteIndex = 0U; byteIndex < StorageBytes; ++byteIndex)
                    if (_bytes[byteIndex] != 0U) {
                        return false;
                    }

                return true;
            }

            /// Indicates whether at least one represented index currently belongs to the set.
            constexpr bool IsAnySet() const noexcept {
                return !IsEmpty();
            }

            /// Calculates the current number of represented members without retaining a cached count.
            constexpr std::size_t Count() const noexcept {
                std::size_t count = 0U;

                for (std::size_t indexValue = 0U; indexValue < TCapacity; ++indexValue)
                    if (
                        IsSet(
                            Index::FromUnchecked(
                                indexValue
                            )
                        )
                    ) {
                        ++count;
                    }

                return count;
            }

            /// Finds the lowest represented index currently belonging to the set, or Invalid when empty.
            constexpr Index FindFirstSet() const noexcept {
                for (std::size_t indexValue = 0U; indexValue < TCapacity; ++indexValue) {
                    const auto index = Index::FromUnchecked(
                        indexValue
                    );

                    if (IsSet(
                        index
                    )) {
                        return index;
                    }
                }

                return Index::Invalid();
            }

            /// Finds the lowest represented index currently absent from the set, or Invalid when full.
            constexpr Index FindFirstClear() const noexcept {
                for (std::size_t indexValue = 0U; indexValue < TCapacity; ++indexValue) {
                    const auto index = Index::FromUnchecked(
                        indexValue
                    );

                    if (!IsSet(
                        index
                    )) {
                        return index;
                    }
                }

                return Index::Invalid();
            }

    };


    /// Zero-capacity bounded index set carrying no membership state.
    ///
    /// @tparam TIndexSpace Compile-time semantic tag retained only for Type identity.
    template<class TIndexSpace>
    class BoundedIndexSet<TIndexSpace, 0U> final {

        public:

            // Public Type metadata.

            /// Zero-capacity strong index Type represented by this empty set.
            using Index = BoundedIndex<TIndexSpace, 0U>;

            /// Zero-capacity sets require no retained backing bytes.
            static constexpr std::size_t StorageBytes = 0U;


            // Construction.

            /// Creates the stateless zero-capacity set.
            constexpr BoundedIndexSet() noexcept = default;


            // Membership mutation.

            /// Rejects membership because a zero-capacity set has no valid index.
            constexpr BoundedIndexSetMutationResult Set(
                Index
            ) noexcept {
                return BoundedIndexSetMutationResult::InvalidIndex;
            }

            /// Rejects membership removal because a zero-capacity set has no valid index.
            constexpr BoundedIndexSetMutationResult Clear(
                Index
            ) noexcept {
                return BoundedIndexSetMutationResult::InvalidIndex;
            }

            /// Performs no work because no membership state exists.
            constexpr void ClearAll() noexcept {
            }

            /// Performs no work because no membership state exists.
            constexpr void SetAll() noexcept {
            }


            // Membership inspection.

            /// Reports false because no index can belong to a zero-capacity set.
            constexpr bool IsSet(
                Index
            ) const noexcept {
                return false;
            }

            /// Reports true because a zero-capacity set contains no members.
            constexpr bool IsEmpty() const noexcept {
                return true;
            }

            /// Reports false because a zero-capacity set cannot contain a member.
            constexpr bool IsAnySet() const noexcept {
                return false;
            }

            /// Returns zero without retaining or scanning state.
            constexpr std::size_t Count() const noexcept {
                return 0U;
            }

            /// Returns Invalid because no set member can exist.
            constexpr Index FindFirstSet() const noexcept {
                return Index::Invalid();
            }

            /// Returns Invalid because no valid clear index can exist.
            constexpr Index FindFirstClear() const noexcept {
                return Index::Invalid();
            }

    };

} // ESPressio::BoundedTopology
