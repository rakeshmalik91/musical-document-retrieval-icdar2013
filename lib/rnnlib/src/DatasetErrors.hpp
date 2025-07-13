/*Copyright 2009,2010 Alex Graves

This file is part of RNNLIB.

RNNLIB is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RNNLIB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RNNLIB.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef _INCLUDED_DatasetErrors_h  
#define _INCLUDED_DatasetErrors_h  

#include "Helpers.hpp"

struct DatasetErrors
{
	//data
	map<string, real_t> errors;
	map<string, real_t> normFactors;
	static set<string> labelErrors;
	static set<string> wordErrors;
	static set<string> percentErrors;
	
	//functions
	DatasetErrors(){}
	void clear()
	{
		errors.clear();
		normFactors.clear();
	}
	void add_seq_errors(const map<string, real_t>& seqErrors, const map<string, real_t>& seqNorms)
	{
		LOOP(const PSD& p, seqErrors)
		{
			const string& name = p.first;
			errors[name] += p.second;
			real_t normFactor = 1;
			if (in(labelErrors, name))
			{
				normFactor = at(seqNorms, string("label"));
			}
			else if (in(wordErrors, name))
			{
				normFactor = at(seqNorms, string("word"));			
			}
			normFactors[name] += normFactor;
		}
	}
	void normalise()
	{
		LOOP(PCSD& p, errors)
		{
			real_t normFactor = normFactors[p.first];
			if (normFactor)
			{
				p.second /= normFactor;
				if (in(percentErrors, p.first)/* || p.first[0] == '_'*/)
				{
					p.second *= 100;
				}				
			}
			normFactors[p.first] = 0;
		}
	}
	void print(ostream& out) const
	{
		LOOP(const PSD& p, errors)
		{
			out << p;
			if (in(percentErrors, p.first)/* || p.first[0] == '_'*/)
			{
				out << "%";
			}
			out << endl;
		}
	}
};
set<string> DatasetErrors::labelErrors = list_of("labelError")("deletions")("insertions")("substitutions");
set<string> DatasetErrors::wordErrors = list_of("wordError")("wordDeletions")("wordInsertions")("wordSubstitutions");		
set<string> DatasetErrors::percentErrors = list_of("classificationError")("seqError")("wordSeqError")
													("labelError")("deletions")("insertions")("substitutions")
													("wordError")("wordDeletions")("wordInsertions")("wordSubstitutions");

static ostream& operator << (ostream& out, const DatasetErrors& de)
{
	de.print(out);
	return out;
}

typedef pair<const string, pair<int, DatasetErrors> > PSPIDE;

#endif
