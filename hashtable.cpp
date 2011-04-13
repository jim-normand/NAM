/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#include "hashtable.h"

bool HashTable::Set(const unsigned index, const unsigned ptid)
{
	hashtable::iterator itin = m_table.find(index);

	if(itin == m_table.end()){
		hashlist tmp;
		tmp.insert(ptid);
		m_table.insert(std::pair<unsigned, hashlist>(index, tmp));
	}
	else{
		hashlist::iterator itpt = (*itin).second.find(ptid);
		if(itpt == (*itin).second.end()){
			(*itin).second.insert(ptid);
		}
		else{
			return false;
		}	
	}

	return true;
}


unsigned HashTable::ComputeID(const blob *target) const
{
	typedef std::map<unsigned, int> evaluation;
	evaluation result;

	for(descriptors::const_iterator itta=target->descs.begin();itta!=target->descs.end();++itta){

		hashtable::const_iterator itha = m_table.find(*itta);

		if(itha != m_table.end()){

			for(hashlist::const_iterator itli=(*itha).second.begin();itli!=(*itha).second.end();++itli){
				
				evaluation::iterator itpt = result.find(*itli);

				if(itpt == result.end()){
					result.insert(std::pair<unsigned, int>(*itli, 1));
				}
				else{
					(*itpt).second++;
				}
			}
		}
	}

	unsigned index;

	if(static_cast<int>(result.size()) != 0){
		index = (*result.begin()).first;
		int count = (*result.begin()).second;

		for(evaluation::iterator itre=result.begin();itre!=result.end();++itre){
			if(count < (*itre).second){
				count = (*itre).second;
				index= (*itre).first;
			}
		}
	}
	else{
		index = NOID;
	}

	return index;
}

void HashTable::Clear()
{
	m_table.clear();
}