/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Copyright (c) 1997
 * Moscow Center for SPARC Technology
 *
 * Copyright (c) 1999
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

#ifndef _STLP_INTERNAL_STACK_H
#define _STLP_INTERNAL_STACK_H

#ifndef _STLP_INTERNAL_DEQUE_H
#  include <stl/_deque.h>
#endif

namespace _STLP_STD {

template <class _Tp, class _Sequence = deque<_Tp> >
class stack
{
  typedef stack<_Tp, _Sequence> _Self;

public:
  typedef typename _Sequence::value_type      value_type;
  typedef typename _Sequence::size_type       size_type;
  typedef          _Sequence                  container_type;

  typedef typename _Sequence::reference       reference;
  typedef typename _Sequence::const_reference const_reference;
protected:
  //c is a Standard name (23.2.3.3), do no make it STLport naming convention compliant.
  _Sequence c;
public:
  stack() : c() {}
  explicit stack(const _Sequence& __s) : c(__s) {}

  stack(__move_source<_Self> src)
    : c(_STLP_PRIV::_AsMoveSource(src.get().c)) {}

  bool empty() const { return c.empty(); }
  size_type size() const { return c.size(); }
  reference top() { return c.back(); }
  const_reference top() const { return c.back(); }
  void push(const value_type& __x) { c.push_back(__x); }
  void pop() { c.pop_back(); }
  const _Sequence& _Get_s() const { return c; }
};

template < class _Tp, class _Sequence >
inline bool  operator==(const stack< _Tp, _Sequence >& __x,
                                   const stack< _Tp, _Sequence >& __y)
{ return __x._Get_s() == __y._Get_s(); }

template < class _Tp, class _Sequence >
inline bool  operator<(const stack< _Tp, _Sequence >& __x,
                                  const stack< _Tp, _Sequence >& __y)
{ return __x._Get_s() < __y._Get_s(); }

#define _STLP_STACK_ARGS _Tp, _Sequence
#define _STLP_STACK_HEADER_ARGS class _Tp, class _Sequence
_STLP_RELOPS_OPERATORS(template < _STLP_STACK_HEADER_ARGS >, stack< _STLP_STACK_ARGS >)
#undef _STLP_STACK_ARGS
#undef _STLP_STACK_HEADER_ARGS

template <class _Tp, class _Sequence>
struct __move_traits<stack<_Tp, _Sequence> > :
  _STLP_PRIV::__move_traits_aux<_Sequence>
{};

}

#endif /* _STLP_INTERNAL_STACK_H */

// Local Variables:
// mode:C++
// End:
