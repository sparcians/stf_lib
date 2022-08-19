#ifndef __STF_INST_HPP__
#define __STF_INST_HPP__

#include <cstdint>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <type_traits>
#include <vector>

#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include "boost_small_vector.hpp"

#include "stf_branch_decoder.hpp"
#include "stf_enums.hpp"
#include "stf_item.hpp"
#include "stf_record.hpp"
#include "stf_record_map.hpp"
#include "stf_record_types.hpp"
#include "stf_reg_def.hpp"
#include "stf_writer.hpp"
#include "util.hpp"

namespace stf {
    template<typename FilterType>
    class STFInstReaderBase;

    namespace delegates {
        class STFInstDelegate;
    } // end namespace delegates

    /**
     * \class MemAccess
     * \brief Defines address and data of a memory access
     *
     */
    class MemAccess {
        private:
            const InstMemAccessRecord* access_ = nullptr;
            const InstMemContentRecord* data_ = nullptr;

        public:
            MemAccess() = default;

            /**
             * Constructs a MemAccess
             * \param record Pointer to underlying InstMemAccessRecord
             */
            explicit MemAccess(const STFRecord* const record) :
                access_(static_cast<const InstMemAccessRecord*>(record))
            {
            }

            /**
             * Constructs a MemAccess
             * \param access_record Pointer to underlying InstMemAccessRecord
             * \param content_record Pointer to underlying InstMemContentRecord
             */
            MemAccess(const STFRecord* const access_record, const STFRecord* const content_record) :
                access_(static_cast<const InstMemAccessRecord*>(access_record)),
                data_(static_cast<const InstMemContentRecord*>(content_record))
            {
            }

            /**
             * Sets the content for the memory access
             * \param record Pointer to underlying InstMemContentRecord
             */
            void setContent(const STFRecord* const record) {
                data_ = static_cast<const InstMemContentRecord*>(record);
            }

            /**
             * Resets the access
             */
            void reset() {
                access_ = nullptr;
                data_ = nullptr;
            }

            /**
             * Gets size of access
             */
            uint64_t getSize() const { return access_->getSize(); }

            /**
             * Gets address of access
             */
            uint64_t getAddress() const { return access_->getAddress(); }

            /**
             * Gets data of access
             */
            uint64_t getData() const { return getContentRecord().getData(); }

            /**
             * Gets type of access
             */
            INST_MEM_ACCESS getType() const { return access_->getType(); }

            /**
             * Gets access attributes
             */
            uint16_t getAttr() const { return access_->getAttr(); }

            /**
             * Gets underlying access record
             */
            const InstMemAccessRecord& getAccessRecord() const { return *access_; }

            /**
             * Gets underlying content record
             */
            const InstMemContentRecord& getContentRecord() const {
                stf_assert(data_, "MemAccess has invalid content record");
                return *data_;
            }

            /**
             * Formats a MemAccess into an ostream
             */
            friend std::ostream& operator<<(std::ostream& os, const MemAccess& access);
    };

    /**
     * \class Operand
     * \brief Defines operand register number and content
     *
     */
    class Operand {
        private:
            static constexpr std::string_view STATE_LABEL_ = "state ";
            static constexpr std::string_view DEST_LABEL_ = "dst ";
            static constexpr std::string_view SRC_LABEL_ = "src ";

            const InstRegRecord* rec_;

        public:
            /**
             * Constructs an Operand
             * \param record Pointer to underlying InstRegRecord
             */
            explicit Operand(const STFRecord::UniqueHandle& record) :
                Operand(static_cast<const InstRegRecord*>(record.get()))
            {
            }

            /**
             * Constructs an Operand
             * \param record Pointer to underlying InstRegRecord
             */
            explicit Operand(const InstRegRecord* record) :
                rec_(record)
            {
            }

            /**
             * Gets the underlying record
             */
            const InstRegRecord& getRecord() const { return *rec_; }

            /**
             * Gets the corresponding label for the record
             */
            std::string_view getLabel() const;

            /**
             * Formats an Operand into an ostream
             */
            friend std::ostream& operator<<(std::ostream& os, const Operand& reg);

            /**
             * Gets the operand value
             */
            uint64_t getScalarValue() const {
                return rec_->getScalarData();
            }

            /**
             * Gets the operand register number
             */
            Registers::STF_REG getReg() const {
                return rec_->getReg();
            }

            /**
             * Gets the operand type
             */
            Registers::STF_REG_OPERAND_TYPE getType() const {
                return rec_->getOperandType();
            }

            /**
             * Gets whether this is a vector operand
             */
            bool isVector() const {
                return rec_->isVector();
            }

            /**
             * Gets the vector operand value
             */
            const InstRegRecord::VectorType& getVectorValue() const {
                return rec_->getVectorData();
            }

            /**
             * Gets the vlen parameter that was set when the record was read
             */
            vlen_t getVLen() const {
                return rec_->getVLen();
            }
    };

    /**
     * \class Event
     * \brief Defines event number and content
     *
     */
    class Event {
        private:
            const EventRecord* event_; /**< event */
            const EventPCTargetRecord* target_ = nullptr; /**< event target */

        public:
            Event() = default;

            /**
             * Constructs an Event
             * \param event_record Pointer to underlying EventRecord
             */
            explicit Event(const STFRecord* const event_record) :
                event_(&event_record->as<EventRecord>())
            {
            }

            /**
             * Sets target record
             * \param event_target_record Target record corresponding to this event
             */
            void setTarget(const STFRecord* const event_target_record) {
                target_ = &event_target_record->as<EventPCTargetRecord>();
            }

            /**
             * Returns event
             */
            auto getEvent() const {
                return event_->getEvent();
            }

            /**
             * Returns data
             */
            const auto& getData() const {
                return event_->getData();
            }

            /**
             * Returns target
             */
            auto getTarget() const {
                return target_->getAddr();
            }

            /**
             * Returns whether data is valid
             */
            bool dataValid() const {
                return event_;
            }

            /**
             * Returns whether target is valid
             */
            bool targetValid() const {
                return target_;
            }

            /**
             * Resets the event
             */
            void reset() {
                event_ = nullptr;
                target_ = nullptr;
            }

            /**
             * Gets whether the event is a fault
             */
            bool isFault() const {
                return event_->isFault();
            }

            /**
             * Gets whether the event is an interrupt
             */
            bool isInterrupt() const {
                return event_->isInterrupt();
            }

            /**
             * Gets whether the event is a mode change
             */
            bool isModeChange() const {
                return event_->isModeChange();
            }

            /**
             * Gets whether the event is a syscall
             */
            bool isSyscall() const {
                return event_->isSyscall();
            }
    };

    /**
     * Formats an Event into an ostream
     */
    std::ostream& operator<<(std::ostream& os, const Event& event);

    /**
     * \class STFInst
     * \brief Keeps all relevent info to an instruction information.
     * It is only modifiable by STFInstReader.
     *
     * \todo implement is_branch, is_load, and is_store
     */
    class STFInst : public STFSkippableItem {
        public:
            /**
             * \enum INSTFLAGS
             * Defines instruction attribute flags
             */
            enum INSTFLAGS : uint16_t {
                INST_INIT_FLAGS         = 0,
                INST_VALID              = 1,        /**< this is valid instr, contain valid opcode */
                INST_TAKEN_BRANCH       = 1 << 1,   /**< is taken branch or not */
                INST_OPCODE16           = 1 << 2,   /**< is 16-bit opcode or not */
                INST_COF                = 1 << 3,   /**< Change of flow. contains an Inst_PC record */
                INST_IS_BRANCH          = 1 << 4,   /**< is a branch instruciton */
                INST_IS_LOAD            = 1 << 5,   /**< inst has load operation */
                INST_IS_STORE           = 1 << 6,   /**< inst has store operation */
                INST_IS_SYSCALL         = 1 << 7,   /**< inst is a syscall */
                INST_IS_FP              = 1 << 8,   /**< inst is FP */
                INST_CHANGE_TO_USER     = 1 << 9,   /**< inst changes to user mode */
                INST_CHANGE_FROM_USER   = 1 << 10,  /**< inst changes from user mode */
                INST_IS_FAULT           = 1 << 11,  /**< instruction is a fault */
                INST_IS_VECTOR          = 1 << 12,  /**< instruction is vector */
                INST_IS_INTERRUPT       = 1 << 13,  /**< instruction is an interrupt */
            };

        private:
            friend class delegates::STFInstDelegate;

            static constexpr uint32_t NOP_OPCODE_ = 0x00000013; /**< Opcode for RISCV nop */
            static const InstRegRecord x0_src_; /**< X0 register source record */
            static const InstRegRecord x0_dest_; /**< X0 register dest record */
            static const boost::container::flat_map<descriptors::internal::Descriptor, descriptors::internal::Descriptor> PAIRED_RECORDS_;
            static const boost::container::flat_set<descriptors::internal::Descriptor> SKIPPED_PAIRED_RECORDS_;

        protected:
            uint64_t branch_target_ = 0; /**< branch target PC */
            uint64_t pc_ = 0; /**< PC of the instruction */

#ifdef STF_INST_HAS_IEM
            INST_IEM iem_ = INST_IEM::STF_INST_IEM_INVALID; /**< instruction encoding mode */
            bool iem_changed_ = false; /**< If true, IEM changed with this instruction */
#endif
            uint32_t opcode_ = 0; /**< opcode */

            uint32_t tgid_ = 0; /**< Thread group ID */
            uint32_t tid_ = 0; /**< Thread ID */
            uint32_t asid_ = 0; /**< ASID */

#ifdef STF_INST_HAS_PAGE_CROSS
            uint64_t i_page_cross_ = page_utils::INVALID_PHYS_ADDR; /**< inst page crossing physical addresses */
            uint64_t d_page_cross_ = page_utils::INVALID_PHYS_ADDR; /**< inst page crossing physical addresses */
#endif

            enums::int_t<INSTFLAGS> inst_flags_ = 0; /**< inst flags to indicate nullified, branch, etc; */
            bool has_vstart_ = false; /**< If true, this instruction accesses the VSTART CSR */
            bool has_vl_ = false; /**< If true, this instruction accesses the VL CSR */
            uint8_t opcode_size_ = 0; /**< Size of the opcode in bytes */

            static constexpr size_t OPERAND_VEC_SIZE = 2; /**< Initial size of an OperandVector */

            static constexpr size_t REG_STATE_IDX = 0; /**< Index of register state records in register_records_ array */
            static constexpr size_t REG_SOURCE_IDX = 1; /**< Index of register source records in register_records_ array */
            static constexpr size_t REG_DEST_IDX = 2; /**< Index of register dest records in register_records_ array */
            static constexpr size_t REG_RECORD_ARRAY_SIZE = REG_DEST_IDX + 1; /**< Size of register_records_ array */

            static_assert(enums::to_int(Registers::STF_REG_OPERAND_TYPE::REG_STATE) - 1 == REG_STATE_IDX &&
                          enums::to_int(Registers::STF_REG_OPERAND_TYPE::REG_SOURCE) - 1 == REG_SOURCE_IDX &&
                          enums::to_int(Registers::STF_REG_OPERAND_TYPE::REG_DEST) - 1 == REG_DEST_IDX,
                          "STF_REG_OPERAND_TYPE values are not compatible with how InstRegRecords are stored in STFInst");

        public:
            /**
             * \typedef EventVector
             * Vector that holds Event objects
             */
            using EventVector = boost::container::small_vector<Event, 4>;
            EventVector events_; /**< events_ saves (possibly multiple) events */

            /**
             * \typedef OperandVector
             * Vector that holds Operand objects
             */
            using OperandVector = boost::container::small_vector<Operand,
                                                                 OPERAND_VEC_SIZE>;
            /**
             * \typedef MemAccessVector
             * Vector that holds MemAccess objects
             */
            using MemAccessVector = boost::container::small_vector<MemAccess, 1>;

        protected:
            /**
             * \class CombinedView
             * Combines two vectors into a single iterable view
             */
            template<typename VecType>
            class CombinedView {
                private:
                    const VecType* vec1_;
                    const VecType* vec2_;

                public:
                    /**
                     * \class iterator
                     * Iterates over the vectors in a CombinedView
                     */
                    class iterator {
                        private:
                            const CombinedView* parent_;
                            const VecType* cur_vec_;
                            using vec_iterator = typename VecType::const_iterator;
                            vec_iterator it_;

                            inline void skipToVec_(const VecType* next_vec, const vec_iterator& next_it) {
                                cur_vec_ = next_vec;
                                it_ = next_it;
                            }

                            inline void skipToVec1_() {
                                skipToVec_(parent_->vec1_, std::prev(parent_->vec1_->end()));
                            }

                            inline void skipToVec2_() {
                                skipToVec_(parent_->vec2_, parent_->vec2_->begin());
                            }

                            /**
                             * Advances iterator to the next vector if we've reached the end of the first vector
                             */
                            inline void checkIteratorFwd_() {
                                if(STF_EXPECT_FALSE(cur_vec_ == parent_->vec1_ && it_ == parent_->vec1_->end())) {
                                    skipToVec2_();
                                }
                            }

                            /**
                             * Advances iterator to the previous vector if we've reached the beginning of the second vector while decrementing
                             */
                            inline bool checkIteratorRev_() {
                                if(STF_EXPECT_FALSE(cur_vec_ == parent_->vec2_ && it_ == parent_->vec2_->begin())) {
                                    skipToVec1_();
                                    return true;
                                }
                                return false;
                            }

                            inline void checkParents_(const iterator& rhs) const {
                                stf_assert((parent_ == rhs.parent_) ||
                                           ((parent_->vec1_ == rhs.parent_->vec1_) && (parent_->vec2_ == rhs.parent_->vec2_)),
                                           "Attempted to compare iterators from different vectors");
                            }

                            /**
                             * Constructs an iterator
                             * \param parent Parent CombinedView object
                             * \param it Initial iterator
                             */
                            iterator(const CombinedView* parent, const VecType* parent_vec, const vec_iterator& it) :
                                parent_(parent),
                                cur_vec_(parent_vec),
                                it_(it)
                            {
                            }

                        public:
                            /**
                             * \typedef iterator_category
                             * Uses the iterator_category of the underlying vector iterator
                             */
                            using iterator_category = typename vec_iterator::iterator_category;
                            /**
                             * \typedef difference_type
                             * Uses the difference_type of the underlying vector iterator
                             */
                            using difference_type = typename vec_iterator::difference_type;
                            /**
                             * \typedef value_type
                             * Uses the value_type of the underlying vector iterator
                             */
                            using value_type = typename vec_iterator::value_type;
                            /**
                             * \typedef pointer
                             * Uses the pointer type of the underlying vector iterator
                             */
                            using pointer = typename vec_iterator::pointer;
                            /**
                             * \typedef reference
                             * Uses the reference type of the underlying vector iterator
                             */
                            using reference = typename vec_iterator::reference;

                        private:
                            difference_type pos_() const {
                                difference_type pos = it_ - cur_vec_->begin();
                                if(cur_vec_ != parent_->vec1_) {
                                    pos += parent_->vec1_->size();
                                }
                                return pos;
                            }

                        public:
                            /**
                             * Constructs a begin iterator to the specified CombinedView
                             * \param parent Parent CombinedView object
                             */
                            static inline iterator constructBegin(const CombinedView* parent) {
                                iterator it(parent, parent->vec1_, parent->vec1_->begin());
                                it.checkIteratorFwd_();
                                return it;
                            }

                            /**
                             * Constructs an end iterator to the specified CombinedView
                             * \param parent Parent CombinedView object
                             */
                            static inline iterator constructEnd(const CombinedView* parent) {
                                return iterator(parent, parent->vec2_, parent->vec2_->end());
                            }

                            /**
                             * Increment operator (prefix)
                             */
                            inline iterator& operator++() {
                                ++it_;
                                checkIteratorFwd_();
                                return *this;
                            }

                            /**
                             * Increment operator (postfix)
                             */
                            inline iterator operator++(int) { // cppcheck-suppress functionConst
                                auto temp = *this;
                                ++(*this);
                                return temp;
                            }

                            /**
                             * Decrement operator (prefix)
                             */
                            inline iterator& operator--() {
                                it_ -= !checkIteratorRev_();
                                return *this;
                            }

                            /**
                             * Decrement operator (postfix)
                             */
                            inline iterator operator--(int) { // cppcheck-suppress functionConst
                                auto temp = *this;
                                --(*this);
                                return temp;
                            }

                            /**
                             * Increment assignment operator
                             * \param n Amount to increment by
                             */
                            inline iterator& operator+=(const difference_type n) {
                                const auto pos = it_ - cur_vec_->begin();
                                const auto cur_vec_size = cur_vec_->size();
                                if(n > 0) {
                                    if(pos + n > cur_vec_size) {
                                        stf_assert(cur_vec_ == parent_->vec1_,
                                                   "Tried to seek past the end of the vector");
                                        skipToVec2_();
                                        it_ += n - (cur_vec_size - pos);
                                    }
                                    else {
                                        it_ += n;
                                        checkIteratorFwd_();
                                    }
                                }
                                else if(n < 0) {
                                    if(pos + n < 0) {
                                        stf_assert(cur_vec_ == parent_->vec2_,
                                                   "Tried to seek before the beginning of the vector");
                                        skipToVec1_();
                                        it_ += n + pos + 1;
                                    }
                                    else {
                                        it_ += n + checkIteratorRev_();
                                    }
                                }
                                return *this;
                            }

                            /**
                             * Decrement assignment operator
                             * \param n Amount to decrement by
                             */
                            inline iterator& operator-=(const difference_type n) {
                                return *this += -n;
                            }

                            /**
                             * Addition operator
                             * \param n Amount to add to iterator
                             */
                            inline iterator operator+(const difference_type n) {
                                auto temp = *this;
                                temp += n;
                                return temp;
                            }

                            /**
                             * Addition operator
                             * \param n Amount to add to iterator
                             * \param it Iterator
                             */
                            friend inline iterator operator+(const difference_type n, const iterator& it) {
                                return it + n;
                            }

                            /**
                             * Subtraction operator
                             * \param n Amount to subtract
                             */
                            inline iterator operator-(const difference_type n) {
                                auto temp = *this;
                                temp -= n;
                                return temp;
                            }

                            /**
                             * Difference operator
                             * \param b Iterator
                             * \param a Iterator
                             */
                            friend inline difference_type operator-(const iterator& b, const iterator& a) {
                                b.checkParents_(a);
                                return b.pos_() - a.pos_();
                            }

                            /**
                             * Dereferences the iterator
                             * \param n Offset to dereference from
                             */
                            inline reference operator[](const difference_type n) {
                                return *(*this + n);
                            }

                            /**
                             * Dereferences the iterator
                             */
                            inline const typename VecType::value_type& operator*() const {
                                return *it_;
                            }

                            /**
                             * Dereferences the iterator
                             */
                            inline const typename VecType::value_type* operator->() const {
                                return it_.operator->();
                            }

                            /**
                             * Equality operator
                             */
                            inline bool operator==(const iterator& rhs) const {
                                return cur_vec_ == rhs.cur_vec_ && it_ == rhs.it_;
                            }

                            /**
                             * Inequality operator
                             */
                            inline bool operator!=(const iterator& rhs) const {
                                return cur_vec_ != rhs.cur_vec_ || it_ != rhs.it_;
                            }

                            /**
                             * Less-than operator
                             */
                            inline bool operator<(const iterator& rhs) const {
                                checkParents_(rhs);
                                return pos_() < rhs.pos_();
                            }

                            /**
                             * Less-than or equal-to operator
                             */
                            inline bool operator<=(const iterator& rhs) const {
                                checkParents_(rhs);
                                return pos_() <= rhs.pos_();
                            }

                            /**
                             * Greater-than operator
                             */
                            inline bool operator>(const iterator& rhs) const {
                                checkParents_(rhs);
                                return pos_() > rhs.pos_();
                            }

                            /**
                             * Greater-than or equal-to operator
                             */
                            inline bool operator>=(const iterator& rhs) const {
                                checkParents_(rhs);
                                return pos_() >= rhs.pos_();
                            }
                    };

                    friend class iterator;

                    /**
                     * Constructs a CombinedView object, allowing sequential iteration over v1 and v2.
                     * \param v1 First vector
                     * \param v2 Second vector
                     */
                    CombinedView(const VecType& v1, const VecType& v2) :
                        vec1_(&v1),
                        vec2_(&v2)
                    {
                    }

                    /**
                     * Gets an iterator to the beginning. If v1 is empty, the iterator will point to the beginning of v2.
                     */
                    inline iterator begin() const {
                        return iterator::constructBegin(this);
                    }

                    /**
                     * Gets an iterator to the end. Always points to the end of v2.
                     */
                    inline iterator end() const {
                        return iterator::constructEnd(this);
                    }

                    /**
                     * Gets the combined size of both vectors
                     */
                    inline size_t size() const {
                        return vec1_->size() + vec2_->size();
                    }

                    /**
                     * Gets whether both vectors are empty
                     */
                    inline bool empty() const {
                        return vec1_->empty() && vec2_->empty();
                    }

                    /**
                     * Gets the first element in the combined view
                     */
                    const auto& front() const {
                        return *begin();
                    }
            };

            /**
             * \typedef CombinedOperandView
             * Combined view of operand vectors
             */
            using CombinedOperandView = CombinedView<OperandVector>;

            /**
             * \typedef CombinedMemAccessView
             * Combined view of memory access vectors
             */
            using CombinedMemAccessView = CombinedView<MemAccessVector>;

            std::array<OperandVector, REG_RECORD_ARRAY_SIZE> register_records_; /**< register_records_ saves (possibly multiple) operands */
            CombinedOperandView operands_{register_records_[REG_SOURCE_IDX], register_records_[REG_DEST_IDX]}; /**< combined view of register_records_ */

            static constexpr size_t MEM_READ_IDX = 0; /**< Index of memory read records in mem_access_records_ array */
            static constexpr size_t MEM_WRITE_IDX = 1; /**< Index of memory write records in mem_access_records_ array */
            static constexpr size_t MEM_ACCESS_ARRAY_SIZE = MEM_WRITE_IDX + 1; /**< Size of mem_access_records_ array */

            static_assert(enums::to_int(INST_MEM_ACCESS::READ) - 1 == MEM_READ_IDX &&
                          enums::to_int(INST_MEM_ACCESS::WRITE) - 1 == MEM_WRITE_IDX,
                          "STF_REG_OPERAND_TYPE values are not compatible with how InstRegRecords are stored in STFInst");

            std::array<MemAccessVector, MEM_ACCESS_ARRAY_SIZE> mem_access_records_; /**< mem_access_records_ saves (possibly multiple)
                                                                                     * memory accesses for the instruction
                                                                                     *
                                                                                     * Note that some instructions have multiple memory
                                                                                     * accesses.
                                                                                     * So ALWAYS iterate through the memory accesses.
                                                                                     */

            CombinedMemAccessView mem_accesses_{mem_access_records_[MEM_READ_IDX], mem_access_records_[MEM_WRITE_IDX]}; /**< combined view of mem_accesses_ */

            RecordMap orig_records_; /**< orig_records_ contains all the STFRecords for
                                      * this instruction, in the order they were read
                                      * from the trace
                                      */

            /**
             * Writes a record type, paired with a second record type
             */
            inline void writeRecordPairs_(STFWriter& stf_writer,
                                          const descriptors::internal::Descriptor first_desc,
                                          const RecordMap::SmallVector& first_record_vec,
                                          const descriptors::internal::Descriptor second_desc) const {
                const auto& second_vec = orig_records_.at(second_desc);
                const bool is_event = first_desc == descriptors::internal::Descriptor::STF_EVENT;
                stf_assert(is_event || (second_vec.size() == first_record_vec.size()),
                           "There must be a 1-1 correspondence between " << first_desc << " and " << second_desc << " records");
                auto second_it = second_vec.begin();
                for(const auto& record: first_record_vec) {
                    stf_writer << *record;
                    // Mode change events don't have PC targets
                    if(STF_EXPECT_TRUE(!(is_event && record->as<EventRecord>().isModeChange()))) {
                        stf_writer << **second_it;
                        ++second_it;
                    }
                }
            }

            /**
             * \brief Add original record to the instruction
             */
            auto appendOrigRecord_(STFRecord::UniqueHandle&& urec) {
                return orig_records_.emplace(std::move(urec));
            }

            /**
             * Turns this instruction into a nop
             */
            void setNop_() {
                // Override the opcode
                opcode_ = NOP_OPCODE_;

                // Remove any events
                events_.clear();

                // Clear operands (leave register state records alone)
                register_records_[REG_SOURCE_IDX].clear();
                register_records_[REG_DEST_IDX].clear();

                // Add r0 as a src operand
                register_records_[REG_SOURCE_IDX].emplace_back(&x0_src_);

                // Clear flags
                inst_flags_ = INST_INIT_FLAGS;
            }

            /**
             * Gets the vector of memory access records of the given type
             * \param type Type of memory access to get
             */
            MemAccessVector& getMemAccessVector_(const INST_MEM_ACCESS type) {
                const auto idx = static_cast<size_t>(type) - 1;
                return mem_access_records_[idx];
            }

            /**
             * Gets the vector of operand records of the given type
             * \param type Type of operand to get
             */
            const MemAccessVector& getMemAccessVector_(const INST_MEM_ACCESS type) const {
                const auto idx = static_cast<size_t>(type) - 1;
                return mem_access_records_[idx];
            }

            /**
             * Gets the vector of operand records of the given type
             * \param type Type of operand to get
             */
            OperandVector& getOperandVector_(const Registers::STF_REG_OPERAND_TYPE type) {
                const auto idx = static_cast<size_t>(type) - 1;
                return register_records_[idx];
            }

            /**
             * \brief Gets the vector of operand records of the given type
             * \param type Type of operand to get
             * \return Vector of Operand objects
             */
            const OperandVector& getOperandVector_(const Registers::STF_REG_OPERAND_TYPE type) const {
                const auto idx = static_cast<size_t>(type) - 1;
                return register_records_[idx];
            }

            /**
             * Resets an STFInst so that it can be reinitialized by an STFInstReader
             *
             * Somewhat clunky solution, but this avoids some reallocations and speeds up reading
             */
            void reset_() {
                STFItem::reset_();
                branch_target_ = 0;
                pc_ = 0;
#ifdef STF_INST_HAS_IEM
                iem_ = 0;
                iem_changed_ = 0;
#endif
                opcode_ = 0;
                tgid_ = 0;
                tid_ = 0;
                asid_ = 0;
#ifdef STF_INST_HAS_PAGE_CROSS
                i_page_cross_ = 0;
                d_page_cross_ = 0;
#endif
                has_vl_ = false;
                has_vstart_ = false;
                inst_flags_ = 0;
                events_.clear();
                auto& reg_state_records = register_records_[REG_STATE_IDX];
                reg_state_records.clear();

                // If reg_state_records has grown past its initial capacity, that indicates that
                // we had a reg state dump - these are rare and relatively large, so clean up the
                // unused memory
                if(STF_EXPECT_FALSE(reg_state_records.capacity() > OPERAND_VEC_SIZE)) {
                    reg_state_records.shrink_to_fit();
                }
                register_records_[REG_SOURCE_IDX].clear();
                register_records_[REG_DEST_IDX].clear();
                for(auto& m: mem_access_records_) {
                    m.clear();
                }
                orig_records_.clear();
            }

            /**
             * Sets one or more instruction flags
             * \param flag Flag(s) to set
             */
            inline void setInstFlag_(const INSTFLAGS flag) {
                inst_flags_ = static_cast<decltype(inst_flags_)>(inst_flags_ | flag);
            }

            /**
             * Checks whether this is a vector instruction. Used by STFInstReader while building the STFInst
             * \param not_state If true, this is not a register state record
             * \param rec InstRegRecord used to check for vector register accesses
             */
            inline bool checkIfVector_(const bool not_state, const InstRegRecord& rec) {
                // Some vector LS instructions only access VSTART and VL when VL == 0
                has_vstart_ |= not_state && (rec.getReg() == Registers::STF_REG::STF_REG_CSR_VSTART);
                has_vl_ |= not_state && (rec.getReg() == Registers::STF_REG::STF_REG_CSR_VL);
                // Otherwise, any vector instruction should have at least 1 vector operand
                return not_state && (rec.isVector() || (has_vstart_ && has_vl_));
            }

            /**
             * \struct ExtraRecordComparer
             * Comparator type used by ExtraRecordQueue
             */
            struct ExtraRecordComparer {
                /**
                 * \brief Returns whether lhs should come before rhs
                 * \param lhs First STFRecord to compare
                 * \param rhs Second STFRecord to compare
                 */
                bool operator()(const STFRecord* lhs, const STFRecord* rhs) const {
                    return descriptors::conversion::reverseEncodedCompare(lhs->getId(), rhs->getId());
                }
            };

        public:
            /**
             * Default constructor
             */
            STFInst() = default;

            /**
             * Move constructor
             */
            STFInst(STFInst&&) = default;

            /**
             * Copy constructor
             */
            STFInst(const STFInst& rhs) = default;

            /**
             * Copy assignment operator
             */
            STFInst& operator=(const STFInst&) = default;

            /**
             * Move assignment operator
             */
            STFInst& operator=(STFInst&&) = default;

            /**
             * \brief Write all records in this instruction to STFWriter
             */
            inline void write(STFWriter& stf_writer) const {
                for (const auto& vec_pair: orig_records_.sorted()) {
                    if(const auto pair_it = PAIRED_RECORDS_.find(vec_pair.first); STF_EXPECT_FALSE(pair_it != PAIRED_RECORDS_.end())) {
                        writeRecordPairs_(stf_writer,
                                          vec_pair.first,
                                          vec_pair.second,
                                          pair_it->second);
                    }
                    else if(STF_EXPECT_FALSE(SKIPPED_PAIRED_RECORDS_.count(vec_pair.first) != 0)) {
                        continue;
                    }
                    else {
                        for(const auto& record: vec_pair.second) {
                            stf_writer << *record;
                        }
                    }
                }
            }

            /**
             * \brief Write all records in this instruction to STFWriter, filtering out specified record types
             */
            inline void write(STFWriter& stf_writer,
                              const std::set<stf::descriptors::internal::Descriptor>& filtered_recs) const {
                for (const auto& vec_pair: orig_records_.sorted()) {
                    if(filtered_recs.count(vec_pair.first)) {
                        continue;
                    }

                    if(const auto pair_it = PAIRED_RECORDS_.find(vec_pair.first); STF_EXPECT_FALSE(pair_it != PAIRED_RECORDS_.end())) {
                        writeRecordPairs_(stf_writer,
                                          vec_pair.first,
                                          vec_pair.second,
                                          pair_it->second);
                    }
                    else if(STF_EXPECT_FALSE(SKIPPED_PAIRED_RECORDS_.count(vec_pair.first) != 0)) {
                        continue;
                    }
                    else {
                        for(const auto& record: vec_pair.second) {
                            stf_writer << *record;
                        }
                    }
                }
            }

            /**
             * \typedef ExtraRecordQueue
             * Queue that holds additional records, sorted by their encoded descriptor
             */
            using ExtraRecordQueue = std::priority_queue<const STFRecord*,
                                                         std::vector<const STFRecord*>,
                                                         ExtraRecordComparer>;
            /**
             * \brief Write all records in this instruction to STFWriter, including new records
             * \param stf_writer STFWriter to use
             * \param extra_recs Extra records to write
             */
            template<typename ... ExtraRecords>
            inline void writeWithExtraRecords(STFWriter& stf_writer, ExtraRecords&&... extra_recs) const {
                writeWithExtraRecords(stf_writer,
                                      ExtraRecordQueue(ExtraRecordComparer(),
                                                       {&extra_recs...}));
            }

            /**
             * \brief Write all records in this instruction to STFWriter, including new records
             * \note The queue is consumed as part of this process
             * \param stf_writer STFWriter to use
             * \param extra_recs ExtraRecordQueue of additional records to write
             */
            inline void writeWithExtraRecords(STFWriter& stf_writer, ExtraRecordQueue&& extra_recs) const {
                stf_assert(!extra_recs.empty(), "Extra records argument cannot be empty!");

                for (const auto& vec_pair: orig_records_.sorted()) {
                    if(STF_EXPECT_FALSE(!extra_recs.empty() &&
                                        !descriptors::conversion::encodedCompare(vec_pair.first,
                                                                                 extra_recs.top()->getId()))) {
                            stf_writer << *extra_recs.top();
                    }

                    if(const auto pair_it = PAIRED_RECORDS_.find(vec_pair.first);
                       STF_EXPECT_FALSE(pair_it != PAIRED_RECORDS_.end())) {
                        writeRecordPairs_(stf_writer,
                                          vec_pair.first,
                                          vec_pair.second,
                                          pair_it->second);
                    }
                    else if(STF_EXPECT_FALSE(SKIPPED_PAIRED_RECORDS_.count(vec_pair.first) != 0)) {
                        continue;
                    }
                    else {
                        for(const auto& record: vec_pair.second) {
                            stf_writer << *record;
                        }
                    }
                }
            }

            /**
             * \brief Returns whether the instruction is kernel code
             */
            bool isKernelCode() const { // cppcheck-suppress functionStatic
                //FIXME: Needs to actually do something
                return false;
            }

            /**
             * \brief Taken branch or not
             * \return True if taken branch
             */
            bool isTakenBranch() const { return inst_flags_ & INST_TAKEN_BRANCH; }

            /**
             * \brief 16-bit opcode or not
             * \return True if 16-bit opcode
             */
            bool isOpcode16() const { return inst_flags_ & INST_OPCODE16; }

            /**
             * \brief Change of flow or not
             * \return True if change of flow
             */
            bool isCoF() const { return inst_flags_ & INST_COF; }

            /**
             * \brief Load or not
             * \return True if load
             */
            bool isLoad() const { return inst_flags_ & INST_IS_LOAD; }

            /**
             * \brief Store or not
             * \return True if store
             */
            bool isStore() const { return inst_flags_ & INST_IS_STORE; }

            /**
             * \brief Syscall or not
             * \return True if syscall
             */
            bool isSyscall() const { return inst_flags_ & INST_IS_SYSCALL; }

            /**
             * \brief Fault or not
             * \return True if fault
             */
            bool isFault() const { return inst_flags_ & INST_IS_FAULT; }

            /**
             * \brief Fault or not
             * \return True if fault
             */
            bool isInterrupt() const { return inst_flags_ & INST_IS_INTERRUPT; }

            /**
             * \brief FP or not
             * \return True if instruction is floating point
             */
            bool isFP() const { return inst_flags_ & INST_IS_FP; }

            /**
             * \brief Vector or not
             * \return True if instruction is vector
             */
            bool isVector() const { return inst_flags_ & INST_IS_VECTOR; }

            /**
             * \brief Instruction is change from user mode
             * \return True if instruction changes from user mode to another mode
             */
            bool isChangeFromUserMode() const { return inst_flags_ & INST_CHANGE_FROM_USER; }

            /**
             * \brief Instruction is change to user mode
             * \return True if instruction changes to user mode from another mode
             */
            bool isChangeToUserMode() const { return inst_flags_ & INST_CHANGE_TO_USER; }

            /**
             * \brief Branch target virtual PC
             * \return The branch target virtual PC
             */
            uint64_t branchTarget() const { return branch_target_; }

            /**
             * Get the map of all records related to this instruction
             */
            const auto& getOrigRecords() const { return orig_records_; }

            /**
             * Gets the register state records
             */
            const OperandVector& getRegisterStates() const {
                return register_records_[REG_STATE_IDX];
            }

            /**
             * Gets the vector of CommentRecords
             */
            const auto& getComments() const { return orig_records_.at(descriptors::internal::Descriptor::STF_COMMENT); }

            /**
             * Gets the vector of InstMicroOpRecords
             */
            const auto& getMicroOps() const { return orig_records_.at(descriptors::internal::Descriptor::STF_INST_MICROOP); }

            /**
             * Gets the vector of InstReadyRegRecords
             */
            const auto& getReadyRegs() const { return orig_records_.at(descriptors::internal::Descriptor::STF_INST_READY_REG); }

            /**
             * \brief Get the vector of events associated with this instruction
             * \return Vector of Event objects
             */
            const auto& getEvents() const {
                return events_;
            }

            /**
             * \brief Gets the register source and destination records
             * \return CombinedView of Operand objects
             */
            const auto& getOperands() const {
                return operands_;
            }

            /**
             * \brief Gets the vector of source operands
             * \return Vector of Operand objects
             */
            const auto& getSourceOperands() const {
                return getOperandVector_(stf::Registers::STF_REG_OPERAND_TYPE::REG_SOURCE);
            }

            /**
             * \brief Gets the vector of destination operands
             * \return Vector of Operand objects
             */
            const auto& getDestOperands() const {
                return getOperandVector_(stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST);
            }

            /**
             * \brief Gets all memory read and write records
             * \return CombinedView of MemAccess objects
             */
            const auto& getMemoryAccesses() const {
                return mem_accesses_;
            }

            /**
             * \brief Gets all memory read records
             * \return Vector of MemAccess objects
             */
            const auto& getMemoryReads() const {
                return getMemAccessVector_(INST_MEM_ACCESS::READ);
            }

            /**
             * \brief Gets all memory write records
             * \return CombinedView of MemAccess objects
             */
            const auto& getMemoryWrites() const {
                return getMemAccessVector_(INST_MEM_ACCESS::WRITE);
            }

            /**
             * \brief Return the sum of memory access sizes this instruction makes
             * \return The total memory access size
             */
            uint64_t totalMemAccessSize() const {
                return std::accumulate(mem_accesses_.begin(),
                                       mem_accesses_.end(),
                                       0ULL,
                                       [](uint64_t sum, const MemAccess& a) { return sum + a.getSize(); });
            }

            /**
             * Gets the vector of PageTableWalkRecords
             */
            const auto& getEmbeddedPTEs() const { return orig_records_.at(descriptors::internal::Descriptor::STF_PAGE_TABLE_WALK); }

#ifdef STF_INST_HAS_PAGE_CROSS
            /**
             * \brief has instruction page cross
             * \return has instruction page cross
             */
            bool hasInstPageCross() const { return page_utils::isValidPhysAddr(i_page_cross_); }

            /**
             * \brief instruction page cross physical address
             * \return instruction page cross physical address
             */
            uint64_t instPageCrossPhysAddr() const { return i_page_cross_; }

            /**
             * \brief has data page cross
             * \return has data page cross
             */
            bool hasDataPageCross() const { return page_utils::isValidPhysAddr(d_page_cross_); }

            /**
             * \brief data page cross physical address
             * \return data page cross physical address
             */
            uint64_t dataPageCrossPhysAddr() const { return d_page_cross_; }
#endif

            /**
             * \brief Process ID
             * \return Process ID
             */
            uint32_t asid() const { return asid_; }

            /**
             * \brief Thread ID
             * \return Thread ID
             */
            uint32_t tid() const { return tid_; }

            /**
             * \brief Thread Group ID (tgid)
             * \return Thread Group ID (tgid)
             */
            uint32_t tgid() const { return tgid_; }

            /**
             * \brief Instruction virtual PC
             * \return Instruction virtual PC
             */
            uint64_t pc() const { return pc_; }

#ifdef STF_INST_HAS_IEM
            /**
             * \brief Instruction Encoding Mode
             * \return Instruction Encoding Mode
             */
            INST_IEM iem() const { return iem_; }

            /**
             * whether IEM has changed
             */
            bool iem_changed() const { return iem_changed_; }
#endif
            /**
             * \brief Opcode
             * \return Opcode
             */
            uint32_t opcode() const { return opcode_; }

            /**
             * \brief Opcode
             * \return Opcode
             */
            uint8_t opcodeSize() const { return opcode_size_; }

            /**
             * \brief whether the information in this instance is valid
             * \return True if valid
             */
            bool valid() const { return inst_flags_ & INST_VALID; };

            /**
             * Get the string representation of the opcode width
             */
            inline const char* getOpcodeWidthStr() const {
                return isOpcode16() ? "INST16" : "INST32";
            }

            /**
             * Gets whether the instruction is a branch
             */
            bool isBranch() const {
                return inst_flags_ & INST_IS_BRANCH;
            }
    };

    namespace delegates {
        /**
         * \class STFInstDelegate
         * Delegate class used to hide any non-const methods from non-reader classes
         */
        class STFInstDelegate : public STFSkippableItemDelegate {
            private:
                /**
                 * Appends a memory access
                 * \param inst STFInst to modify
                 * \param type Type of memory access
                 * \param access_record Memory access record
                 * \param content_record Memory content record
                 */
                __attribute__((always_inline))
                static inline void appendMemAccess_(STFInst& inst,
                                                    const INST_MEM_ACCESS type,
                                                    const STFRecord* const access_record,
                                                    const STFRecord* const content_record) {
                    inst.getMemAccessVector_(type).emplace_back(access_record, content_record);
                }

                /**
                 * Sets an instruction flag
                 * \param inst STFInst to modify
                 * \param flag Flag to set
                 */
                __attribute__((always_inline))
                static inline void setFlag_(STFInst& inst, const STFInst::INSTFLAGS flag) {
                    inst.setInstFlag_(flag);
                }

                /**
                 * Appends an event to the EventVector
                 * \param inst STFInst to modify
                 * \param rec Event record to append
                 */
                __attribute__((always_inline))
                static inline void appendEvent_(STFInst& inst, const STFRecord* const rec) {
                    inst.events_.emplace_back(rec);
                }

                /**
                 * Sets the last event's target info
                 * \param inst STFInst to modify
                 * \param rec Event target record to add
                 */
                __attribute__((always_inline))
                static inline void setLastEventTarget_(STFInst& inst, const STFRecord* const rec) {
                    return inst.events_.back().setTarget(rec);
                }

                /**
                 * Marks the instruction as a taken branch
                 * \param inst STFInst to modify
                 * \param target Branch target address
                 */
                __attribute__((always_inline))
                static inline void setTakenBranch_(STFInst& inst, const uint64_t target) {
                    inst.setInstFlag_(STFInst::INST_TAKEN_BRANCH);
                    inst.branch_target_ = target;
                }

                /**
                 * Resets the instruction to its initial state
                 * \param inst STFInst to reset
                 */
                __attribute__((always_inline))
                static inline void reset_(STFInst& inst) {
                    inst.reset_();
                }

                /**
                 * Appends an operand to the instruction
                 * \param inst STFInst to modify
                 * \param type Operand type
                 * \param rec Operand record to append
                 */
                __attribute__((always_inline))
                static inline void appendOperand_(STFInst& inst,
                                                  const Registers::STF_REG_OPERAND_TYPE type,
                                                  const InstRegRecord& rec) {
                    const bool not_state = type != Registers::STF_REG_OPERAND_TYPE::REG_STATE;
                    inst.getOperandVector_(type).emplace_back(&rec);

                    // Set FP flag if we have an FP source or dest register
                    // Set vector flag if we have a vector source or dest register
                    inst.setInstFlag_(math_utils::conditionalValue(
                        not_state && rec.isFP(), STFInst::INST_IS_FP,
                        inst.checkIfVector_(not_state, rec), STFInst::INST_IS_VECTOR
                    ));
                }

                /**
                 * Appends a record to the instruction's record map
                 * \param inst STFInst to modify
                 * \param urec Record to append
                 */
                __attribute__((always_inline))
                static inline const STFRecord* appendOrigRecord_(STFInst& inst, STFRecord::UniqueHandle&& urec) {
                    return inst.appendOrigRecord_(std::move(urec));
                }

                /**
                 * Appends a record to the instruction's record map
                 * \param inst STFInst to modify
                 * \param urec Record to append
                 */
                template<typename InstRecordType>
                __attribute__((always_inline))
                static inline void setInstInfo_(STFInst& inst,
                                                const InstRecordType& rec,
                                                const INST_IEM iem,
#ifdef STF_INST_HAS_IEM
                                                const bool iem_changed,
#endif
                                                const uint32_t asid,
                                                const uint32_t tid,
                                                const uint32_t tgid,
                                                const bool is_skipped) {
                    static constexpr bool is_compressed = std::is_same_v<InstRecordType, InstOpcode16Record>;

                    inst.opcode_ = rec.getOpcode();
                    inst.pc_ = rec.getPC();
                    inst.opcode_size_ = rec.getOpcodeSize();
#ifdef STF_INST_HAS_IEM
                    inst.iem_ = iem;
                    inst.iem_changed_ = iem_changed;
#endif
                    inst.setSkipped_(is_skipped);
                    inst.setInstFlag_(math_utils::conditionalValue(STFBranchDecoder::isBranch(iem, rec), STFInst::INST_IS_BRANCH,
                                                                   is_compressed, STFInst::INST_OPCODE16));
                    inst.asid_ = asid;
                    inst.tid_ = tid;
                    inst.tgid_ = tgid;
                }

                /**
                 * Turns this instruction into a nop
                 * \param inst Instruction to turn into a nop
                 */
                __attribute__((always_inline))
                static inline void setNop_(STFInst& inst) {
                    inst.setNop_();
                }

                /**
                 * \brief Only STFInstReader can modify the content of STFInst
                 */
                template<typename FilterType>
                friend class stf::STFInstReaderBase;
        };
    } // end namespace delegates

} // end namespace stf

#endif
