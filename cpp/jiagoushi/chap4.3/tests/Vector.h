/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _VECTOR_UTIL_H
#define _VECTOR_UTIL_H

#include <vector>

// function to conveniently stream vectors
template <class U>
std::ostream& operator<<(std::ostream& os,const std::vector<U>& c){
  typedef std::vector<U> V;
  os<<"[";
  if(c.size()>0){
      for(typename V::const_iterator it=c.begin();it!=c.end();++it)
          os<<*it<<",";
      os.seekp(-1,std::ios::cur);
  }
  os<<"]";
  return os;
}

#endif // _VECTOR_UTIL_H
x::~Mutex() { delete impl_;}
void Mutex::acquire() {
    LIBC_SYMBOLS.pthread_mutex_lock(&impl_->mut_);
}
void Mutex::release() {
    LIBC_SYMBOLS.pthread_mutex_unlock(&impl_->mut_);
}

// ****************************************************************************
// Atomics
int32_t atomic_post_incr(volatile int32_t* operand, int32_t incr)
{
#if defined(__GNUC__)
    return __sync_fetch_and_add(operand,incr);
#else
    int32_t result;
    __asm__ __volatile__(
         "lock xaddl %0,%1\n"
         : "=r"(result), "=m"(*operand)
         : "0"(incr)
         : "memory");
   return result;
#endif
}
int32_t atomic_fetch_store(volatile int32_t *ptr, int32_t value)
{
#if defined(__GNUC__)
    return __sync_lock_test_and_set(ptr,value);
#else
    int32_t result;
    __asm__ __volatile__("lock xchgl %0,%1\n"
                          : "=r"(result), "=m"(*ptr)
                          : "0"(value)
                          : "memory");
   return result; 
#endif
}
#else
int32_t atomic_post_incr(volatile int32_t* operand, int32_t incr){
    int32_t v=*operand;
    *operand+=incr;
    return v;
}
int32_t atomic_fetch_store(volatile int32_t *ptr, int32_t value)
{
    int32_t result=*ptr;
    *ptr=value;
    return result;
}
#endif // THREADED
