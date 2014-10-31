/*
 * KTMemberVariable.hh
 *
 *  Created on: Aug 5, 2014
 *      Author: nsoblath
 */

#ifndef KTMEMBERVARIABLE_HH_
#define KTMEMBERVARIABLE_HH_


/**
 * Creates a member variable with type TYPE name f[NAME], plus getter and setter.
 * MEMBERVARIABLE_NOSET will provide the variable and getter, but no setter, allowing you to provide a custom setter.
 *
 * Usage example, in a class header file:
 *     MEMBERVARIABLE(double, MyVar)
 *
 * You still need to initialize the variables in the class constructors.
 *
 * The generated code is:
 *     private:
 *         TYPE f[NAME];
 *     public:
 *         inline TYPE Get[NAME]() const
 *         {
 *             return f[NAME];
 *         }
 *         inline void Set[NAME](TYPE var)
 *         {
 *             f[NAME] = var;
 *         }
 */
#define MEMBERVARIABLE_NOSET(TYPE, NAME) \
        private: \
            TYPE f##NAME; \
        public: \
            inline TYPE Get##NAME() const {return f##NAME;} \

#define MEMBERVARIABLE(TYPE, NAME) \
        MEMBERVARIABLE_NOSET(TYPE, NAME) \
            inline void Set##NAME(TYPE var) {f##NAME = var; return;}

#define MEMBERVARIABLE_PROTECTED_NOSET(TYPE, NAME) \
        protected: \
            TYPE f##NAME; \
        public: \
            inline TYPE Get##NAME() const {return f##NAME;} \

#define MEMBERVARIABLE_PROTECTED(TYPE, NAME) \
        MEMBERVARIABLE_PROTECTED_NOSET(TYPE, NAME) \
            inline void Set##NAME(TYPE var) {f##NAME = var; return;}



/**
 * Creates a member variable with type TYPE name f[NAME], plus getters and setters.
 * MEMBERVARIABLEREF_NOSET will provide the variable and getter, but no setter, allowing you to provide a custom setter.
 *
 * Usage example, in a class header file:
 *     MEMBERVARIABLEREF(std::string, MyVar)
 *
 * You still need to initialize the variables in the class constructors.
 *
 * The generated code is:
 *     private:
 *         TYPE f[NAME];
 *     public:
 *         inline const TYPE& Get[NAME]() const
 *         {
 *             return f[NAME];
 *         }
 *         inline void Set[NAME](const TYPE& var)
 *         {
 *             f[NAME] = var;
 *         }
 */
#define MEMBERVARIABLEREF_NOSET(TYPE, NAME) \
        private: \
            TYPE f##NAME; \
        public: \
            inline const TYPE& Get##NAME() const {return f##NAME;}

#define MEMBERVARIABLEREF(TYPE, NAME) \
        MEMBERVARIABLEREF_NOSET(TYPE, NAME) \
            inline void Set##NAME(const TYPE& var) {f##NAME = var; return;}

#define MEMBERVARIABLEREF_PROTECTED_NOSET(TYPE, NAME) \
        protected: \
            TYPE f##NAME; \
        public: \
            inline const TYPE& Get##NAME() const {return f##NAME;}

#define MEMBERVARIABLEREF_PROTECTED(TYPE, NAME) \
        MEMBERVARIABLEREF_PROTECTED_NOSET(TYPE, NAME) \
            inline void Set##NAME(const TYPE& var) {f##NAME = var; return;}


#endif /* KTMEMBERVARIABLE_HH_ */
