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

#include "stf_enums.hpp"
#include "stf_record.hpp"
#include "stf_record_map.hpp"
#include "stf_record_types.hpp"
#include "stf_reg_def.hpp"
#include "stf_writer.hpp"
#include "util.hpp"

namespace stf {
    /**
     * \class MemAccess
     * \brief Defines address and data of a memory access
     *
     */
    class MemAccess {
        private:
            const InstMemAccessRecord* access_;
            const InstMemContentRecord* data_;

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
    class STFInst {
        private:
            static constexpr uint32_t NOP_OPCODE_ = 0x00000013; /**< Opcode for RISCV nop */
            static const InstRegRecord x0_src_; /**< X0 register source record */
            static const InstRegRecord x0_dest_; /**< X0 register dest record */
            static const boost::container::flat_map<descriptors::internal::Descriptor, descriptors::internal::Descriptor> PAIRED_RECORDS_;
            static const boost::container::flat_set<descriptors::internal::Descriptor> SKIPPED_PAIRED_RECORDS_;


        protected:
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
            };

            uint64_t branch_target_ = 0; /**< branch target PC */
            uint64_t pc_ = 0; /**< PC of the instruction */

#ifdef STF_INST_HAS_IEM
            INST_IEM iem_ = INST_IEM::STF_INST_IEM_INVALID; /**< instruction encoding mode */
            bool iem_changed_ = false; /**< If true, IEM changed with this instruction */
#endif
            uint32_t opcode_ = 0; /**< opcode */
            uint64_t index_ = 0; /**< instruction index */

            uint32_t tgid_ = 0; /**< Thread group ID */
            uint32_t tid_ = 0; /**< Thread ID */
            uint32_t asid_ = 0; /**< ASID */

#ifdef STF_INST_HAS_PAGE_CROSS
            uint64_t i_page_cross_ = page_utils::INVALID_PHYS_ADDR; /**< inst page crossing physical addresses */
            uint64_t d_page_cross_ = page_utils::INVALID_PHYS_ADDR; /**< inst page crossing physical addresses */
#endif

            enums::int_t<INSTFLAGS> inst_flags_ = 0; /**< inst flags to indicate nullified, branch, etc; */
            bool skipped_ = false; /**< If true, this instruction should be skipped over by the reader */
            uint8_t opcode_size_ = 0; /**< Size of the opcode in bytes */

            /**
             * \typedef EventVector
             * Vector that holds Event objects
             */
            using EventVector = boost::container::small_vector<Event, 4>;
            EventVector events_; /**< events_ saves (possibly multiple) events */

            static constexpr size_t OPERAND_VEC_SIZE = 2; /**< Initial size of an OperandVector */
            /**
             * \typedef OperandVector
             * Vector that holds Operand objects
             */
            using OperandVector = boost::container::small_vector<Operand,
                                                                 OPERAND_VEC_SIZE>;

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
                            using vec_iterator = typename VecType::const_iterator;
                            vec_iterator it_;

                            /**
                             * Advances iterator to the next vector if we've reached the end of the first vector
                             */
                            inline void checkIterator_() {
                                if(STF_EXPECT_FALSE(it_ == parent_->vec1_->end())) {
                                    it_ = parent_->vec2_->begin();
                                }
                            }

                            /**
                             * Constructs an iterator
                             * \param parent Parent CombinedView object
                             * \param it Initial iterator
                             */
                            iterator(const CombinedView* parent, const vec_iterator& it) :
                                parent_(parent),
                                it_(it)
                            {
                            }

                        public:
                            /**
                             * Constructs a begin iterator to the specified CombinedView
                             * \param parent Parent CombinedView object
                             */
                            static inline iterator constructBegin(const CombinedView* parent) {
                                iterator it(parent, parent->vec1_->begin());
                                it.checkIterator_();
                                return it;
                            }

                            /**
                             * Constructs an end iterator to the specified CombinedView
                             * \param parent Parent CombinedView object
                             */
                            static inline iterator constructEnd(const CombinedView* parent) {
                                return iterator(parent, parent->vec2_->end());
                            }

                            /**
                             * Advances the iterator (prefix)
                             */
                            inline iterator& operator++() {
                                ++it_;
                                checkIterator_();
                                return *this;
                            }

                            /**
                             * Advances the iterator (postfix)
                             */
                            inline iterator operator++(int) {
                                auto temp = *this;
                                ++(*this);
                                return temp;
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
                            inline bool operator==(const iterator& rhs) {
                                return it_ == rhs.it_;
                            }

                            /**
                             * Inequality operator
                             */
                            inline bool operator!=(const iterator& rhs) {
                                return it_ != rhs.it_;
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

            static constexpr size_t REG_STATE_IDX = 0; /**< Index of register state records in register_records_ array */
            static constexpr size_t REG_SOURCE_IDX = 1; /**< Index of register source records in register_records_ array */
            static constexpr size_t REG_DEST_IDX = 2; /**< Index of register dest records in register_records_ array */
            static constexpr size_t REG_RECORD_ARRAY_SIZE = REG_DEST_IDX + 1; /**< Size of register_records_ array */

            static_assert(enums::to_int(Registers::STF_REG_OPERAND_TYPE::REG_STATE) - 1 == REG_STATE_IDX &&
                          enums::to_int(Registers::STF_REG_OPERAND_TYPE::REG_SOURCE) - 1 == REG_SOURCE_IDX &&
                          enums::to_int(Registers::STF_REG_OPERAND_TYPE::REG_DEST) - 1 == REG_DEST_IDX,
                          "STF_REG_OPERAND_TYPE values are not compatible with how InstRegRecords are stored in STFInst");
            std::array<OperandVector, REG_RECORD_ARRAY_SIZE> register_records_; /**< register_records_ saves (possibly multiple) operands */

            /**
             * \typedef CombinedOperandView
             * Combined view of operand vectors
             */
            using CombinedOperandView = CombinedView<OperandVector>;
            CombinedOperandView operands_{register_records_[REG_SOURCE_IDX], register_records_[REG_DEST_IDX]}; /**< combined view of register_records_ */

            /**
             * \typedef MemAccessVector
             * Vector that holds MemAccess objects
             */
            using MemAccessVector = boost::container::small_vector<MemAccess, 1>;

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

            /**
             * \typedef CombinedMemAccessView
             * Combined view of memory access vectors
             */
            using CombinedMemAccessView = CombinedView<MemAccessVector>;
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
                branch_target_ = 0;
                pc_ = 0;
#ifdef STF_INST_HAS_IEM
                iem_ = 0;
                iem_changed_ = 0;
#endif
                opcode_ = 0;
                index_ = 0;
                tgid_ = 0;
                tid_ = 0;
                asid_ = 0;
#ifdef STF_INST_HAS_PAGE_CROSS
                i_page_cross_ = 0;
                d_page_cross_ = 0;
#endif
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
             * \struct ExtraRecordComparer
             * Comparator type used by ExtraRecordQueue
             */
            struct ExtraRecordComparer {
                /**
                 * \brief Returns whether lhs should come before rhs
                 * \param lhs First STFRecord to compare
                 * \param rhs Second STFRecord to compare
                 */
                bool operator()(const STFRecord* lhs, const STFRecord* rhs) {
                    return descriptors::conversion::reverseEncodedCompare(lhs->getDescriptor(), rhs->getDescriptor());
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
             * \brief Only STFInstReader can modify the content of STFInst
             */
            template<typename FilterType>
            friend class STFInstReaderBase;
            friend class STFInstWriter;

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
                                                                                 extra_recs.top()->getDescriptor()))) {
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
            bool isKernelCode() const {
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
             * \brief Instruction index (starting from 1)
             * \return Instruction index
             */
            uint64_t index() const { return index_; }

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
    };

} // end namespace stf

#endif
