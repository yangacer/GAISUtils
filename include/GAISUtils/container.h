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

#ifndef _CONTAINER_H
#define _CONTAINER_H

#include <istream>
#include <ostream>
#include <iterator>
#include <utility>

/// Forwarded declaration
template<class T>
class circular_buffer;

/// Iterator for iterating a circular buffer
template<class T>
class circular_iterator
	: public std::iterator< std::random_access_iterator_tag, T>
{
	template<class U> friend class circular_buffer;
	template<class Elem> friend std::basic_ostream<Elem> &operator<<
	(
	        std::basic_ostream<Elem>&, 
		std::pair< circular_iterator<Elem> ,  circular_iterator<Elem> > 
	);

public:
	typedef std::iterator< std::random_access_iterator_tag, T> base_;

	typedef typename base_::value_type value_type;
	typedef typename base_::difference_type difference_type;
	typedef typename base_::difference_type distance_type;
	typedef typename base_::reference reference;
	typedef typename base_::pointer pointer;

	/** @param cb A reference to a circular buffer
	 *  @param cur The position to initiate this iterator
	 */
	circular_iterator(const circular_buffer<T>& cb, pointer cur);
	circular_iterator();

	/** @param cp Constant reference to be copied
	 */
	circular_iterator& operator=(const circular_iterator& cp);

	/** @param offset An offset to advance pointer
	 */
	circular_iterator &operator += (distance_type offset);

	/** @param offset An offset to move current pointer backward
	 */
	circular_iterator &operator -= (distance_type offset);

	/** Return true if the circular buffer used to initinate this iterator
	 *  has not been chanaged.
	 */
	bool isValid();

	/** @param offset An offset to advance current pointer
	 */
	circular_iterator operator + (distance_type offset) const;

	/** @param offset An offset to back off current pointer
	 */
	circular_iterator operator - (distance_type offset) const;
	
	
	circular_iterator &operator ++();

	circular_iterator operator++(int);

	circular_iterator &operator--();

	circular_iterator operator--(int);

	/** @param from An iterator to be subtract from this param
	 */
	difference_type operator - (const circular_iterator &from) const;
	
	bool operator ==(const circular_iterator &rhs) const;
	bool operator !=(const circular_iterator &rhs) const;
	bool operator <(const circular_iterator &rhs) const;
	bool operator >(const circular_iterator &rhs) const;
	bool operator <=(const circular_iterator &rhs) const;
	bool operator >=(const circular_iterator &rhs) const;

	reference operator *();

private:
	const circular_buffer<T> *cb_;
	pointer cur_;
	pointer beg_;
};

///An circular buffer that eliminates time consuming of moving memory
/**
 *  @see container.cpp
 *  for an example
 */
template<class Elem>
class circular_buffer
{
	template<class T> friend class circular_iterator;
	template<class E> friend std::basic_ostream<E>& operator<<(std::basic_ostream<E>& , circular_buffer<E>&); 
	template<class E> friend std::basic_ostream<E> &operator<<
	(
	        std::basic_ostream<E>&, 
		std::pair< circular_iterator<E> ,  circular_iterator<E> > 
	);
	
public:
	typedef Elem* pointer;
	typedef Elem& reference;
	typedef Elem value_type;
	typedef unsigned int size_type;
	typedef circular_iterator<Elem> iterator;

	circular_buffer();

	/** @param size Buffer size
	 *  @remark A circular buffer uses (size + 1)*sizeof(Elem) actually
	 */
	circular_buffer(size_type size);

	~circular_buffer();

	/** 
	 * @param size New size
	 */
	void resize(size_type size);
	
	/** 
	 *  @return Maximum size of buffer.
	 */
	size_type capacity() const;
	
	/** 
	 *  @return Current used buffer size
	 */
	size_type size() const;
	
	/** Clean buffer content
	 */
	void clear();

	/** @param is An input stream reference to read data into this buffer
	 *  @return The input stream reference passed in
	 */
	std::basic_istream<value_type>& tryfill(std::basic_istream<value_type>& is);

	/** @param offset An offset to move current read pointer advance
	 *  @return offset or end pointer - current pointer
	 */
	size_type seekoff(size_type offset);

	/** @return A begin iterator of this buffer
	 */
	iterator begin();

	/** @return Const begin iterator
	 */
	const iterator begin() const;
	
	/** @return A end iterator of this buffer
	 */
	iterator end();

	/** @return Const end iterator
	 */
	const iterator end() const;

	/** @return Iterator of the next one to the last read
	 */
	iterator current();
	
	/** @return Const iterator of the next one to the last read
	 */
	const iterator current() const;
private:
	pointer bufBeg_, bufEnd_;
	pointer beg_, cur_, end_;
};

/// Operator<< for std::ostream and circular_buffer
/** Optimized for sequential access
 */
template<class Elem>
std::basic_ostream<Elem> &operator<<(std::basic_ostream<Elem>& os, circular_buffer<Elem>& cb);

/// Operator<< for std::ostream and a range composed by two circular_iterators.
/** Optimized for sequential access
*/
template<class Elem>
std::basic_ostream<Elem> &operator<<(
	std::basic_ostream<Elem>& os, 
	std::pair< circular_iterator<Elem>, circular_iterator<Elem> > range );

#include "container.inl"

#endif
