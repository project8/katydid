/*
 * KTMemberVariable.hh
 *
 *  Created on: Aug 5, 2014
 *      Author: nsoblath
 */

#ifndef KTMEMBERVARIABLE_HH_
#define KTMEMBERVARIABLE_HH_


/**
 * Creates a member variable with type TYPE name f[NAME], plus getters and setters.
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
#define MEMBERVARIABLE(TYPE, NAME) \
        private: \
            TYPE f##NAME; \
        public: \
            inline TYPE Get##NAME() const {return f##NAME;} \
            inline void Set##NAME(TYPE var) {f##NAME = var; return;}

/**
 * Creates a member variable with type TYPE name f[NAME], plus getters and setters.
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
#define MEMBERVARIABLEREF(TYPE, NAME) \
        private: \
            TYPE f##NAME; \
        public: \
            inline const TYPE& Get##NAME() const {return f##NAME;} \
            inline void Set##NAME(const TYPE& var) {f##NAME = var; return;}


#endif /* KTMEMBERVARIABLE_HH_ */
