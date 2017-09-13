//
//  Events.hpp
//
//  SGEL - Simple Game Engine Library for SFML
//

#pragma once

#include <cassert>
#include <functional>
#include <map>

namespace sgel
{
    /**
     *  Implements a Signals-Slots pattern using lambda functions
     */
    template <typename... Args>
    class Signal
    {
    public:

        typedef std::function<void(Args...)>    SlotType;
        typedef int                             SlotRef;

    public:

        Signal();
        Signal(const Signal& original);
        Signal(Signal&& original);

        SlotRef                         Connect(const SlotType& slot);
        template <typename T> SlotRef   Connect(T *object, void (T::*func)(Args...));
        template <typename T> SlotRef   Connect(T *object, void (T::*func)(Args...) const);

        void                            Disconnect(SlotRef reference);
        void                            DisconnectAll();

        void                            Emit(Args... args);

        Signal& operator=(const Signal& rhs);
        Signal& operator=(Signal&& rhs);

    private:

        std::map<SlotRef, SlotType> slots;
        SlotRef                     nextSlotRef;
    };

    /**
     *  Default constructor
     *
     *  Initialise the nextSlotRef to 0 so references are determinate
     */
    template <typename... Args>
    Signal<Args...>::Signal() :
    nextSlotRef(0)
    {
    }

    /**
     *  Copy constructor
     */
    template <typename... Args>
    Signal<Args...>::Signal(const Signal& original) :
    slots(original.slots),
    nextSlotRef(original.nextSlotRef)
    {
    }

    /**
     *  Move constructor
     */
    template <typename... Args>
    Signal<Args...>::Signal(Signal&& original) :
    slots(std::move(original.slots)),
    nextSlotRef(original.nextSlotRef)
    {
        original.DisconnectAll();
        original.nextSlotRef = 0;
    }

    /**
     *  Connect a slot that invokes the specified function
     *
     *  @param  slot  A funcion to be invoked by the signal
     *
     *  @return Reference to the connected slot can be used to disconnect from the signal
     */
    template <typename... Args>
    typename Signal<Args...>::SlotRef Signal<Args...>::Connect(const SlotType& slot)
    {
        SlotRef newRef = nextSlotRef;
        nextSlotRef++;

        slots[newRef] = slot;

        return newRef;
    }

    /**
     *  Connect a slot that will invoke a method on the specified object
     *
     *  A convenience function for binding and object's method to a slot. Called something like this:-
     *
     *    slot.Connect(this, &ClassName::MethodName);
     *
     *  @param  object The instance to call the method on
     *  @param  func   Method function pointer
     *
     *  @return Reference to the connected slot can be used to disconnect from the signal
     */
    template <typename... Args>
    template <typename T>
    typename Signal<Args...>::SlotRef Signal<Args...>::Connect(T *object, void (T::*func)(Args...))
    {
        assert(object);

        return Connect([=](Args... args) {
            (object->*func)(args...);
        });
    }

    /**
     *  Connect a slot that will invoke a const method on the specified object
     *
     *  A convenience function for binding and object's const method to a slot. Called something like
     *  this:-
     *
     *    slot.Connect(this, &ClassName::MethodName);
     *
     *  @param  object The instance to call the method on
     *  @param  func   Method function pointer
     *
     *  @return Reference to the connected slot can be used to disconnect from the signal
     */
    template <typename... Args>
    template <typename T>
    typename Signal<Args...>::SlotRef Signal<Args...>::Connect(T *object, void (T::*func)(Args...) const)
    {
        assert(object);

        return Connect([=](Args... args) {
            (object->*func)(args...);
        });
    }

    /**
     *  Remove the specified slot to that it will no longer be invoked by Emit
     *
     *  @param  reference A valid slot reference returned by a Connect call
     */
    template <typename... Args>
    void Signal<Args...>::Disconnect(SlotRef reference)
    {
        slots.erase(reference);
    }

    /**
     *  Remove all slots so future calls to Emit will do nothing
     */
    template <typename... Args>
    void Signal<Args...>::DisconnectAll()
    {
        slots.clear();
    }

    /**
     *  Emit the signal by invoking all the slots with the specified arguments
     *
     *  Only slots registered at the point Emit is called will be invoked. New slots can be safely
     *  added and they will be called on the next emit. Any slots disconnected during the emit before
     *  they are invoked, will not be called.
     */
    template <typename... Args>
    void Signal<Args...>::Emit(Args... args)
    {
        // Iterate over a copy of our slots so slots can mutate during the emit
        auto safeSlots = slots;
        for (auto slot : safeSlots)
        {
            if (slots.find(slot.first) != slots.end())
            {
                slot.second(args...);
            }
        }
    }
    
    /**
     *  Assignment operator
     */
    template <typename... Args>
    Signal<Args...>& Signal<Args...>::operator=(const Signal& rhs)
    {
        slots = rhs.slots;
        nextSlotRef = rhs.nextSlotRef;
        return *this;
    }
    
    /**
     *  Move assignment operator
     */
    template <typename... Args>
    Signal<Args...>& Signal<Args...>::operator=(Signal&& rhs)
    {
        slots = std::move(rhs.slots);
        nextSlotRef = rhs.nextSlotRef;
        
        rhs.DisconnectAll();
        rhs.nextSlotRef = 0;
        
        return *this;
    }
}
