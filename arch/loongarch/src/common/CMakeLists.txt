# ##############################################################################
# arch/loongarch/src/common/CMakeLists.txt
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed to the Apache Software Foundation (ASF) under one or more contributor
# license agreements.  See the NOTICE file distributed with this work for
# additional information regarding copyright ownership.  The ASF licenses this
# file to you under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#
# ##############################################################################

set(SRCS)

list(APPEND SRCS la64_saveusercontext.S la64_vectors.S)

list(APPEND SRCS la64_initialize.c la64_initialstate.c)
list(APPEND SRCS la64_nputs.c la64_createstack.c)
list(APPEND SRCS la64_releasestack.c la64_stackframe.c la64_usestate.c)
list(APPEND SRCS la64_exit.c)
list(APPEND SRCS la64_getintstack.c la64_registerdump.c)
list(APPEND SRCS la64_switchcontext.c la64_saveusercontext.S)

list(APPEND SRCS la64_vectors.S la64_exception.c la64_doirq.c)
list(APPEND SRCS la64_tlb_refill.S, la64_fatal.S)

list(APPEND SRCS la64_percpu.c)

list(APPEND SRCS la64_idle.c)

list(APPEND SRCS la64_sigdeliver.c la64_schedulesigaction.c)

list(APPEND SRCS la64_allocateheap.c )
list(APPEND SRCS la64_stable_timer.c)

list(APPEND SRCS la64_debug.c)

target_sources(arch PRIVATE ${SRCS})
