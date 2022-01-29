//
// Created by user1 on 2021/12/20.
//
#include "gfa_reader.h"
#include "kmer_counter.h"
#include "kmer_coder.h"
#include "lockfree_hash_map.h"
#include "mem_allocator.h"


int main()
{
    gjfish::Param param;
    auto *reader = new gjfish::GFAReader("../test/GRCh38-20-0.10b.gfa", param);
    auto *ma = new gjfish::MemAllocator(100000);

    reader->Start();
    reader->GenerateSuperSeg();

    auto *counter = new gjfish::KmerCounter(ma, reader);

    counter->StartCount();
    return 0;
}