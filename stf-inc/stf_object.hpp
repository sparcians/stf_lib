#ifndef __STF_OBJECT_HPP__
#define __STF_OBJECT_HPP__

namespace stf {
    template<typename TypeIdEnum>
    class STFObject {
        protected:
            const TypeIdEnum id_;

        public:
            using id_type = TypeIdEnum;

            STFObject(const TypeIdEnum id) :
                id_(id)
            {
            }

            inline TypeIdEnum getId() const {
                return id_;
            }
    };

    template<typename Type, typename TypeIdEnum>
    class TypeAwareSTFObject {
        public:
            static TypeIdEnum getTypeId();
    };

} // end namespace stf

#endif
