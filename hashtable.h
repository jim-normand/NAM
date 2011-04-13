/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <map>
#include <set>

#include "blob.h"

typedef std::set<unsigned> hashlist;
typedef std::map<unsigned, hashlist> hashtable;


class HashTable
{
public:
	bool Set(const unsigned index, const unsigned ptid);
	unsigned ComputeID(const blob *target) const;
	void Clear();

private:
	hashtable m_table;
};

#endif