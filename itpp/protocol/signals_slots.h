/*!
 * \file 
 * \brief Definitions of Signals and Slots classes
 * \author Anders Persson
 *
 * $Date$
 * $Revision$
 *
 * -------------------------------------------------------------------------
 *
 * IT++ - C++ library of mathematical, signal processing, speech processing,
 *        and communications classes and functions
 *
 * Copyright (C) 1995-2007  (see AUTHORS file for a list of contributors)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * -------------------------------------------------------------------------
 */

#ifndef SIGNAL_SLOT_H
#define SIGNAL_SLOT_H

#include <itpp/protocol/events.h>
#include <list>
#include <iostream>


namespace itpp {

  //! \addtogroup protocol
  //@{

  class Base_Signal;
  template<class DataType> class Signal;
  template<class DataType> class Base_Slot;
  template<class ObjectType, class DataType> class Slot;


  /*!
    \brief Signals and slots


  A simple example where to objects A and B are communicating through signals and slots. Each object has one signal
  and one slot. The A_signal is used to send a signal to the B_slot and vice versa. When a signal is received by the B_slot
  it is forwarded to the function forward(). The class definition includes the definition of the signals, slots and 
  forward functions including a name and the type of data to transmit.


  \code
  #include "signals_slots.h"
  class A;
  class B;

  class A {
  public:
    A(){
      A_slot.forward(this, &A::member);
      A_signal.set_name("A_signal");
      A_signal.set_debug(true);
      A_slot.set_name("A_slot");
      N = 10;
    }
    Signal<int> A_signal;
    Slot<A, double> A_slot;
  private:
    int N;
    void member(const double x) {
      if(N)
        A_signal.arm(3.4, N--);
    }
  };

  class B {
  public:
    B(){
      B_slot.forward(this, &B::member);
      B_signal.set_name("B_signal");
      B_signal.set_debug();
      B_slot.set_name("B_slot");
    }
    Signal<double> B_signal;
    Slot<B, int> B_slot;
  private:
    void member(const int k){ B_signal.arm(23.2, M_PI); }  
  };

  int main(){
    A a; // class A does not know anything about class B.
    B b; // class B does not know anything about class A.
  
    a.A_signal.connect(&b.B_slot); // Connect a to b.
    b.B_signal.connect(&a.A_slot); // Connect b to a.

    a.A_signal.arm(56.2, 3); // First event in 56.2 seconds carrying data = 3
  
    Event_Queue::start(); // start the event-based simulation
  }

  \endcode

  */
  template<class DataType>
    class Signal {
  public:
    friend class Base_Slot<DataType>;

    //!
    Signal(const std::string signal_name = "Unamed Signal", const bool single_shot = false, const bool enable_debug = false);

    //  Signal(const std::string signal_name = "Unamed Signal", const bool single_shot = false, const bool enable_debug = true);

    //!
    ~Signal();

    //! Connect a slot to the signal
    void connect(Base_Slot<DataType>* slot);

    //! Disconnect the slot from the signal
    void disconnect(Base_Slot<DataType>* slot = NULL);

    //  Base_Event* arm(const Ttype delta_time, DataType signal); // Signal will trigger in 'delta_time' time units carrying data signal.


    //! Issue a signal
    Base_Event* operator()(DataType signal, const Ttype delta_time = 0);

    //! cancel signal
    void cancel();

    //! set name of signal
    void set_name(const std::string &signal_name);

    //! Set debug mode. If true all signals are printed to stdout
    void set_debug(const bool enable_debug = true);

    //!
    void trigger(DataType u);

  protected:
    typedef typename std::list<Base_Slot<DataType>*, std::allocator< Base_Slot<DataType>* > >::iterator Base_Slot_Iterator;
    void _disconnect(Base_Slot<DataType>* slot);
    std::list<Base_Slot<DataType>*, std::allocator<Base_Slot<DataType>* > > connected_slots;
    std::string name;

  private:
    bool armed; 
    bool debug;
    bool single;
    Data_Event<Signal, DataType> *e;
  };


  /*!
    \brief Base Slot class

  */
  template<class DataType> 
    class Base_Slot{
  public:
    friend class Signal<DataType>;

    //!
    Base_Slot(const std::string slot_name = "Unamed Base_Slot");

    //!
    virtual ~Base_Slot();

    //! set slot name
    void set_name(const std::string &slot_name);

    //!
    virtual void operator()(DataType signal) = 0;

  protected:
    //   virtual void exec(DataType signal) = 0;
    typedef typename std::list<Signal<DataType>*, std::allocator< Signal<DataType>* > >::iterator Signal_Iterator;
    std::string name;
    void _connect(Signal<DataType>* signal);
    void _disconnect(Signal<DataType>* signal);
    std::list<Signal<DataType>*, std::allocator<Signal<DataType>* > > connected_signals;
  };

  /*!
    \brief Slot Class

  */
  template<class ObjectType, class DataType>
    class Slot : public Base_Slot<DataType> {
  public:
    //!
    Slot(const std::string _name = "Unamed Slot");

    //!
    void forward(ObjectType *object_pointer, void(ObjectType::*object_function_pointer)(DataType u));
    
    //!
    ~Slot();

    //!
    void operator()(DataType u);

    //void exec(DataType signal);

  private:
    ObjectType *po;
    void(ObjectType::*pm)(DataType signal); 
  };


  /*!

  */
  template<class ObjectType, class DataType>
    class ATimer {
  public:
    //!
    ATimer(const std::string Name = "Unamed ATimer") {    
      time_out_signal = new Signal<DataType>(Name, true);
      time_out_slot = new Slot<ObjectType, DataType>(Name);
      time_out_signal->connect(time_out_slot); 
      set_name(Name);
    }

    //!
    void forward(ObjectType *po, void(ObjectType::*pm)(DataType u)) { time_out_slot->forward(po, pm); }

    //!
    void set(DataType u, const Ttype delta_t) {
      time_out_signal->operator()(u, delta_t);
    }

    //!
    void cancel() { time_out_signal->cancel(); }

    //!
    void set_name(const std::string Name) {
      name = Name;
      time_out_signal->set_name(name);
      time_out_slot->set_name(name);
    }

  protected:
    std::string name;

  private:
    Signal<DataType> *time_out_signal;
    Slot<ObjectType, DataType> *time_out_slot;
  };



  /*! 
    TTimer is a class that can be set in order to be
    remembered at a future instance of time. The
    difference to "generic event" is the easy usage
    that already take care about posting and canceling
    events 
    @ingroup EventHandling
  */
  template <class THandler>
    class TTimer {
  public:
    TTimer(THandler & handler, void (THandler::*handlerFunction) (Ttype time)) :
      signal("timer_signal", true)
    {
      fPending = false;
      fExpirationTime = 0;

      registered_handler = &handler;
      registered_handler_function = handlerFunction;

      slot.forward(this, &TTimer<THandler>::HandleProcessEvent);
      slot.set_name("timer_slot");
      signal.set_debug(false);
      signal.connect(&slot);  
    }

    virtual ~TTimer() {
      if (fPending)
	signal.cancel();   
    }
    
    void  Set(Ttype time, bool relative = true) {
	if (fPending)
	  signal.cancel();   

	fPending = true;
	double current_time = Event_Queue::now();
	double delta_time;
	if (relative) {
	  fExpirationTime = current_time + time;
	  delta_time = time;
	} else {
	  fExpirationTime = time;
	  delta_time = time - current_time;
	}
	signal(fExpirationTime, delta_time);
      }

    void  Reset() {
	if (fPending) {
	  signal.cancel();
	  fPending = false; // TODO: Added this myself. Didn't work otherwise.
	}
      }

    Ttype  ExpirationTime() const
    {
      it_assert(fPending, "TTimer<>::ExpirationTime: timer not set");
      return fExpirationTime;
    }

    bool  IsPending() const { return fPending; }

  protected:
    virtual void HandleProcessEvent (Ttype currentTime) {
	fPending = false;
	(*registered_handler.*registered_handler_function)(currentTime);
      }

    virtual void HandleCancelEvent (Ttype) {
	if (fPending)
	  signal.cancel();   

	fPending = false;
      }

    bool  fPending;    /**< \brief is timer set */
    Ttype  fExpirationTime;

  private:
    THandler *registered_handler;
    void(THandler::*registered_handler_function)(Ttype expiry_time);

    Signal<double> signal;     // Used internally
    Slot<TTimer, double> slot; // Used internally
  };






    // -----------------------------------------------------------------------------------------------

    template<class DataType>
      Signal<DataType>::Signal(const std::string signal_name, const bool single_shot, const bool enable_debug) 
      {
	armed = false;
	e = NULL;
	single = single_shot;
	set_name(signal_name);
	set_debug(enable_debug);
      }

    template<class DataType>
      Signal<DataType>::~Signal()
      {
	Base_Slot_Iterator
	  begin = connected_slots.begin(),
	  end   = connected_slots.end(),
	  i;

	for(i=begin; i!=end; i++)
	  (*i)->_disconnect(this);
  
	connected_slots.clear();

	if(e!=NULL) // Cancel a possibly pending event since we are about to die!
	  e->cancel();  
      }

    template<class DataType>
      void Signal<DataType>::set_name(const std::string &signal_name) 
      {
	name = signal_name;
      }

    template<class DataType>
      void Signal<DataType>::set_debug(const bool enable_debug)
      {
	debug = enable_debug;
      }

    template<class DataType>
      void Signal<DataType>::connect(Base_Slot<DataType>* slot)
      {
	Base_Slot_Iterator
	  begin = connected_slots.begin(),
	  end   = connected_slots.end(),
	  i;  

	bool is_already_connected = false;

	for(i=begin; i!=end; i++)
	  if((*i) == slot)
	    is_already_connected = true;  

	if(!is_already_connected) { // Multiple connections is meaningless.
	  connected_slots.push_back(slot);
	  slot->_connect(this); // Needed if a connected slot is deleted during run time.
	} else {
	  std::cout<<"Signal '"<< name <<"' and Slot '"<< slot->name<<"' are already connected. Multiple connections have no effect!"<< std::endl;    
	}
      }

    template<class DataType>
      void Signal<DataType>::disconnect(Base_Slot<DataType>* slot)
      {
	Base_Slot_Iterator
	  begin = connected_slots.begin(),
	  end   = connected_slots.end(),
	  i;

	for(i=begin; i!=end; i++)
	  if((*i) == slot) {
	    (*i)->_disconnect(this);
	    connected_slots.erase(i);
	    break;
	  }                
      }

    template<class DataType>
      Base_Event* Signal<DataType>::operator()(DataType signal, const Ttype delta_time)
      {
	// Signal will trigger in 'delta_time' time units.
	if(single){ // We are operating in single-shot mode.
	  if(armed){ // Cancel and schedule again with the new 'delta_time'.
	    if(debug)
	      std::cout<<"Warning: Changing time for Signal '"<<name<<"'."<< std::endl;
	    cancel();
	    operator()(signal, delta_time);
	  } else {
	    e = new Data_Event<Signal, DataType>(this, &Signal<DataType>::trigger, signal, delta_time);
	    armed = true;
	    Event_Queue::add(e);    
	  }
	} else { // Continious mode (cancel() has no effect).
	  e = new Data_Event<Signal, DataType>(this, &Signal<DataType>::trigger, signal, delta_time);
	  armed = true;
	  Event_Queue::add(e);       
	}
	return e;
      }

    template<class DataType>
      void Signal<DataType>::cancel()
      {
	if(armed&&single){
	  e->cancel();
	  e = NULL;
	  armed = false;
	}
      }


    template<class DataType>
      void Signal<DataType>::trigger(DataType u)
      {
	armed = false;
	e = NULL;
	Base_Slot_Iterator
	  begin = connected_slots.begin(),
	  end   = connected_slots.end(),
	  i;

	for(i=begin; i!=end; i++) { // Execute all the functions of the connected slots.
	  if(debug)
	    std::cout << "Time = " << Event_Queue::now() << ". Signal '" << name << "' was sent to Slot '" << (*i)->name<< "'." << std::endl;
	  (*i)->operator()(u);      
	}
      }

    template<class DataType>
      void Signal<DataType>::_disconnect(Base_Slot<DataType>* slot)
      {
	Base_Slot_Iterator
	  begin = connected_slots.begin(),
	  end   = connected_slots.end(),
	  i;

	for(i=begin; i!=end; i++)
	  if((*i) == slot) {
	    connected_slots.erase(i);
	    break;
	  }                
      }


    template<class DataType>
      Base_Slot<DataType>::Base_Slot(const std::string slot_name)
      {
	set_name(slot_name);
      }

    template<class DataType>
      void Base_Slot<DataType>::set_name(const std::string &slot_name)
      {
	name = slot_name;
      }

    template<class DataType>
      Base_Slot<DataType>::~Base_Slot()
      { // Notify all signals connect that we are being deleted ...

	Signal_Iterator
	  begin = connected_signals.begin(),
	  end   = connected_signals.end(),
	  i;

	for(i=begin; i!=end; i++)
	  (*i)->_disconnect(this);  
  
	connected_signals.clear();
      }

    template<class DataType>
      void Base_Slot<DataType>::_connect(Signal<DataType>* signal)
      { // A signal is being connected to us.
	connected_signals.push_back(signal);
      }

    template<class DataType>
      void Base_Slot<DataType>::_disconnect(Signal<DataType>* signal)
      { // A signal is being disconnected from us.

	Signal_Iterator
	  begin = connected_signals.begin(),
	  end   = connected_signals.end(),
	  i;

	for(i=begin; i!=end; i++)
	  if((*i) == signal) {
	    connected_signals.erase(i);
	    break;
	  }
      }

    template<class ObjectType, class DataType>
      Slot<ObjectType, DataType>::Slot(const std::string slot_name) : Base_Slot<DataType>(slot_name)
      {
	pm = NULL;    
	po = NULL;      
      }

      template<class ObjectType, class DataType>
	Slot<ObjectType, DataType>::~Slot(){}

      template<class ObjectType, class DataType>
	void Slot<ObjectType, DataType>::forward(ObjectType *object_pointer, void(ObjectType::*object_function_pointer)(DataType u))
	{
	  pm = object_function_pointer;    
	  po = object_pointer;
	}

      // template<class ObjectType, class DataType>
      // void Slot<ObjectType, DataType>::exec(DataType signal){
      //   if(pm&&po) 
      //     (*po.*pm)(signal);
      // } 

      template<class ObjectType, class DataType>
	void Slot<ObjectType, DataType>::operator()(DataType signal)
	{
	  if(pm&&po) 
	    (*po.*pm)(signal);
	}

  //@}

} // namespace itpp

#endif // #ifndef SIGNAL_SLOT_H

