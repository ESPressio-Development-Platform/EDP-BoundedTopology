#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace ESPressio::BoundedTopology {

    /// Describes the outcome of checked bounded-index construction.
    enum class BoundedIndexCreationResult : std::uint8_t {

        /// A valid bounded index was created.
        Succeeded = 0,

        /// The supplied numeric value lies outside the bounded index space.
        OutOfRange = 1

    };


    /// Strong compact identity for one position in a compile-time finite index space.
    ///
    /// @tparam TIndexSpace Compile-time semantic tag distinguishing this index space from unrelated spaces.
    /// @tparam TCapacity Number of valid indices represented by this index space.
    template<class TIndexSpace, std::size_t TCapacity>
    class BoundedIndex final {

        private:

            // Compact representation.

            /// Smallest unsigned scalar capable of representing every valid index plus one invalid sentinel.
            using StorageType = std::conditional_t<
                (TCapacity <= static_cast<std::size_t>(std::numeric_limits<std::uint8_t>::max())),
                std::uint8_t,
                std::conditional_t<
                    (TCapacity <= static_cast<std::size_t>(std::numeric_limits<std::uint16_t>::max())),
                    std::uint16_t,
                    std::conditional_t<
                        (TCapacity <= static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max())),
                        std::uint32_t,
                        std::uint64_t
                    >
                >
            >;

            /// Raw scalar representation. The maximum value of StorageType is reserved as Invalid.
            StorageType _value = std::numeric_limits<StorageType>::max();

            // Internal construction.

            /// Constructs directly from a representation already known to obey the caller's intended contract.
            constexpr explicit BoundedIndex(
                StorageType value
            ) noexcept :
                _value(value) {
            }

        public:

            // Public Type metadata.

            /// Runtime storage Type selected from the compile-time capacity.
            using Storage = StorageType;

            /// Semantic tag distinguishing this index space.
            using IndexSpace = TIndexSpace;

            /// Invalid sentinel representation reserved outside the valid 0..Capacity-1 range.
            static constexpr Storage InvalidValue = std::numeric_limits<Storage>::max();


            // Construction.

            /// Creates the invalid bounded-index value.
            constexpr BoundedIndex() noexcept = default;

            /// Returns the invalid bounded-index value.
            static constexpr BoundedIndex Invalid() noexcept {
                return BoundedIndex(
                    InvalidValue
                );
            }

            /// Constructs one index without performing a range check.
            ///
            /// The caller must guarantee value < Capacity(). This exists for bounded internal loops and
            /// compile-time-proven paths where repeating a range check would add no semantic value.
            static constexpr BoundedIndex FromUnchecked(
                std::size_t value
            ) noexcept {
                return BoundedIndex(
                    static_cast<Storage>(
                        value
                    )
                );
            }

            /// Attempts to construct one bounded index from an arbitrary numeric value.
            ///
            /// The output is unchanged when value is outside the valid index space.
            static constexpr BoundedIndexCreationResult TryCreate(
                std::size_t value,
                BoundedIndex& output
            ) noexcept {
                if (value >= TCapacity) {
                    return BoundedIndexCreationResult::OutOfRange;
                }

                output = FromUnchecked(
                    value
                );

                return BoundedIndexCreationResult::Succeeded;
            }


            // Capacity and representation.

            /// Returns the compile-time number of valid indices in this index space.
            static constexpr std::size_t Capacity() noexcept {
                return TCapacity;
            }

            /// Indicates whether this value identifies one valid member of the bounded index space.
            constexpr bool IsValid() const noexcept {
                return static_cast<std::size_t>(
                    _value
                ) < TCapacity;
            }

            /// Returns the exact scalar representation, including InvalidValue for the canonical invalid index.
            constexpr Storage Value() const noexcept {
                return _value;
            }


            // Comparison.

            /// Compares two indices from the same semantic index space and capacity for exact equality.
            constexpr bool operator ==(
                const BoundedIndex& other
            ) const noexcept = default;

            /// Compares two indices from the same semantic index space and capacity for inequality.
            constexpr bool operator !=(
                const BoundedIndex& other
            ) const noexcept = default;

    };

} // ESPressio::BoundedTopology
