#ifndef _ASM_H
#define _ASM_H
	
#include "image_processing.h"
	
using namespace std;
using namespace image_processing;
	
namespace image_processing {
	struct Match {
		int start, end;
		double edit_distance;
		Match(int s, int e, double ed=0) : start(s), end(e), edit_distance(ed) {}
		bool overlaps(Match);
	};
	bool Match::overlaps(Match m) {
		return (m.start>=start && m.start<=end || m.end>=start && m.end<=end);
	}
	typedef vector<Match> MatchList;
	
	MatchList approximateStringMatch(Sequence text, Sequence pattern, double n=0, double** cost=NULL, bool overlapping=true) {
		MatchList match;
		double d[pattern.size()+1][text.size()+1];
		for(int t=0; t<=text.size(); t++)
			d[0][t]=0;
		for(int p=1; p<=pattern.size(); p++)
			d[p][0]=p;
		for(int p=1; p<=pattern.size(); p++) {
			for(int t=1; t<=text.size(); t++) {
				double c=cost ? cost[p-1][t-1] : (text[t-1]==pattern[p-1] ? 0 : 1);
				d[p][t]=min(d[p-1][t-1]+c, d[p][t-1]+1.0, d[p-1][t]+1.0);
			}
		}
		for(int t=1; t<=text.size(); t++)
			if(d[pattern.size()][t]<=n)
				match.push_back(Match(t-1, t-1, d[pattern.size()][t]));
		for(MatchList::iterator i=match.begin(); i!=match.end(); i++) {
			for(int t=i->end+1; t>=0; t--)
				d[pattern.size()][t]=0;
			for(int p=pattern.size()-1; p>=0; p--)
				d[p][i->end+1]=pattern.size()-p;
			for(int p=pattern.size()-1; p>=0; p--) {
				for(int t=i->end; t>=0; t--) {
					double c=cost ? cost[p][t] : (text[t]==pattern[p] ? 0 : 1);
					d[p][t]=min(d[p+1][t+1]+c, d[p][t+1]+1.0, d[p+1][t]+1.0);
				}
			}
			for(int t=i->end+1; t>=0; t--)
				if(round(d[0][t]*1e6)==round(i->edit_distance*1e6)) {
					i->start=t;
					break;
				}
		}
		if(!overlapping) {
			MatchList::iterator i=match.begin();
			Match last=*i;
			for(i++; i!=match.end(); ) {
				if(last.overlaps(*i)) {
					if(last.edit_distance<i->edit_distance)
						i=match.erase(i);
					else {
						i--;
						i=match.erase(i);
						last=*i;
						i++;
					}
				} else {
					last=*i;
					i++;
				}
			}
		}
		return match;
	}
	MatchList approximateStringMatch(double** cost, int textsize, int patternsize, double n=0, bool overlapping=true) {
		MatchList match;
		double d[patternsize+1][textsize+1];
		for(int t=0; t<=textsize; t++)
			d[0][t]=0;
		for(int p=1; p<=patternsize; p++)
			d[p][0]=p;
		for(int p=1; p<=patternsize; p++) {
			for(int t=1; t<=textsize; t++) {
				double c=cost[p-1][t-1];
				d[p][t]=min(d[p-1][t-1]+c, d[p][t-1]+1.0, d[p-1][t]+1.0);
			}
		}
		for(int t=1; t<=textsize; t++)
			if(d[patternsize][t]<=n)
				match.push_back(Match(t-1, t-1, d[patternsize][t]));
		for(MatchList::iterator i=match.begin(); i!=match.end(); i++) {
			for(int t=i->end+1; t>=0; t--)
				d[patternsize][t]=0;
			for(int p=patternsize-1; p>=0; p--)
				d[p][i->end+1]=patternsize-p;
			for(int p=patternsize-1; p>=0; p--) {
				for(int t=i->end; t>=0; t--) {
					double c=cost[p][t];
					d[p][t]=min(d[p+1][t+1]+c, d[p][t+1]+1.0, d[p+1][t]+1.0);
				}
			}
			for(int t=i->end+1; t>=0; t--)
				if(round(d[0][t]*1e6)==round(i->edit_distance*1e6)) {
					i->start=t;
					break;
				}
		}
		if(!overlapping) {
			MatchList::iterator i=match.begin();
			Match last=*i;
			for(i++; i!=match.end(); ) {
				if(last.overlaps(*i)) {
					if(last.edit_distance<i->edit_distance)
						i=match.erase(i);
					else {
						i--;
						i=match.erase(i);
						last=*i;
						i++;
					}
				} else {
					last=*i;
					i++;
				}
			}
		}
		return match;
	}
}

#endif