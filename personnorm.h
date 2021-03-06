// Library for I/O of genetic data
// Author: Amy Williams <alw289  cornell edu>
//
// This program is distributed under the terms of the GNU General Public License

#include "marker.h"
#include "superperson.h"
#include "personio.h"
#include "dynarray.h"

#ifndef PERSONNORM_H
#define PERSONNORM_H

struct Genotype {
  int & operator[] (int h) { return a[h]; }

  // The two alleles of this Genotype
  int a[2];
};

class PersonNorm : public SuperPerson {
  public:
    //////////////////////////////////////////////////////////////////
    // public static methods
    //////////////////////////////////////////////////////////////////

    static PersonNorm * lookupId(char *id) { return _idToPerson.lookup(id); }

    friend class PersonIO<PersonNorm>;

    //////////////////////////////////////////////////////////////////
    // public methods
    //////////////////////////////////////////////////////////////////

    PersonNorm(char *id, char sex, int popIndex, short familyIdLength = 0);
    ~PersonNorm();

    int getGenotype(int chunkNum, int chunkIdx, int chromIdx,
		    int chromMarkerIdx) {
      int markerNum = Marker::getFirstMarkerNum(chromIdx) + chromMarkerIdx;
      assert(Marker::getMarker(markerNum)->getNumAlleles() <= 2);
      return _geno[chromIdx][chromMarkerIdx][0] +
					    _geno[chromIdx][chromMarkerIdx][1];
    }
    int getHapAllele(int homolog, int chunkNum, int chunkIdx, int chromIdx,
		     int chromMarkerIdx) {
      return _geno[chromIdx][chromMarkerIdx][homolog];
    }

    // Note: these next two methods are currently only used by PersonIO:
    // isUnrelated() really doesn't apply for PersonNorm, which is
    // intended to be used for family-based phasing; samples are not
    // from a population
    bool isUnrelated() { return false; }
    bool isPhased() { return true; }

    //////////////////////////////////////////////////////////////////
    // public static variables
    //////////////////////////////////////////////////////////////////

    static dynarray<PersonNorm *> _allIndivs;
    // Hash from PersonNorm ids to PersonNorm *
    static Hashtable<char *, PersonNorm *> _idToPerson;

  private:
    //////////////////////////////////////////////////////////////////
    // private static methods
    //////////////////////////////////////////////////////////////////

    // Nothing to clean up after parsing all the persons; the one
    // parameter to this function applies to a different Person class
    static void cleanUpPostParse(bool dontcare) { }

    //////////////////////////////////////////////////////////////////
    // private methods
    //////////////////////////////////////////////////////////////////

    void setGenotype(int hapChunkNum, int chunkIdx, int chromIdx,
		     int chromMarkerIdx, int geno[2]);
    void setParents(char *familyid, PersonNorm *parents[2],
		    int numParents, bool &warningPrinted, FILE *log,
		    int *numMendelError, int *numMendelCounted);
    // Used for males on the X chromosome: heterozygous sites are erroneous
    void setXHetToMissing(FILE *log, int *numHets = NULL, int *numCalls = NULL);

    //////////////////////////////////////////////////////////////////
    // private variables
    //////////////////////////////////////////////////////////////////

    // Stores genotypes for <this>.  Genotypes are indexed first by chromosome
    // then by marker number.  Markers are ordered by physical position on each
    // chromosome.
    Genotype **_geno;

    // Pointers to parents of <this>. _parents[0] is the father,
    // _parents[1] is the mother
    PersonNorm *_parents[2];
};

#endif // PERSONNORM_H
