/*! \file container.h */
/* This material was developed by Yang, Acer at the National Chung Cheng University, Department of Computer Science. 
 * Permission is granted to copy this software, to redistribute it on a nonprofit basis, and to use it for any purpose, 
 * subject to the following restrictions and understandings. 
 * 
 * 1. Any copy made of this software must include this copyright notice in full. 
 *
 * 2. All materials developed as a consequence of the use of this software shall duly acknowledge such use, in 
 *    accordance with the usual standards of acknowledging credit in academic research. 
 *
 * 3. The authors have made no warranty or representation that the operation of this software will be error-free or 
 *    suitable for any application, and they are under under no obligation to provide any services, by way of 
 *    maintenance, update, or otherwise. The software is an experimental prototype offered on an as-is basis. 
 *
 * 4. Redistribution for profit requires the express, written permission of the authors. 
 *
 */


#include <stdexcept>
#include "mem_helper.h"

#include "container.h"

template<class T>
circular_iterator<T>::circular_iterator(const circular_buffer<T>& cb, pointer cur)
	: cb_(&cb), cur_(cur), beg_(cb.beg_)
{}

template<class T>
circular_iterator<T>::circular_iterator()
	: cb_(0), cur_(0), beg_(0)
{}

template<class T>
circular_iterator<T> & circular_iterator<T>::operator=(const circular_iterator& cp)
{
	cb_ = cp.cb_;
	beg_ = cp.beg_;
	cur_ = cp.cur_;
	return *this;
}    

template<class T>
circular_iterator<T> & circular_iterator<T>::operator += (distance_type offset)
{
	if(offset <0)
		return this->operator-=(-1*offset);
	if(cur_ <= cb_->end_){
		if(cur_ + offset <= cb_->end_)
			cur_ += offset;
		else{
			cur_ = cb_->end_;
#ifdef _DEBUG_
			throw std::runtime_error("circular_iterator line 44\n");
#endif
		}
	}else if(cb_->bufEnd_ - cur_ < offset){
		cur_ = cb_->bufBeg_ + offset - (cb_->bufEnd_ - cur_ + 1);
		if(cur_ > cb_->end_){
			cur_ = cb_->end_;
#ifdef _DEBUG_
			throw std::runtime_error("circular_iterator line 52\n");
#endif    
		}
	}else{
		cur_ += offset;
	}
	return *this;
}

	template<class T>
circular_iterator<T> & circular_iterator<T>::operator -= (distance_type offset)
{
	if( offset <0)
		return this->operator+=(-1*offset);
	if(cur_ < cb_->beg_){
		if(cur_ - cb_->bufBeg_ < offset){
			cur_ = cb_->bufEnd_ - offset + (cur_ - cb_->bufBeg_ + 1);
			if(cur_ < cb_->beg_){
				cur_ = cb_->beg_;
#ifdef _DEBUG_
				throw std::runtime_error("circular_iterator line 69\n");
#endif
			}
		}else{
			cur_ -= offset;
		}
	}else{
		cur_ -= offset;
		if(cur_ < cb_->beg_){
			cur_ = cb_->beg_;
#ifdef _DEBUG_
			throw std::runtime_error("circular_iterator line 73\n");
#endif
		}
	}
	return *this;
}

template<class T>
bool circular_iterator<T>::isValid()
{
	return (cb_ != 0 && beg_ == cb_->beg_);
}

template<class T>
circular_iterator<T> circular_iterator<T>::operator + (distance_type offset) const
{
	circular_iterator citer(*this);
	citer += offset;
	return citer;
}

template<class T>
circular_iterator<T> circular_iterator<T>::operator - (distance_type offset) const
{
	circular_iterator citer(*this);
	citer -= offset;
	return citer;
}

template<class T>
circular_iterator<T> & circular_iterator<T>::operator ++()
{
	return this->operator+=(1);
}

template<class T>
circular_iterator<T> circular_iterator<T>::operator++(int)
{
	circular_iterator iter(*this);
	++(*this);
	return iter;
}

template<class T>
circular_iterator<T> & circular_iterator<T>::operator--()
{
	return this->operator-=(1);
}

template<class T>
circular_iterator<T> circular_iterator<T>::operator--(int)
{
	circular_iterator iter(*this);
	--(*this);
	return iter;
}

template<class T>
typename circular_iterator<T>::difference_type 
circular_iterator<T>::operator - (const circular_iterator<T> &from) const
{
	if(from.beg_ != cb_->beg_ || beg_ != cb_->beg_)
		throw std::runtime_error("circular_iterator line 134\n");

	distance_type to, fr;
	if(cur_ < cb_->beg_){
		to = cur_ - cb_->bufBeg_ + (cb_->bufEnd_ - cb_->beg_) + 1;
	}else{
		to = cur_ - cb_->beg_;
	}

	if(from.cur_ < cb_->beg_){
		fr = from.cur_ - cb_->bufBeg_ + (cb_->bufEnd_ - cb_->beg_) + 1;
	}else{
		fr = from.cur_ - cb_->beg_;
	}

	return to - fr;
}

template<class T>
bool circular_iterator<T>::operator ==(const circular_iterator<T> &rhs) const
{
	return cur_ == rhs.cur_ && beg_ == rhs.beg_;
}

template<class T>
bool circular_iterator<T>::operator !=(const circular_iterator<T> &rhs) const
{
	return !((*this) == rhs);
}

template<class T>
bool circular_iterator<T>::operator <(const circular_iterator<T> &rhs) const
{
	return (rhs - (*this)) > 0;
}

template<class T>
bool circular_iterator<T>::operator >(const circular_iterator<T> &rhs) const
{
	return (rhs - (*this)) < 0;
}

template<class T>
bool circular_iterator<T>::operator <=(const circular_iterator<T> &rhs) const
{
	return (rhs - (*this)) >= 0;
}

template<class T>
bool circular_iterator<T>::operator >=(const circular_iterator<T> &rhs) const
{
	return (rhs - (*this)) <= 0;
}

template<class T>
typename circular_iterator<T>::reference 
circular_iterator<T>::operator *()
{
	return *cur_;
}


template<class T>
circular_buffer<T>::circular_buffer()
	: bufBeg_(0), bufEnd_(0), beg_(0), cur_(0), end_(0)
{}


template<class T>
circular_buffer<T>::circular_buffer(typename circular_buffer<T>::size_type size)
	: bufBeg_(safeBuffer<value_type>()(size, false)), 
	bufEnd_(bufBeg_+size),
	beg_(bufBeg_), cur_(bufBeg_), end_(bufBeg_)
{}


template<class T>
circular_buffer<T>::~circular_buffer()
{
	safeFreeBuffer(&bufBeg_);
}

template<class T>
void circular_buffer<T>::resize(size_type size)
{
	if((size_type)(bufEnd_ - bufBeg_) < size){
		safeFreeBuffer(&bufBeg_);
		bufBeg_ = safeBuffer<value_type>()(size, false);
	}
	bufEnd_ = bufBeg_ + size;
	clear();
}

template<class T>
typename circular_buffer<T>::size_type 
circular_buffer<T>::capacity() const 
{
	return bufEnd_ - bufBeg_;
}

template<class T>
typename circular_buffer<T>::size_type 
circular_buffer<T>::size() const
{
	return (end() - begin());
}

template<class T>
void circular_buffer<T>::clear()
{
	beg_ = cur_ = end_ = bufBeg_;
}

template<class T>
std::basic_istream<T>& circular_buffer<T>::tryfill(std::basic_istream<T>& is)
{
	if(!bufBeg_)
		throw std::underflow_error("circular_buffer underflow");
	if(cur_ -1 == end_ || end_ - cur_ == bufEnd_ - bufBeg_)
		throw std::overflow_error("circular_buffer overflow");

	beg_ = cur_;
	if(cur_ > end_){
		is.read(end_, cur_ - 1 - end_);
		end_ += is.gcount();
	}else{ 
		is.read(end_, bufEnd_ - end_);
		if(!is || cur_ == bufBeg_){
			end_ += is.gcount();
		}else{
			int gc = is.gcount();
			*(end_ + gc) = (value_type)is.get();
			if(!is){
				end_ += gc;
				return is;
			}
			is.read(bufBeg_, cur_-1 - bufBeg_);
			end_ = bufBeg_ + is.gcount();
		}
	}
	return is;
}

	
template<class T>
typename circular_buffer<T>::size_type 
circular_buffer<T>::seekoff(typename circular_buffer<T>::size_type offset)
{
	iterator iterFrom(*this, cur_), iterTo(*this, cur_);
	iterTo += offset;
	cur_ = iterTo.cur_;
	return iterTo - iterFrom;
}

template<class T>
typename circular_buffer<T>::iterator 
circular_buffer<T>::begin()
{
	return iterator(*this, beg_);
}

template<class T>
typename circular_buffer<T>::iterator const 
circular_buffer<T>::begin() const
{
	return iterator(*this, beg_);
}
	
template<class T>
typename circular_buffer<T>::iterator 
circular_buffer<T>::end()
{
	return iterator(*this, end_);
}

template<class T>
typename circular_buffer<T>::iterator const 
circular_buffer<T>::end() const
{
	return iterator(*this, end_);
}

template<class T>
typename circular_buffer<T>::iterator 
circular_buffer<T>::current()
{
	return iterator(*this, cur_);
}

template<class T>
typename circular_buffer<T>::iterator const 
circular_buffer<T>::current() const
{
	return iterator(*this, cur_);
}

template<class Elem>
std::basic_ostream<Elem> &operator<<(std::basic_ostream<Elem>& os, circular_buffer<Elem>& cb)
{
	if( cb.end_ > cb.cur_ ){
		os.write(cb.cur_, cb.end_ - cb.cur_);
	}else if(cb.end_ < cb.cur_){
		os.write(cb.cur_, cb.bufEnd_ - cb.cur_+1);
		os.write(cb.bufBeg_, cb.end_ - cb.bufBeg_);
	}
	return os;
}

template<class Elem>
std::basic_ostream<Elem> &operator<<(
	std::basic_ostream<Elem>& os, 
	std::pair< circular_iterator<Elem>, circular_iterator<Elem> > range )
{
	typename circular_iterator<Elem>::pointer first = range.first.cur_,
						  last = range.second.cur_;
	circular_buffer<Elem> const *cb = range.first.cb_;

	if(last > first){
		os.write(first, last - first);
	}else if(last < first){
		os.write(first, cb->bufEnd_ - first + 1);
		os.write(cb->bufBeg_, last - cb->bufBeg_);
	}
	return os;
}

